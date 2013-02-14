#include <stdio.h>
#include <stdlib.h>

#include "astar.h"
#include "list.h"
#include "path.h"

Node *grid[GRID_SIZE_X][GRID_SIZE_Y];
List_element *list_closed = NULL, *list_open = NULL;

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
 * Processes a node in the a* algorithm.
 * @param node <Node*> node to process
 * @param parent <Node*> parent of the node
 * @param step_weight <int> weight of the step to make
 * @param destination <Position> destination
 */

void processNeighbour(Node *node, Node *parent, int step_weight, Position destination) {
    // Check if node if valid or is in the closed list
    if (node && !inList(list_closed, node)) {
        // Check wether the node has been added to the open list
        if (!inList(list_open, node)) {
            // Add to open list
            addToList(&list_open, node);

            // Change parent
            node->parent = parent;
            
            // Set H score to Manhattan distance from destination
            node->H = abs(node->position.x - destination.x) + abs(node->position.y - destination.y);
            
            // Set path weight
            node->G = parent->G + step_weight;
            
            // Set step weight
            node->step_weight = step_weight;
        } else {
            // Check if node's score is better
            if (parent->G + step_weight < node->G) {
                // Update node details
                node->G = parent->G + step_weight;
                node->parent = parent;
                node->step_weight = step_weight;
            }
        }
    }
}

/**
 * Reverses the given direction.
 * @param direction <int> direction to reverse
 * @return <int> reversed direction
 */

int reverseDirection(int direction) {
    // Reverse direction
    switch (direction) {
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        case EAST: return WEST;
        case WEST: return EAST;
    }
}

/**
 * Gets in which direction the parent of the node is located.
 * @param node <Node*> node
 * @return <int> parent's direction
 */

int getParentLocation(Node *node) {
    // Test if the parent exists
    if (node->parent) {
        
        // Find location by comparing neighbours
        if (node->east == node->parent) {
            return EAST;
        } else if (node->parent == node->west) {
            return WEST;
        } else if (node->parent == node->south) {
            return SOUTH;
        } else if (node->parent == node->north) {
            return NORTH;
        }
    }

    return 0;
}

/**
 * Determines the step weight based on the location of the parent and destination.
 * @param destination_location <int> destination's location
 * @param parent_location <int> parent's location
 * @return <int> step weight
 */

int determineStepWeight(int destination_location, int parent_location) {
    // Determine the step weight by checking if the directions are perpendicular or reversed.
    switch (destination_location) {
        case SOUTH:
            if (parent_location == EAST || parent_location == WEST) return STEP_WEIGHT + TURNING_WEIGHT;
            else if (parent_location == SOUTH) return STEP_WEIGHT + 2 * TURNING_WEIGHT;
            else return STEP_WEIGHT;
            break;
        case EAST:
            if (parent_location == NORTH || parent_location == SOUTH) return STEP_WEIGHT + TURNING_WEIGHT;
            else if (parent_location == EAST) return STEP_WEIGHT + 2 * TURNING_WEIGHT;
            else return STEP_WEIGHT;
            break;
        case NORTH:
            if (parent_location == EAST || parent_location == WEST) return STEP_WEIGHT + TURNING_WEIGHT;
            else if (parent_location == NORTH) return STEP_WEIGHT + 2 * TURNING_WEIGHT;
            else return STEP_WEIGHT;
            break;
        case WEST:
            if (parent_location == NORTH || parent_location == SOUTH) return STEP_WEIGHT + TURNING_WEIGHT;
            else if (parent_location == SOUTH) return STEP_WEIGHT + 2 * TURNING_WEIGHT;
            else return STEP_WEIGHT;
            break;
    }
}

/**
 * Finds the shortest path between two nodes.
 * This method uses the a* algorithm.
 * @param start_x <int> start x position
 * @param start_y <int> start y position
 * @param facing_direction <int> initial facing direction
 * @param destination_x <int> destination x
 * @param destination_y <int> destination y
 * @return <Path_element*> shortest path
 */

Path_element* findShortestPath(int start_x, int start_y, int facing_direction, int destination_x, int destination_y) {
    // Clears the grid cache to prevent incorrect results
    clearGridCache();

    // Initiate the node element, the start and destination and the path
    Node *node = malloc(sizeof (Node)), *start = malloc(sizeof (Node)), *destination = malloc(sizeof (Node));
    Path_element *path = NULL;

    // Link start and destination to the grid consequently
    start = grid[start_x][start_y];
    destination = grid[destination_x][destination_y];

    // Add start to open list
    addToList(&list_open, start);

    // Find initial 'parent location'
    int parent_location = reverseDirection(facing_direction);

    do {
        // Find the node with the best score in the open list
        node = findLowestFInList(list_open);
        
        // Remove the node from the open list
        removeFromList(&list_open, node);
        
        // Add the node to the closed list
        addToList(&list_closed, node);

        // Find the parent location
        if (node->parent) parent_location = getParentLocation(node);

        // Process the neighbours
        processNeighbour(node->south, node, determineStepWeight(SOUTH, parent_location), destination->position);
        processNeighbour(node->north, node, determineStepWeight(NORTH, parent_location), destination->position);
        processNeighbour(node->east, node, determineStepWeight(EAST, parent_location), destination->position);
        processNeighbour(node->west, node, determineStepWeight(WEST, parent_location), destination->position);
        
        // Do this while the destination is not in the closed list, in which case the path has been found, or the open list is empty
    } while (!inList(list_closed, destination) && list_open != NULL);

    // Check if best path is found, in which case the destination is in the closed list
    if (inList(list_closed, destination)) {
        // Add destination to path
        node = destination;
        addToBeginOfPath(&path, node->position.x, node->position.y, reverseDirection(getParentLocation(node)), node->step_weight);

        do {
            // Get the parent of the node, doing this enough times will eventually get you to the start
            node = node->parent;

            // Add node to path
            addToBeginOfPath(
                    &path,
                    node->position.x, node->position.y,
                    !getParentLocation(node) ? facing_direction : reverseDirection(getParentLocation(node)),
                    node->step_weight
                    );
        } while (node != start);
    }

    // Clear the lists
    clearList(&list_closed);
    clearList(&list_open);

    // Return the path
    return path;
}

/**
 * Checks if there is a connection between two nodes.
 * @param x1 <int> node one x
 * @param y1 <int> node one y
 * @param x2 <int> node two x
 * @param y2 <int> node two y
 * @return <int> boolean wether there is a connection
 */

int checkConnection(int x1, int y1, int x2, int y2) {
    // Check if both nodes exist
    if (grid[x1][y1] && grid[x2][y2]) {
        // Arrange the coordinate pairs for easier checking, otherwise swap coordinates
        if (y1 == y2 && x2 - x1 == -1) {
            swap(&x1, &x2);
        } else if (x1 == x2 && y2 - y1 == 1) {
            swap(&y1, &y2);
        }

        // Check if the nodes are neighbours, in which case there is a connection
        if (
                x2 - x1 == 1 &&
                grid[x2][y2]->west == grid[x1][y1] &&
                grid[x1][y1]->east == grid[x2][y2]
                ) {
            return 1;
        } else if (
                y2 - y1 == -1 &&
                grid[x2][y2]->north == grid[x1][y1] &&
                grid[x1][y1]->south == grid[x2][y2]
                ) {
            return 1;
        }
    }

    // No return value already return, so no connection found
    return 0;
}

/**
 * Removes a connection between two nodes.
 * @param x1 <int> node one x
 * @param y1 <int> node one y
 * @param x2 <int> node two x
 * @param y2 <int> node two y
 */

void removeConnection(int x1, int y1, int x2, int y2) {
    // Check if both nodes are valid and are connected
    if (grid[x1][y1] && grid[x2][y2] && checkConnection(x1, y1, x2, y2)) {
        // Arrange the coordinate pairs for easier checking, otherwise swap coordinates
        if (y1 == y2 && x2 - x1 == -1) {
            swap(&x1, &x2);
        } else if (x1 == x2 && y2 - y1 == 1) {
            swap(&y1, &y2);
        }

        // Remove the connection by removing the pointer to eachother as neighbours
        if (x2 - x1 == 1) {
            grid[x2][y2]->west = NULL;
            grid[x1][y1]->east = NULL;
        } else if (y2 - y1 == -1) {
            grid[x2][y2]->north = NULL;
            grid[x1][y1]->south = NULL;
        }
    }
}

/**
 * Cleares the cache of the grid.
 * The a* algorithm will store certain information on the nodes. This information needs to be cleared for another run of a*.
 */

void clearGridCache() {
    int x, y;

    // Loop through grid
    for (x = 0; x < GRID_SIZE_X; x++) {
        for (y = 0; y < GRID_SIZE_Y; y++) {
            // Check if node is valid
            if (grid[x][y]) {
                // Clear cache
                Node *node = grid[x][y];
                node->H = 0;
                node->G = 0;
                node->step_weight = 0;
                node->parent = NULL;
            }
        }
    }
}

/**
 * Generates the grid and initiates the nodes.
 */

void generateGrid() {
    int x, y;

    // Loop through grid
    for (x = 0; x < GRID_SIZE_X; x++) {
        for (y = 0; y < GRID_SIZE_Y; y++) {
            // Initialize node
            grid[x][y] = malloc(sizeof (Node));

            // Assign NULL values to neighbours for now to prevent NULL pointers
            Node *node = grid[x][y];
            node->east = NULL;
            node->west = NULL;
            node->south = NULL;
            node->north = NULL;
        }
    }

    // Loop through grid again
    for (x = 0; x < GRID_SIZE_X; x++) {
        for (y = 0; y < GRID_SIZE_Y; y++) {
            // Now assign the position and reset the values
            Node *node = grid[x][y];
            node->position.x = x;
            node->position.y = y;
            node->H = 0;
            node->G = 0;
            node->mark = 0;
            node->step_weight = 0;
            node->parent = NULL;

            // Check if there is a neighbour in each direction, if so, add it as neighbour
            if (grid[x + 1][y] && x + 1 < GRID_SIZE_X) node->east = grid[x + 1][y];
            if (grid[x - 1][y] && x > 0) node->west = grid[x - 1][y];
            if (grid[x][y - 1] && y > 0) node->south = grid[x][y - 1];
            if (grid[x][y + 1] && y + 1 < GRID_SIZE_Y) node->north = grid[x][y + 1];
        }
    }
}

/**
 * Removes a node from the grid.
 * @param x <int> node x
 * @param y <int> node y
 */

void removeFromGrid(int x, int y) {
    // Check if there are neighbours, if so, remove the connection
    if (grid[x][y]->north) grid[x][y]->north->south = NULL;
    if (grid[x][y]->south) grid[x][y]->south->north = NULL;
    if (grid[x][y]->west) grid[x][y]->west->east = NULL;
    if (grid[x][y]->east) grid[x][y]->east->west = NULL;

    // Free and reset the grid element
    free(grid[x][y]);
    grid[x][y] = NULL;
}

/**
 * Prints the grid.
 * @param robot_path <Path_element*> path of the robot
 */

void printGrid(Path_element* robot_path) {
    // Check if the path isn't NULL
    if (robot_path) {
        // Mark the start
        grid[robot_path->x][robot_path->y]->mark = 'S';

        // Loop through path
        if (robot_path->next) {
            do {
                // Mark path
                robot_path = robot_path->next;
                grid[robot_path->x][robot_path->y]->mark = 'X';
            } while (robot_path->next);
        }

        // Mark destination
        grid[robot_path->x][robot_path->y]->mark = 'R';
    }

    int x, y;

    // Loop through rows
    for (y = GRID_SIZE_Y - 1; y >= 0; y--) {
        // Display grid y
        printf("%5-d", y);

        // Loop through columns
        for (x = 0; x < GRID_SIZE_X; x++) {
            // Display node
            if (grid[x][y] && grid[x][y]->mark) {
                printf(" %c ", grid[x][y]->mark);
            } else if (grid[x][y]) {
                printf(" . ");
            } else {
                printf("   ");
            }
            
            // Display connection between horizontal nodes
            if (checkConnection(x, y, x + 1, y)) {
                printf(" - ");
            } else {
                printf("   ");
            }
        }

        // Display newline
        printf("\n     ");

        // Loop through columns again
        for (x = 0; x < GRID_SIZE_X; x++) {
            // Display vertical connection
            if (checkConnection(x, y, x, y - 1)) {
                printf(" | ");
            } else {
                printf("   ");
            }

            printf("   ");
        }
        
        // Display newline
        printf("\n");
    }
    
    printf("      ");
    
    // Display grid x
    for (x = 0; x < GRID_SIZE_X; x++) {
        printf("%6-d", x);
    }
    
    printf("\n");
}