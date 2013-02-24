#include <stdio.h>
#include <stdlib.h>

#include "path.h"

/**
 * Adds a path element to the end of the given linked list.
 * @param path <Path_element**> path to add to
 * @param x <int> x position
 * @param y <int> y position
 */
void addToEndOfPath(Path_element** path, int x, int y) {
    // Initiate element to add
    Path_element* element = malloc(sizeof(Path_element));
    Path_element* path_copy = malloc(sizeof(Path_element));
    element->x = x;
    element->y = y;
    
    // If linked list is NULL, just assign the element
    if (!*path) { *path = element; return; }
    
    // Loop through a copy of the path, not modifying the original path giving to the function, as this would result in a one-time use
    path_copy = *path;
    while (path_copy->next) path_copy = path_copy->next;
    
    // Add element
    element->previous = path_copy;
    path_copy->next = element;
}

/**
 * Adds a Path_element type pointer to the begin of the given linked list.
 * @param path <Path_element**> path to add to
 * @param x <int> x position
 * @param y <int> y position
 * @param facing_direction <int> the direction in which the robot is facing
 * @param step_weight <int> the weight of the step to make to reach the specified node
 */
void addToBeginOfPath(Path_element** path, int x, int y, int facing_direction, int step_weight) {
    // Initiate element to add
    Path_element* element = malloc(sizeof(Path_element));
    element->x = x;
    element->y = y;
    element->facing_direction = facing_direction;
    element->step_weight = step_weight;
    
    // Add element
    element->next = *path;
    element->previous = NULL;
    if (element->next) element->next->previous = element;
    *path = element;
}
