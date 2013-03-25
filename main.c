/* 
 * File:   main.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "astar.h"
#include "list.h"
#include "path.h"
#include "places.h"
#include "mines.h"

// Import extern variables
extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];
extern int places_to_visit[3][3];
extern int mine_locations[NUMBER_OF_MINES][4];
extern int no_total_path_possible;

/**
 * Clears screen
 * This functions clears the console using the special commands.
 */

void cls() {
    printf("\033[2J\033[0;0f");
    printf("\033[%d;%df", 0, 0);
}

/**
 * Swaps the value of two integers.
 * @param a <int> first value
 * @param b <int> second value
 */

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Runs the program.
 * @return <int> status
 */

int main() {
    // Initializations
    Position start, destination;
    Path_element *path, *robot_path;
    int initial_facing_direction;
    int x, y, i;
    static int first_run = 1;

    if (first_run) {
	first_run = 0;

	// Get which places to user wants to visit
	getPlacesToVisit();
    }

    // Reset no_total_path_possible
    no_total_path_possible = 0;

    // Unvisit all places
    unvisitAll();

    // Seed the rand() function
    srand(time(NULL));

    // Randomize facing direction
    initial_facing_direction = rand() % 4 + 1;

    // Initialize grid
    generateGrid();

    // Create mines
    createMines();

    // Define the start position in the bottom left
    start.x = destination.x = 0;
    start.y = destination.y = 0;

    // Discover mines at start location
    discoverMines(start.x, start.y);

    // Get first destination
    destination.x = places_to_visit[getTargetPlace(start.x, start.y, initial_facing_direction, 0)][0];
    destination.y = places_to_visit[getTargetPlace(start.x, start.y, initial_facing_direction, 0)][1];

    // Create the robot_path linked list
    robot_path = malloc(sizeof (Path_element));
    robot_path->x = start.x;
    robot_path->y = start.y;
    robot_path->facing_direction = initial_facing_direction;
    robot_path->next = NULL;

    // Mark places to visit
    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	grid[places_to_visit[i][0]][places_to_visit[i][1]]->mark = 49 + i;
    }

    // Find initial path
    path = findShortestPath(start.x, start.y, initial_facing_direction, destination.x, destination.y);

    // If path == NULL, there was no path found
    if (!path || no_total_path_possible) {
	// Clear screen, print the grid, display a message, sleep for a while and then rerun the program
	cls();
	printGrid(robot_path);
	printf("\nCouldn't find a path...\n");
	getchar();
	main();
	return 0;
    }

    // Record start
    x = start.x;
    y = start.y;

    // Display first move
    cls();
    printGrid(robot_path);
    getchar();

    // Infinite loop until broken
    while (1) {
	// Move and save position and step weight, also add the position to the path
	path = path->next;
	addToEndOfPath(&robot_path, path->x, path->y);
	x = path->x;
	y = path->y;

	// Keep tracking which locations are visited
	visit(x, y);

	// When all places are visited, break the loop
	if (visitedAllPlaces()) break;

	// Check for mines
	discoverMines(x, y);

	// Get destination
	destination.x = places_to_visit[getTargetPlace(x, y, path->facing_direction, 0)][0];
	destination.y = places_to_visit[getTargetPlace(x, y, path->facing_direction, 0)][1];

	// Calculate new path, destination could be changed
	path = findShortestPath(x, y, path->facing_direction, destination.x, destination.y);

	// If path == NULL, there was no path found
	if (!path || no_total_path_possible) {
	    // Clear screen, print the grid, display a message, sleep for a while and then rerun the program
	    cls();
	    printGrid(robot_path);
	    printf("\nCouldn't find a path...\n");
	    getchar();
	    main();
	    return 0;
	}

	// Display path and add step weight to path weight
	cls();
	printGrid(robot_path);
	printf("\nDestination: (%d,%d)\n", destination.x, destination.y);
	getTargetPlace(x, y, path->facing_direction, DEBUG);
	getchar();
    }

    // Reveal mines
    revealMines();

    // Display final screen
    cls();
    printGrid(robot_path);
    printf("\nDone.\n");
    getchar();

    // Rerun program
    main();

    return EXIT_SUCCESS;
}

