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
#define MAX_NBS 10

typedef struct position {
    int x, y;
} position;

typedef struct li {
    char name;
    struct position **parents;
    struct li **nbs;
	struct position *pos;
	int id;
	int isolated;
} li;

typedef struct si {
    struct si *next;
    char *data;
} si;

void initializeNodes(void);
int setNbs(int, int);
void bruteForcePaths(int);
int getNumberOfNbs(int);
void isolateNode(int);

// Reduce complexity algorithm
li** getVictims(int);
int reduceComplexity();

void init(void);

void stageTwo(void);

#endif	/* TSP_H */

