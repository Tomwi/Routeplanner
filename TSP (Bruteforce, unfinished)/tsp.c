/* 
 * File:   tsp.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>
#include "tsp.h"

li **nodes;

int setNbs(int i1, int i2) {
	int j1 = 0, j2 = 0, i;

	while (nodes[i1]->nbs[j1] != NULL) j1++;
	while (nodes[i2]->nbs[j2] != NULL) j2++;

	if (j1 > 3 || j2 > 3) return 0;

	int exists1 = 0, exists2 = 0;

	if (j1 != 0) {
		for (i = 0; i < j1; i++) {
			if (nodes[i1]->nbs[i] == nodes[i2]) exists1 = 1;
		}
	}

	if (j2 != 0) {
		for (i = 0; i < j2; i++) {
			if (nodes[i2]->nbs[i] == nodes[i1]) exists2 = 1;
		}
	}

	if (!exists1) nodes[i1]->nbs[j1] = nodes[i2];
	if (!exists2) nodes[i2]->nbs[j2] = nodes[i1];

	return 1;
}

/**
 * Initialize the nodes by transforming the grid.
 */
void initializeNodes() {
	int i = 0, j;

	nodes = malloc(sizeof (li) * NUMBER_OF_NODES);

	// Initialize all nodes
	for (i = 0; i < NUMBER_OF_NODES; i++) {
		*(nodes + i) = malloc(sizeof (li));

		nodes[i]->name = 42 + i;

		nodes[i]->nbs = malloc(sizeof (li)*4);

		nodes[i]->parents = malloc(sizeof (pos)*2);
		nodes[i]->parents[0] = malloc(sizeof (pos));
		nodes[i]->parents[1] = malloc(sizeof (pos));
	}

	// Set correct parents (!!)

	// Upper horizontal connections
	for (j = 0; j < 3; j++) {
		setNbs(j, j + 1);
	}

	// Upper middle cross connections
	for (j = 6; j <= 10; j += 2) {
		setNbs(j, (j - 6) / 2);
		setNbs(j, (j - 6) / 2 + 1);
	}

	// Lower middle cross connections
	for (i = 15; i <= 33; i += 9) {
		for (j = 0; j <= 4; j += 2) {
			setNbs(j + i, j + i - 10);
			setNbs(j + i, j + i - 9);
		}
	}

	// Lower horizontal connections
	for (i = 4; i <= 31; i += 9) {
		for (j = 0; j <= 7; j++) {
			setNbs(i + j, i + j + 1);
		}
	}

	// Left side
	for (i = 13; i <= 31; i += 9) {
		setNbs(i, i - 9);
		setNbs(i, i - 8);
	}

	// Right side
	for (i = 21; i <= 39; i += 9) {
		setNbs(i, i - 10);
		setNbs(i, i - 9);
	}

	// Tilted sides
	setNbs(0, 4);
	setNbs(3, 12);

	bruteForcePaths(1);
}

void addStack(int name, si *stack, si *stackhead, int *stacksize) {
	if (stack == NULL) { // Stack is empty
		stack = malloc(sizeof (si));
		stackhead = stack;
	} else {
		stack->next = malloc(sizeof (si));
		stack = stack->next;
	}

	char *data = malloc(sizeof (char) *MAX_PATH_LENGTH);
	*data = name;
	stack->data = data;
	stack->next = NULL;
	*stacksize++;
}

li** createNbsMap() {
	li **map = malloc(sizeof (li) * NUMBER_OF_NODES);
	int i;

	for (i = 0; i < NUMBER_OF_NODES; i++) {
		map[nodes[i]->name - 42] = nodes[i];
	}

	return map;
}

int chrInStr(char c, char * str, int len) {
	while (*str != '\0' && len) {
		if (*(str++) == c) return 1;
		len--;
	}

	return 0;
}

/**
 * No character in the path can occur more than two times (because that will result in a deadlock)
 * @param path
 * @param pathlen
 * @return 
 */
int validatePath(char * path, int pathlen) {
	char * temp;

	// Get all the different character in the string
	char * dataset = malloc(sizeof (char) *MAX_PATH_LENGTH);
	int datasetlen = 0;

	temp = path;
	while (*temp != '\0' && temp) {
		if (!chrInStr(*temp, dataset-datasetlen, datasetlen)) {
			*(dataset++) = *temp;
			datasetlen++;
		}
		temp++;
	}

	printf("%s\n", path);
	
	*dataset = '\0';
	dataset = dataset - datasetlen;
	// All the different characters are now stored in dataset, now check if a character in that set occurs more than two times in the path
	while (*dataset != '\0') {
		int occurrence = 0, len = pathlen;
		
		temp = path;
		while (*temp != '\0' && len-- > 0) {
			if (*temp == *dataset) {
				occurrence++;
			}
			
			temp++;
		}
		
		dataset++;
		if (occurrence > 2) return 0;
	}
	
	return 1;
}

int validateConnection(li* p1, li* p2) {
	int i;

	for (i = 0; i < 4; i++) {
		if (
				(p1->nbs[i] != NULL && p1->nbs[i] == p2) ||
				(p2->nbs[i] != NULL && p2->nbs[i] == p1)
				)
			return 1;
	}

	return 0;
}

void bruteForcePaths(int start) {
	int firstrun = 1;
	si *stack, *stackhead;
	si *queue, *queuehead;
	si *temp, *p;
	li **nbsMap = createNbsMap();

	int a = validatePath("abcdaefgab", 10);

	printf("--> %d <-- \n", a);

	while (queue != queuehead || !firstrun) {
		firstrun = 0;
		temp = queuehead;

		/*
		 * The stack contains a number of paths.
		 * Each node contains a certain amount of neighbours. The 'set' which contains a certain node, is the set of neighbours of the previous node in the path. Now start at a node, and iterate the following for each path in the stack:
		 * - If the length of the path is not the maximum length, add a node to the path which must be the first in the data set. Add this new path to the queue.
		 * - If the last node in the path is not the final node in his set, advance this node to the next one. Add this new path to the queue.
		 * After that:
		 * - Test each path in the stack.
		 * - Stack = queue.
		 * - Reinitialize the queue.
		 * Repeat until the queue is empty (which will also be on the first iteration.
         */
		while (temp) {
			if (!validatePath(temp->data, 9)) {
				temp = temp->next;
				continue;
			}

			// Get available options for current (add) and previous (change)
			// First get the size, then create the set
			int i;
			
			while (nbsMap[])

			// Get last and change if length is still acceptable

			// Add if length is still acceptable

			temp = temp->next;
		}

		// Check whole stack for acceptable length, then test length of paths and save optimal one

		// queue = stack, reinitialize stack 
	}
}
