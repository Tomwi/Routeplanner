/* 
 * File:   main.c
 * Author: Wessel Bruinsma
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "astar.h"
#include "list.h"
#include "path.h"

#define NUMBER_OF_MINES 13

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
 * Create mines .
 */

void createMines() {
    int available_mine_spots[GRID_SIZE_X * GRID_SIZE_Y * 2][4];
    int x, y, i, j, index = 0;

    // Find available spots
    for (x = 0; x < GRID_SIZE_X; x++) {
        for (y = 0; y < GRID_SIZE_Y; y++) {
            if (grid[x + 1][y] && x < GRID_SIZE_X - 1) {
                available_mine_spots[index][0] = x;
                available_mine_spots[index][1] = y;
                available_mine_spots[index][2] = x + 1;
                available_mine_spots[index++][3] = y;
            }

            if (grid[x][y + 1] && y < GRID_SIZE_Y - 1) {
                available_mine_spots[index][0] = x;
                available_mine_spots[index][1] = y;
                available_mine_spots[index][2] = x;
                available_mine_spots[index++][3] = y + 1;
            }
        }
    }

    // Shuffle array
    for (i = index - 1; i > 0; i--) {
        j = rand() % (i + 1);
        // Swap
        swap(&available_mine_spots[i][0], &available_mine_spots[j][0]);
        swap(&available_mine_spots[i][1], &available_mine_spots[j][1]);
        swap(&available_mine_spots[i][2], &available_mine_spots[j][2]);
        swap(&available_mine_spots[i][3], &available_mine_spots[j][3]);
    }

    // Put in mines
    for (i = 0; i < NUMBER_OF_MINES; i++) {
        mine_locations[i][0] = available_mine_spots[i][0];
        mine_locations[i][1] = available_mine_spots[i][1];
        mine_locations[i][2] = available_mine_spots[i][2];
        mine_locations[i][3] = available_mine_spots[i][3];
    }
}


/**
 * Mine at connection
 */

int mineAtConnection(int x1, int y1, int x2, int y2) {
    int i;

    if (checkConnection(x1, y1, x2, y2)) {
        for (i = 0; i < NUMBER_OF_MINES; i++) {
            if (
                    (mine_locations[i][0] == x1 &&
                    mine_locations[i][1] == y1 &&
                    mine_locations[i][2] == x2 &&
                    mine_locations[i][3] == y2) ||
                    (mine_locations[i][0] == x2 &&
                    mine_locations[i][1] == y2 &&
                    mine_locations[i][2] == x1 &&
                    mine_locations[i][3] == y1)
                    ) {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * Discovers mines at a given point.
 */

int discoverMines(int x, int y) {
    int return_value = 0;

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
 * Reveal all mines.
 */

void revealMines() {
    int i;
    
    for (i = 0; i < NUMBER_OF_MINES; i++) {
        removeConnection(mine_locations[i][0], mine_locations[i][1], mine_locations[i][2], mine_locations[i][3]);
    }
}

int main() {
    // Initializations
    Position start, destination;
    int initial_facing_direction;
    Path_element *path;
    int path_weight = 0, step_weight, shortest_path_weight;
    int x, y;
    Path_element *robot_path, *shortest_path;

    // Seed the rand() function
    srand(time(NULL));

    // Randomize facing direction
    initial_facing_direction = rand() % 4 + 1;

    // Initialize grid
    generateGrid();

    // Create mines
    createMines();

    // Randomize the start position
    start.x = destination.x = rand() % 5;
    start.y = destination.y = rand() % 5;

    // Get a destination which is not equal to the start position
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

    if (!path) {
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

    // Move while not on destination
    while (1) {
        // Move and save position and step weight
        path = path->next;
        addToEndOfPath(&robot_path, path->x, path->y);
        x = path->x;
        y = path->y;
        step_weight = path->step_weight;
        
        // Check if at destination
        if (x == destination.x && y == destination.y) {
            path_weight += step_weight;
            break;
        }

        // Check for mines
        if (discoverMines(x, y)) {
            path = findShortestPath(x, y, path->facing_direction, destination.x, destination.y);

            if (!path) {
                cls();
                printGrid(robot_path);
                printf("\nCouldn't find a path...\n");
                sleep(2);
                main();
                return 0;
            }
        }

        // Display path and add step weight to path weight
        cls();
        printGrid(robot_path);
        path_weight += step_weight;
        printf("\nCurrent step weight: %03d\nTotal path weight: %03d\n", step_weight, path_weight);
        sleep(1);
    }
    
    // Reveal mines
    revealMines();
    
    // Calculate shortest path
    shortest_path = findShortestPath(start.x, start.y, initial_facing_direction, destination.x, destination.y);
    
    shortest_path_weight = 0;
    
    while (shortest_path) {
        shortest_path_weight += shortest_path->step_weight;
        shortest_path = shortest_path->next;
    }
    

    // Display final screen
    cls();
    printGrid(robot_path);
    printf("\nCurrent step weight: %03d\nFinal path weight: %03d\nShortest path weight: %03d\nEfficiency: %02d%%\n", step_weight, path_weight, shortest_path_weight, 100*shortest_path_weight/path_weight);
    sleep(3);
    main();
    return EXIT_SUCCESS;
}

