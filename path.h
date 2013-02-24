/* 
 * File:   path.h
 * Author: Wessel Bruinsma
 */

#ifndef PATH_H
#define	PATH_H

// Defining the structs used
struct Path_element {
    int x, y, facing_direction, step_weight;
    struct Path_element *next, *previous;
};
typedef struct Path_element Path_element;

// Defining the functions used
void addToEndOfPath(Path_element**, int, int);
void addToBeginOfPath(Path_element**, int, int, int, int);

#endif	/* PATH_H */

