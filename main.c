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

// Import the grid from another file and define the mine locations
extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];
extern int places_to_visit[3][3];
int mine_locations[NUMBER_OF_MINES][4];
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
 * Creates the mines in the grid.
 */

void createMines() {
    int available_mine_spots[GRID_SIZE_X * GRID_SIZE_Y * 2][4];
    int x, y, i, j, index = 0;

    // Find available spots by walking through the grid and saving each connection of a node with his south and east neightbour.
    for (x = 0; x < GRID_SIZE_X; x++) {
	for (y = 0; y < GRID_SIZE_Y; y++) {
	    // Check if the right neighbour exists and act accordingly
	    if (grid[x + 1][y] && x < GRID_SIZE_X - 1) {
		available_mine_spots[index][0] = x;
		available_mine_spots[index][1] = y;
		available_mine_spots[index][2] = x + 1;
		available_mine_spots[index++][3] = y;
	    }

	    // Check if the bottom neighbour exists and act accordingly
	    if (grid[x][y + 1] && y < GRID_SIZE_Y - 1) {
		available_mine_spots[index][0] = x;
		available_mine_spots[index][1] = y;
		available_mine_spots[index][2] = x;
		available_mine_spots[index++][3] = y + 1;
	    }
	}
    }

    // Shuffle array by using the Fisher-Yates shuffle algorithm
    for (i = index - 1; i > 0; i--) {
	j = rand() % (i + 1);
	// Swap
	swap(&available_mine_spots[i][0], &available_mine_spots[j][0]);
	swap(&available_mine_spots[i][1], &available_mine_spots[j][1]);
	swap(&available_mine_spots[i][2], &available_mine_spots[j][2]);
	swap(&available_mine_spots[i][3], &available_mine_spots[j][3]);
    }

    // Save the first NUMBER_OF_MINES mines in the mine_locations array, using the shuffled array
    for (i = 0; i < NUMBER_OF_MINES; i++) {
	mine_locations[i][0] = available_mine_spots[i][0];
	mine_locations[i][1] = available_mine_spots[i][1];
	mine_locations[i][2] = available_mine_spots[i][2];
	mine_locations[i][3] = available_mine_spots[i][3];
    }
}

/**
 * Checks if there is a mine at the given location.
 * @param x1 <int> x of the first node
 * @param y1 <int> y of the first node
 * @param x2 <int> x of the second node
 * @param y2 <int> y of the second node
 * @return <int> boolean wether a mine is at the given location
 */

int mineAtConnection(int x1, int y1, int x2, int y2) {
    int i;

    // First check if the connection is valid
    if (checkConnection(x1, y1, x2, y2)) {
	// Now walk through all the mines and check if a location matches
	for (i = 0; i < NUMBER_OF_MINES; i++) {
	    if (
		    (mine_locations[i][0] == x1 && mine_locations[i][1] == y1 && mine_locations[i][2] == x2 && mine_locations[i][3] == y2) ||
		    (mine_locations[i][0] == x2 && mine_locations[i][1] == y2 && mine_locations[i][2] == x1 && mine_locations[i][3] == y1)
		    ) {
		return 1;
	    }
	}
    }

    // No value returned before, so no mine at the given location, so return 0
    return 0;
}

/**
 * Discovers mines at a given point.
 * @param x <int> x of the given location
 * @param y <int> y of the given location
 */

int discoverMines(int x, int y) {
    // Define return value
    int return_value = 0;

    // Now check north, south, east and west of the node if there is a mine.
    // If a mine is found, remove the connection and set the return value to 1.
    if (mineAtConnection(x, y, x, y + 1)) {
	removeConnection(x, y, x, y + 1);
	return_value = 1;
    }

    if (mineAtConnection(x, y, x + 1, y)) {
	removeConnection(x, y, x + 1, y);
	return_value = 1;
    }

    if (mineAtConnection(x, y, x - 1, y)) {
	removeConnection(x, y, x - 1, y);
	return_value = 1;
    }

    if (mineAtConnection(x, y, x, y - 1)) {
	removeConnection(x, y, x, y - 1);
	return_value = 1;
    }

    return return_value;
}

/**
 * Reveals all mines.
 */

void revealMines() {
    int i;

    // Loop through all the mine locations and then removing the connections in which the mines are located
    for (i = 0; i < NUMBER_OF_MINES; i++) {
	removeConnection(mine_locations[i][0], mine_locations[i][1], mine_locations[i][2], mine_locations[i][3]);
    }
}

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
	getTargetPlace(x, y, path->facing_direction, 1);
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

