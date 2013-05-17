#include <stdio.h>
#include <stdlib.h>
#include "tsp.h"
#include "openglfuncs.h"
#define GLFW_DLL
#include <GL/glfw.h>
#include "astar.h"
#include "smartwalk.h"

extern li** nodes;
int kill = 0, finalpathsize;
int *finalpath;
int debug = 0;

#define DEBUG 0

void smartwalkgo() {
	int *start = malloc(sizeof (int) * MAX_PATH_LENGTH);
	start[0] = 0;
	int i;

	//smartwalk(1, start);

#if DEBUG == 0
	printf("Closed path found: ");
#endif

	for (i = 0; i < finalpathsize; i++) {
#if DEBUG == 0
		printf("%d ", finalpath[i]);
#endif
	}

#if DEBUG == 0
	printf("\n");
#endif
}

void drawPath() {
	int i;

	for (i = 0; i < finalpathsize - 1; i++) {
		drawLine(nodes[finalpath[i]]->pos->x,
				nodes[finalpath[i]]->pos->y,
				nodes[finalpath[i + 1]]->pos->x,
				nodes[finalpath[i + 1]]->pos->y, 1);
	}
}

void smartwalk(int psize, int* path) {
	int i, j, closed_path = 1;

#if DEBUG == 1
	printf("== New call [node: %d, path size: %d]\n", nodes[path[psize - 1]]->id, psize);
#endif

	for (i = 0; nodes[path[psize - 1]]->nbs[i] != NULL; i++) {
#if DEBUG == 1
		printf("- Nb %d [%d]\n", nodes[path[psize - 1]]->nbs[i]->id, i);
#endif
		int visited = 0;

		for (j = 0; j < psize; j++) {
			if (nodes[path[psize - 1]]->nbs[i]->id == path[j]) {
				visited = 1;
			}
		}

		if (!visited) {
			int *npath = malloc(sizeof (int) * MAX_PATH_LENGTH);
			int npsize = psize;

#if DEBUG == 1
			printf("Node not visited yet, copying..\n");
#endif
			memcpy(npath, path, sizeof (int) * MAX_PATH_LENGTH);
			npath[npsize++] = nodes[path[psize - 1]]->nbs[i]->id;
#if DEBUG == 1
			printf("..done.\n");
#endif

			closed_path = 0;

			if (npsize <= MAX_PATH_LENGTH) {


				if (0){//!connectivityCheck(npsize, npath)) {
#if DEBUG == 0
					printf("Connectivity failed...\n");
#endif
				} else {
					//printf("Connected still!");
					
					if (!kill) smartwalk(npsize, npath);
					else {
#if DEBUG == 1
						printf("== Call killed\n");
#endif
					}
				}

			} else {
#if DEBUG == 1
				printf("== Call killed [LENGTH OVERFLOW]\n");
#endif
			}

			free(npath);
		} else {
#if DEBUG == 1
			printf("Node already visited.\n");
#endif
		}
	}

#if DEBUG == 1
	printf("== End of call [node: %d, path size: %d]\n", nodes[path[psize - 1]]->id, psize);
#endif

	if (closed_path) {
#if DEBUG == 1
		printf("Possible path found, checking...\n");
#endif

		int fail = 0;

		if (!completionCheck(psize, path)) {
#if DEBUG == 1
			printf("Verification failed...\n");
#endif
			fail = 1;
		}

		if (!fail) {
#if DEBUG == 1
			printf("Path verified.\n");
#endif
			finalpathsize = psize;
			finalpath = path;
			kill = 1;
		} else {
//			printf("Path failed...\n");
		}
	}
}

int completionCheck(int psize, int*path) {
	if (psize >= NUMBER_OF_NODES - NUMBER_OF_MINES-5) return 1;
//	printf("Completion fucked...");
	return 0;
}

int connectivityCheck(int psize, int*path) {
	int i, j;

	for (i = 0; i < NUMBER_OF_NODES; i++) {
		int inset = 0;

		for (j = 0; j < psize; j++) {
			if (path[j] == i) {
				inset = 1;
			}
		}

		if (!inset) {
			for (j = 0; j < psize; j++) {
				isolateNode(path[j]);
			}

			printf("%d vs %d \n", getNodeRange(i), NUMBER_OF_NODES - psize);
			clearCheckset();
			
			if (getNodeRange(i) != NUMBER_OF_NODES - psize) {
				clearCheckset();
				
				init();
				return 0;
			} else {
				clearCheckset();
				
				init();
				printf("Checked!");
				return 1;
			}
		}
	}

	return 1;
}