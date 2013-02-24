/* 
 * File:   list.h
 * Author: Wessel Bruinsma
 */

#ifndef LIST_H
#define	LIST_H

// Defining the structs used
struct List_element {
    struct Node *node;
    struct List_element *next, *previous;
};
typedef struct List_element List_element;

// Defining the functions used
void addToList(List_element**, Node*);
void removeFromList(List_element**, Node*);
Node* findLowestFInList(List_element*);
int inList(List_element*, Node*);
void clearList(List_element**);

#endif	/* LIST_H */

