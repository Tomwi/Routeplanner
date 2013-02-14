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

extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];

/**
 * Clears screen.
 */
void cls() {
    printf("\033[2J\033[0;0f");
    printf("\033[%d;%df", 0, 0);
}

int main() {
    // Initializations
    Position start, destination;
    int initial_facing_direction;
    Path_element *path;
    int path_weight = 0, step_weight;
    int x, y;
    Path_element *robot_path;
    
    // Seed the rand() function
    srand(time(NULL));
    
    // Randomize facing direction
    initial_facing_direction = rand() % 4 + 1;    
    
    // Initialize grid
    generateGrid();
    removeConnection(1, 1, 1, 2);
    removeConnection(2, 3, 2, 2);
    removeConnection(1, 2, 1, 1);
    removeConnection(3, 3, 3, 4);
    removeConnection(2, 2, 3, 2);
    removeConnection(3, 2, 3, 3);
    removeConnection(1, 3, 0, 3);
    removeConnection(3, 4, 2, 4);
    removeConnection(4, 1, 4, 2);
    removeConnection(1, 0, 1, 1);
    removeConnection(2, 1, 3, 1);
    
    // Randomize the start position
    start.x = destination.x = rand() % 5;
    start.y = destination.y = rand() % 5;

    // Get a destination which is not equal to the start position
    while (destination.x == start.x && destination.y == start.y) {
        destination.x = rand() % 5;
        destination.y = rand() % 5;
    }

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

    // Record start
    x = start.x;
    y = start.y;

    // Display first move
    cls();
    printGrid(robot_path);
    sleep(1);

    // Move while not on destination
    while (x != destination.x || y != destination.y) {
        // Move and save position and step weight
        path = path->next;
        addToEndOfPath(&robot_path, path->x, path->y);
        x = path->x;
        y = path->y;
        step_weight = path->step_weight;

        // Display path and add step weight to path weight
        cls();
        printGrid(robot_path);
        path_weight += step_weight;
        printf("\nCurrent step weight: %03d, total path weight: %03d\n", step_weight, path_weight);
        sleep(1);
    }

    // Display final screen
    cls();
    printGrid(robot_path);
    printf("\nFinal path weight: %03d\n", path_weight);
    sleep(2);
    
    // Rerun program
    main();
    return EXIT_SUCCESS;
}

