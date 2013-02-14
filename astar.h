/* 
 * File:   astar.h
 * Author: Wessel Bruinsma
 *
 * Created on February 13, 2013, 2:36 PM
 */

#ifndef ASTAR_H
#define	ASTAR_H

#define abs(x) ((x)<0?-(x):(x))

#define GRID_SIZE_X 5
#define GRID_SIZE_Y 5

#define TURNING_WEIGHT 2 // Penalty for turning 90 degrees
#define STEP_WEIGHT 10

#define NORTH 1
#define SOUTH 3
#define WEST 4
#define EAST 2

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
    int G, H, step_weight;
    struct Position position;
    char mark;
};

typedef struct Node Node;
typedef struct Position Position;

void generateGrid(void);
void printGrid(struct Path_element*);
void processNeighbour(Node*, Node*, int, Position);
struct Path_element* findShortestPath(int, int, int, int, int);
void removeFromGrid(int, int);
int checkConnection(int, int, int, int);
void removeConnection(int, int, int, int);
void clearGridCache(void);

int checkConnectionForMine(int, int, int, int);

#endif	/* ASTAR_H */

