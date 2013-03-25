/* 
 * File:   places.h
 * Author: Wessel Bruinsma
 *
 * Created on March 25, 2013, 1:52 PM
 */

#ifndef PLACES_H
#define	PLACES_H

#define NUMBER_OF_PLACES_TO_VISIT 3

struct Path {
    int length;
    int facing_direction;
};
typedef struct Path Path;

struct Total_path {
    int places[NUMBER_OF_PLACES_TO_VISIT];
    int length;
};
typedef struct Total_path Total_path;

void getPlacesToVisit(void);
int getTargetPlace(int, int, int, int);
void visit(int, int);
void unvisitAll(void);
void findBestTotalPath(int, int, int*, int*, Total_path**, int);
int visitedAllPlaces(void);

#endif	/* PLACES_H */

