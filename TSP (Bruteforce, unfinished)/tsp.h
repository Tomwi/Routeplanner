/* 
 * File:   tsp.h
 * Author: Wessel Bruinsma
 *
 * Created on May 13, 2013, 5:59 PM
 */

#ifndef TSP_H
#define	TSP_H

#define NUMBER_OF_NODES 40
#define MAX_PATH_LENGTH 60

typedef struct pos {
    int x, y;
} pos;

typedef struct li {
    char name;
    struct pos **parents;
    struct li **nbs;
} li;

typedef struct si {
    struct si *next;
    char *data;
} si;

void initializeNodes(void);
int setNbs(int, int);
void bruteForcePaths(int);

#endif	/* TSP_H */

