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

// Import the grid from another file and define the mine locations
extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];
int mine_locations[NUMBER_OF_MINES][4];

/**
 * Clears screen.
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
    // Swap variables
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Creates the mines in the grid.
 */

void createMines() {
    // Initiate the variables used
    int available_mine_spots[GRID_SIZE_X * GRID_SIZE_Y * 2][4];
    int x, y, i, j, index = 0;

    // Find available spots by walking through the grid and saving each connection of a node with his south and right neightbour.
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
    Path_element *path, *robot_path, *shortest_path;
    int initial_facing_direction;
    int path_weight = 0, step_weight, shortest_path_weight;
    int x, y;
    static int first_run = 1;
    
    // Display a message which explains what the user is about to see. Do this to avoid confusion.
    if (first_run) {
        first_run = 0;
        
        printf("On the following screen a grid will be shown in which the robot, marked with the letter 'R', is able to move to his destination, marked 'G'. There are also %d mines places randomly. When the robot moves to a location in the grid which is connected to one or more mines, the mine will explode and the robot will be unable to move that way. The robot will try to move as fast as possible from the start to his destination without having foreknowledge of the locations of the mines. Statistics will be shown beneath the grid.\n\nPress enter to conitue...", NUMBER_OF_MINES);
        getchar();
    }

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

    // Get a random destination which is not equal to the start position by
    // generating a random destination while the destination is equal to the start
    while (destination.x == start.x && destination.y == start.y) {
        destination.x = rand() % 5;
        destination.y = rand() % 5;
    }

    // Discover mines at start location
    discoverMines(start.x, start.y);

    // Create the robot_path linked list
    robot_path = malloc(sizeof (Path_element));
    robot_path->x = start.x;
    robot_path->y = start.y;
    robot_path->facing_direction = initial_facing_direction;
    robot_path->next = NULL;

    // Mark the destination node with a 'G' ('G'oal)
    grid[destination.x][destination.y]->mark = 'G';

    // Find initial path
    path = findShortestPath(start.x, start.y, initial_facing_direction, destination.x, destination.y);

    // If path == NULL, there was no path found
    if (!path) {
        // Clear screen, print the grid, display a message, sleep for a while and then rerun the program
        cls();
        printGrid(robot_path);
        printf("\nCouldn't find a path...\n");
        sleep(2);
        main();
        return 0;
    }

    // Record start
    x = start.x;
    y = start.y;

    // Display first move
    cls();
    printGrid(robot_path);
    sleep(1);

    // Infinite loop until broken
    while (1) {
        // Move and save position and step weight, also add the position to the path
        path = path->next;
        addToEndOfPath(&robot_path, path->x, path->y);
        x = path->x;
        y = path->y;
        step_weight = path->step_weight;

        // Check if at destination
        if (x == destination.x && y == destination.y) {
            // Destination reached, end the loop
            path_weight += step_weight;
            break;
        }

        // Check for mines
        if (discoverMines(x, y)) {
            // A mine was found, a new path has to be calculated
            path = findShortestPath(x, y, path->facing_direction, destination.x, destination.y);

            // If path == NULL, there was no path found
            if (!path) {
                // Clear screen, print the grid, display a message, sleep for a while and then rerun the program
                cls();
                printGrid(robot_path);
                printf("\nCouldn't find a path...\n");
                sleep(2);
                main();
                return 0;
            }
        }

        // Display path and add step weight to path weight
        path_weight += step_weight;
        cls();
        printGrid(robot_path);
        printf("\nCurrent step weight: %03d\nTotal path weight: %03d\n", step_weight, path_weight);
        sleep(1);
    }

    // Reveal mines
    revealMines();

    // Calculate shortest path
    shortest_path = findShortestPath(start.x, start.y, initial_facing_direction, destination.x, destination.y);

    // Set variable
    shortest_path_weight = 0;

    // Walk through the pad, adding all the step weights and determining that way the total weight of the path
    while (shortest_path) {
        shortest_path_weight += shortest_path->step_weight;
        shortest_path = shortest_path->next;
    }


    // Display final screen
    cls();
    printGrid(robot_path);
    printf("\nCurrent step weight: %03d\nFinal path weight: %03d\nShortest path weight: %03d\nEfficiency: %0.2lf%%\n", step_weight, path_weight, shortest_path_weight, round((float) 100 * shortest_path_weight / path_weight));
    sleep(3);
    
    // Rerun program
    main();

    return EXIT_SUCCESS;
}

