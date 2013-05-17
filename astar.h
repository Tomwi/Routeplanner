/* 
 * File:   astar.h
 * Author: Wessel Bruinsma
 */

#ifndef ASTAR_H
#define	ASTAR_H

// Macro for the absolute value
#define abs(x) ((x)<0?-(x):(x))

// Define the grid size
#define GRID_SIZE_X 5
#define GRID_SIZE_Y 5

// Penalty for turning 90 degrees
#define TURNING_WEIGHT 2

// Penalty for taking a step in the grid (ie: driving from one node to another)
#define STEP_WEIGHT 10

// Is the robot able to drive backwards?
#define BACKWARDS_DRIVING 1

// Do you want mines?
#define MINES_ACTIVE 1

// How many mines are placed in the field
#define NUMBER_OF_MINES 13

// Some defines
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

// Number of places to visit
#define NUMBER_OF_PLACES_TO_VISIT 3

// Debug mode
#define DEBUG 0

// Macro for calculating a reversed direction, a function would be not nessecary
#define reverseDirection(direction) (direction==NORTH||direction==SOUTH?(direction==NORTH?SOUTH:NORTH):(direction==WEST?EAST:WEST))

// Macro for determining the step weight, taking in account the direction the robot is facing and applying the turning penalty accordingly
#define determineStepWeight(direction,parent_location) \
(STEP_WEIGHT +  \
        (reverseDirection(direction) == parent_location ? 0 : \
                (direction == parent_location ? 2 * TURNING_WEIGHT * !BACKWARDS_DRIVING : TURNING_WEIGHT) \
        ))

// Defining the structs used
struct Path_element;

struct Position {
    unsigned int x, y;
};

struct Node {
    struct Node *north,
            *east,
            *south,
            *west;
    struct Node *parent;
    int G, H, step_weight, id, odd;
    struct Position position, positionGrid;
    char mark;
};

typedef struct Node Node;
typedef struct Position Position;

// Defining the functions used
void swap(int*, int*);
void generateGrid(void);
void printGrid(struct Path_element*);
void processNeighbour(Node*, Node*, int, Position);
struct Path_element* findShortestPath(int, int, int, int, int);
struct Path* getPathDetails(int, int, int, int, int);
void removeFromGrid(int, int);
int checkConnection(int, int, int, int);
void removeConnection(int, int, int, int);
void clearGridCache(void);
void cls(void);

#endif	/* ASTAR_H */

