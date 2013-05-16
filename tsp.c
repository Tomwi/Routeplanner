/* 
 * File:   tsp.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>
#include "tsp.h"
#include "openglfuncs.h"
#define GLFW_DLL
#include <GL/glfw.h>
#include "astar.h"

li **nodes;

int setNbs(int i1, int i2) {
	int j1 = 0, j2 = 0, i;

	while (nodes[i1]->nbs[j1] != NULL) j1++;
	while (nodes[i2]->nbs[j2] != NULL) j2++;

	if (j1 > MAX_NBS - 1 || j2 > MAX_NBS - 1) return 0;

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

void unNb(int n1, int n2) {
	li **nbs1 = malloc(sizeof (li) * MAX_NBS), **nbs2 = malloc(sizeof (li) * MAX_NBS);

	int i, size1 = 0, size2 = 0;

	// Filter the connection

	for (i = 0; nodes[n1]->nbs[i] != NULL; i++) {
		if (nodes[n1]->nbs[i] != nodes[n2]) {
			nbs1[size1] = malloc(sizeof (li));
			nbs1[size1++] = nodes[n1]->nbs[i];
		}
	}

	for (i = 0; nodes[n2]->nbs[i] != NULL; i++) {
		if (nodes[n2]->nbs[i] != nodes[n1]) {
			nbs2[size2] = malloc(sizeof (li));
			nbs2[size2++] = nodes[n2]->nbs[i];
		}
	}

	// Set the new nbs

	for (i = 0; i < MAX_NBS; i++) {
		if (i < size1) {
			nodes[n1]->nbs[i] = nbs1[i];
		} else {
			nodes[n1]->nbs[i] = NULL;
		}
	}

	for (i = 0; i < MAX_NBS; i++) {
		if (i < size2) {
			nodes[n2]->nbs[i] = nbs2[i];
		} else {
			nodes[n2]->nbs[i] = NULL;
		}
	}
}


/**
 * GRAPH CHECKER
 */
int *checkset;
int checkseti = 0;

int inCheckset(int x) {
	int i;

	for (i = 0; i < checkseti; i++) {
		if (checkset[i] == x) {
			return 1;
		}
	}

	return 0;
}

int addCheckset(int x) {
	if (!inCheckset(x))
		checkset[checkseti++] = x;
}

void initCheckset() {
	checkset = malloc(NUMBER_OF_NODES * sizeof (int));
}

void clearCheckset() {
	checkseti = 0;
}

int getNodeRange(int nd) {
	int curr = nd, x;
	int i;

	addCheckset(nd);

	// Add nbs to checkset
	for (i = 0; nodes[curr]->nbs[i] != NULL; i++) {
		x = nodes[curr]->nbs[i]->id;

		if (!inCheckset(x)) {
			getNodeRange(x);
			addCheckset(x);
		}

	}

	return checkseti;
}

int graphConnected() {
	int i, range = -1;

	for (i = 0; i < NUMBER_OF_NODES; i++) {
		if (range == -1) {
			range = getNodeRange(i);
			clearCheckset();
		} else if (range != getNodeRange(i)) {
			clearCheckset();
			return 0;
		}
	}

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
		nodes[i]->id = i;

		nodes[i]->nbs = malloc(sizeof (li) * MAX_NBS);

		nodes[i]->parents = malloc(sizeof (position)*2);
		nodes[i]->parents[0] = malloc(sizeof (position));
		nodes[i]->parents[1] = malloc(sizeof (position));

		nodes[i]->pos = malloc(sizeof (position));
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
			setNbs(j + i, j + i - 8);
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

	//unNb(6, 7);
	//	setNbs(1, 17);
	// Randomly isolate 13 nodes -> number of mines
	int nm = 1; //NUMBER_OF_MINES;
	int *isolated = malloc(sizeof (int) *nm);

	initCheckset();

	printf("Graph connected check: %d\n", graphConnected());
	clearCheckset();

	printf("Graph connected check: %d\n", graphConnected());
	clearCheckset();


	printf("Generating random isolated nodes...\n");

	for (i = 0; i < nm; i++) {
		int x, y;

		while (1) {
			//			sleep(1);
			x = rand() % NUMBER_OF_NODES;

			// if x not in isolated, good
			for (y = 0; isolated[y] != NULL; y++) {
				if (isolated[y] == x) continue;
			}


			// SO ISOLATE THAT NODE
			isolateNode(x);

			isolated[i] = x;

			break;
		}


	}

	printf("done.\n");

	/*
	 *	Create coordinates for the nodes for graphical view.
	 */

	int xspacing = 100, xmargin = 80;
	int ymargin = 30, yspacing = 80;

	for (i = 0; i < 4; i++) {
		nodes[i]->pos->x = xmargin + xspacing*i;
		nodes[i]->pos->y = ymargin;
	}

	xmargin = 50;
	xspacing = 50;
	int nodeindex = 4;

	for (j = 0; j < 4; j++) {
		for (i = 0; i < 9; i++) {
			nodes[nodeindex]->pos->x = xmargin + xspacing*i;
			nodes[nodeindex++]->pos->y = ymargin + yspacing * (j + 1);
		}
	}

	printf("Report of graphical setup:\n");
	for (i = 0; i < NUMBER_OF_NODES; i++) {
		printf("[%d] x: %d, y: %d \n", i, nodes[i]->pos->x, nodes[i]->pos->y);
	}





	openWindow();
}

///////////

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

void isolateNode(int nd) {
	int i, nbs = 0;

	printf("isolate %d: nbs: %d\n", nd, getNumberOfNbs(nd));

	int *unsets = malloc(sizeof (int) *(nbs = getNumberOfNbs(nd)));

	for (i = 0; nodes[nd]->nbs[i] != NULL; i++) {

		unsets[i] = nodes[nd]->nbs[i]->id;
	}

	for (i = 0; i < nbs; i++) {
		printf("unset %d and %d\n", i, nd);
		unNb(nd, unsets[i]);
	}
}

int getNumberOfNbs(int i) {
	int j;

	for (j = 0; nodes[i]->nbs[j] != NULL; j++);

	return j;
}

void execute() {
	int its = 15, i;

	for (i = 0; i < its; i++) reduceComplexity();

	printf("Reduce complexity algorithm with connectivity check iterations: %d\nFinal key points due to graph structure:\n", its);

	stageTwo();
}

/**
 * REDUCE COMPLEXITY ALGORITHM
 */

int reduceComplexity() {
	static int num = 0;
	printf("New iteration [%d]\n", ++num);
	int i, j, ii = 0, changes = 0;

	int *indices = malloc(sizeof (int) *NUMBER_OF_NODES);

	for (i = 0; i < NUMBER_OF_NODES; i++) {
		if (getNumberOfNbs(i) % 2 == 1) { // Voor nodes met oneven aantal
			indices[ii++] = i;
			printf("Node %d added (%d nbs(s))\n", i, getNumberOfNbs(i));
			changes = 1;
		}
	}

	for (i = 0; i < ii; i++) {
		printf("=> Node %d\n", indices[i]);

		li **victims = getVictims(indices[i]);

		for (j = 0; victims[j] != NULL; j++) {
			if (getNumberOfNbs(indices[i]) > 2) {
				printf(" - unconnect with %d", victims[j]->id);
				int rangeprev = getNodeRange(indices[i]);
				clearCheckset();

				unNb(victims[j]->id, indices[i]);

				// CHECK IF THIS UNCONNECT WOULD WRECK THE GRAPH

				if (rangeprev != getNodeRange(indices[i])) {
					setNbs(victims[j]->id, indices[i]);
					printf(" ---- WOULD UNCONNECT GRAPH - INTERCEPTED!\n");
				}
				clearCheckset();
			}
		}

		printf("\n");

		free(victims);
	}
	printf("End of iteration [%d] [return: %d]\n", num, !changes);

	return !changes;
}

li** getVictims(int nb) {
	li **nbs = malloc(sizeof (li) * MAX_NBS);

	// Filter the connection
	int i, index = 0, x, y;

	for (i = 0; nodes[nb]->nbs[i] != NULL; i++) {
		printf("-- Nb: %d\n", nodes[nb]->nbs[i]->id);
		if (getNumberOfNbs(nodes[nb]->nbs[i]->id) > 2) {
			// Save in a smart way -> no sorting needed afterwards
			printf("-- Yeah victim found!\n");

			if (index == 0) {
				nbs[index] = malloc(sizeof (li));
				nbs[index++] = nodes[nb]->nbs[i];
			} else {
				int change = 0;

				for (x = 0; x < index; x++) {

					printf("Compare: %d [%d] vs %d [%d] (node [nbs])\n", nbs[x]->id, getNumberOfNbs(nbs[x]->id), nodes[nb]->nbs[i]->id, getNumberOfNbs(nodes[nb]->nbs[i]->id));
					if (getNumberOfNbs(nbs[x]->id) < getNumberOfNbs(nodes[nb]->nbs[i]->id)) { // Save after current element (item value > elem value)
						for (y = index; y >= x; y--) { // shift all items right of it right
							nbs[y + 1] = nbs[y];
						}

						// Save element
						nbs[x] = nodes[nb]->nbs[i];

						index++;

						change = 1;

						// Break loop
						x = index;
					}
				}

				if (!change) {
					nbs[index] = malloc(sizeof (li));
					nbs[index++] = nodes[nb]->nbs[i];
				}
			}
		}
		printf("-- Theirs: %d\n", getNumberOfNbs(nodes[nb]->nbs[i]->name - 42));
	}

	for (i = 0; i < index; i++) {
		printf("Nb: %d with nbs: %d\n", nbs[i]->id, getNumberOfNbs(nbs[i]->id));
	}

	for (i = index; i < MAX_NBS; i++) {
		nbs[i] = NULL;
	}

	return nbs;
}

void stageTwo() {
	int i, numkeynodes = 0, numcritnodes = 0, numonenodes = 0;
	int *keynodes = malloc(sizeof (int) *NUMBER_OF_NODES);
	int *critnodes = malloc(sizeof (int) *NUMBER_OF_NODES);
	int *onenodes = malloc(sizeof (int) *NUMBER_OF_NODES); // onenode, parent, onenode, parent, .....

	for (i = 0; i < NUMBER_OF_NODES; i++) {
		if (getNumberOfNbs(i) % 2 == 1) { // Voor nodes met oneven aantal
			keynodes[numkeynodes++] = i;
		}
	}

	for (i = 0; i < numkeynodes; i++) {
		printf("==== Keynode: %d [%d]\n", keynodes[i], getNumberOfNbs(keynodes[i]));

		if (getNumberOfNbs(keynodes[i]) == 1) {
			printf("{one-way keynode, trying to deduce parent with simple deduction algorithm...}\n");

			int now = keynodes[i], prev;

			while (getNumberOfNbs(now) <= 2 || now == keynodes[i]) {
				printf("Curr node: %d\n", now);
				if (now == keynodes[i]) {
					prev = now;
					now = nodes[now]->nbs[0]->id;
				} else {
					if (nodes[now]->nbs[0]->id == prev) {
						prev = now;
						now = nodes[now]->nbs[1]->id;
					} else {
						prev = now;
						now = nodes[now]->nbs[0]->id;
					}
				}
				printf("Next node: %d [%d]\n", now, getNumberOfNbs(now));
			}

			printf("== Parent found: %d\n", now);

			onenodes[numonenodes++] = i;
			onenodes[numonenodes++] = now;
		} else {
			critnodes[numcritnodes++] = i;
		}
	}

	printf("---------------\n\n\nOverview (non reduced analysis):\n\nTotal complexity level: %d\nTotal keynodes: %d\nTotal standalone nodes: %d\n\nReduced complexity level: %d\n\n", (numcritnodes + 1), numkeynodes + 1, (numonenodes+1)/2, (numcritnodes+1)-((numonenodes+1)/2));
	
	printf("Press enter to in-depth analyse the graph...\n\n");
	
	/**
	 * Reduceer grid door:
	 * - Zoek nkele lussen
	 * - Zoek enkele takken (onenodes)
	 * [deze twee noem ik aanhangsels voor nu, BELANGRIJK IMPIRISCH FEIT: KETENS VAN AANHANGSELS KUNNEN GECONTOURLOPEN WORDEN]
	 * 
	 * Verwijder deze totdat er geen gevonden meer kunnen worden.
	 * NU IS HET BASALE PAD OVERGEBLEVEN. Loop deze door en op plekken waar een aanhangsel is moet een contourloop uitgevoerd worden.
	 * Dit is het pad.
	 */
}