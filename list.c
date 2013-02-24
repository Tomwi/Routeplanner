#include <stdio.h>
#include <stdlib.h>

#include "astar.h"
#include "list.h"

/**
 * Adds element to the begin of the list.
 * @param list <List_element**> list to add to
 * @param node <Node*> node to add
 */
void addToList(List_element **list, Node *node) {
    // Initiate element to add
    List_element *element = malloc(sizeof (List_element));
    element->node = node;

    // Add element
    element->next = *list;
    element->previous = NULL;
    if (element->next) element->next->previous = element;
    *list = element;
}

/**
 * Finds the element which contains the given node and remove it.
 * @param list <List_element**> list to remove from
 * @param node <Node*> node to add
 */

void removeFromList(List_element **list, Node *node) {
    // Initiate necessary pointers
    List_element *p = malloc(sizeof (List_element));
    List_element *list_copy = malloc(sizeof (List_element));

    // If the given node is the first one in the list
    if ((*list)->node == node) {
        // Save the pointer
        p = *list;

        // If a next element exists, correct the linking
        if ((*list)->next) {
            (*list)->next->previous = NULL;
            *list = (*list)->next;
        } else *list = NULL;

        // Free up memory
        free(p);
        return;
    }

    // The given node is not the first element, copy the list for easier coding and not modifying the original list given to the function
    list_copy = (*list)->next;

    // While list_copy is a valid element
    while (list_copy) {
        // Check if the node is equivalent to the given one
        if (list_copy->node == node) {
            // Save the pointer
            p = list_copy;

            // Check if the element is last in the list, act accordingly
            if (!list_copy->next)
                list_copy->previous->next = NULL;
            else {
                list_copy->previous->next = list_copy->next;
                list_copy->next->previous = list_copy->previous;
            }

            // Free up memory
            free(p);
            return;
        }

        // Get next element
        list_copy = list_copy->next;
    }
}

/**
 * Find the element in the list which contains the node with the lowest sum of lowest path weight score and the heurisic.
 * @param list <List_element*> list to find in
 * @return <Node*> node with lowest score
 */

Node* findLowestFInList(List_element *list) {
    // Create element for the node to save in
    List_element *element = malloc(sizeof (List_element));
    element = list;

    // Loop through list
    while (list) {
        // Check if the node element's score is better and act consequently
        if (element->node->H + element->node->G > list->node->H + list->node->G) element = list;
        list = list->next;
    }

    // Return node
    return element->node;
}

/**
 * Checks if the given list contains the given node.
 * @param list <List_element*> list to search in
 * @param node <Node*> node to search for
 * @return <int> boolean wether the node has been found
 */

int inList(List_element *list, Node *node) {
    // Loop through list
    while (list) {
        // Check if node matches
        if (list->node == node) return 1;
        list = list->next;
    }

    // Nothing returned before, so nothing found, so return 0
    return 0;
}

/**
 * Clears list.
 * Clears list by removing all elements.
 * @param <List_element**> list to clear
 */

void clearList(List_element **list) {
    // Loop through list and remove items
    while (*list) removeFromList(list, (*list)->node);
}
