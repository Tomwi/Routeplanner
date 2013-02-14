/* 
 * File:   path.h
 * Author: Wessel Bruinsma
 *
 * Created on February 13, 2013, 5:59 PM
 */

#ifndef PATH_H
#define	PATH_H

struct Path_element {
    int x, y, facing_direction, step_weight;
    struct Path_element *next, *previous;
};
typedef struct Path_element Path_element;

void addToEndOfPath(Path_element**, int, int);
void addToBeginOfPath(Path_element**, int, int, int, int);

#endif	/* PATH_H */

