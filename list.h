/* 
 * File:   list.h
 * Author: Wessel Bruinsma
 *
 * Created on February 13, 2013, 2:42 PM
 */

#ifndef LIST_H
#define	LIST_H

struct List_element {
    struct Node *node;
    struct List_element *next, *previous;
};
typedef struct List_element List_element;

void addToList(List_element**, Node*);
void removeFromList(List_element**, Node*);
Node* findLowestFInList(List_element*);
int inList(List_element*, Node*);
void clearList(List_element**);

#endif	/* LIST_H */

