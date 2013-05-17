#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openglfuncs.h"
#define GLFW_DLL
#include <GL/glfw.h>
#include "astar.h"
#include "smartpath.h"
#include "path.h"
#include "places.h"

/**
 * Calculates recursively the number of possible ways to organize a set of 'totalsize' things into subsets of size 'setsize'. Totalsize must be a integer multiple of setsize.
 * @param setsize
 * @param totalsize
 * @return 
 */

int spPairCombs(int setsize, int totalsize) {
	if (setsize == totalsize) {
		return 1;
	} else {
		int setsize_f = 1, totalsize_msetsize_f = 1, totalsize_f = 1, i;

		// Calculate factorials
		for (i = 0; i < setsize; i++) setsize_f *= i + 1;
		for (i = 0; i < totalsize - setsize; i++) totalsize_msetsize_f *= i + 1;
		for (i = 0; i < totalsize; i++) totalsize_f *= i + 1;

		return (totalsize_f / setsize_f / totalsize_msetsize_f)*spPairCombs(setsize, totalsize - setsize);
	}
}

/**
 * Initialize the grid for graphical view
 */

extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];

void spInitGridGraphical() {
	int x, y;

	for (x = 0; x < GRID_SIZE_X; x++) {
		for (y = 0; y < GRID_SIZE_Y; y++) {
			grid[x][y]->position.x = x * X_SPACING + X_MARGIN;
			grid[x][y]->position.y = y * Y_SPACING + Y_MARGIN;
		}
	}
}

/**
 * Get node by id
 */

Node* getNodeById(int id) {
	int x, y;

	for (x = 0; x < GRID_SIZE_X; x++) {
		for (y = 0; y < GRID_SIZE_Y; y++) {
			if (grid[x][y]->id == id)
				return grid[x][y];
		}
	}

	return NULL;
}

/**
 * Get odd nodes
 */

int oddNodesSize;
int *oddNodes;

void getOddNodes() {
	int i;
	oddNodesSize = 0;
	oddNodes = malloc(sizeof (int) *NUM_NODES);

	for (i = 0; i < NUM_NODES; i++) {
		// Now count nodes
		Node* node = getNodeById(i);
		int nbs = 0;

		if (node->south) nbs++;
		if (node->north) nbs++;
		if (node->east) nbs++;
		if (node->west) nbs++;

		if (nbs % 2 == 1) { // This is an odd node
			oddNodes[oddNodesSize++] = i;
			node->odd = 1;
		} else if (nbs == 0) {
			printf("Lone node found [%d]\n", i);
		}
	}

	printf("%d odd node(s) found.\n", oddNodesSize);
}

/**
 * Pair odd nodes
 */

void pairOddNodesBruteForce(int curPairsSize, int* curPairs) {
	static int iteration = 0;
	iteration++;

	if (iteration % 10000 == 0) {
		printf("Iteration: %d0k\n", iteration / 10000);
	}

	if (curPairsSize == oddNodesSize) { // All nodes are paired, final call
		int scorebefore = rateCombs();
		save();

		// Test em
		int i;
		for (i = 0; i < oddNodesSize; i++) {
			oddNodes[i] = curPairs[i];
		}

		if (rateCombs() > scorebefore) {
			revert();
		} else {
			printf("More optimal solution found\n");
		}

		free(curPairs);
		return;
	}


	if (curPairsSize == 0) {
		curPairs = malloc(sizeof (int) *oddNodesSize);
	}

	int* dataSet = malloc(sizeof (int) *(oddNodesSize));
	int dataSetSize = 0;

	// Filter the current pairs
	int i, j, k;

	for (i = 0; i < oddNodesSize; i++) {
		// Now check if appears in curPairs
		int appears = 0;

		// Check if oddNodes[i] appears in curPairs
		for (j = 0; j < curPairsSize; j++) {
			if (curPairs[j] == oddNodes[i]) {
				appears = 1;
			}
		}

		// Does not appear in curPairs, add to dataSet
		if (!appears)
			dataSet[dataSetSize++] = oddNodes[i];
	}

	//printf("Pairing started...\n");
	//	int x = 0;
	// Now create all possible pairs from current dataset
	for (i = 0; i < dataSetSize; i++) {
		for (j = 0; j < dataSetSize; j++) {
			if (j != i && j > i) { // That way 'aa' wont appear and 'ab' and 'ba' wont be both checked, we only want to check combinations, not all permutations!

				int* newCurPairs = malloc(sizeof (oddNodes));
				int newCurPairsSize = curPairsSize + 2;

				// Now reconstruct
				for (k = 0; k < curPairsSize; k++) {
					newCurPairs[k] = curPairs[k];
				}

				newCurPairs[k++] = dataSet[i];
				newCurPairs[k++] = dataSet[j];

				pairOddNodesBruteForce(newCurPairsSize, newCurPairs);
			}
		}
	}

	free(dataSet);
}

/**
 * Genetic improving
 */

int rateCombs() {
	int i, length = 0;

	for (i = 0; i < oddNodesSize; i += 2) {
		length += getLengthBetween(i, i + 1);
	}

	return length;
}

int getLengthBetweenNodes(int node1, int node2) {
	Node* from = getNodeById(node1);
	Node* to = getNodeById(node2);

	Path_element *path = findShortestPath(
			from->positionGrid.x,
			from->positionGrid.y,
			1,
			to->positionGrid.x,
			to->positionGrid.y
			);

	int length = 0;
	
	while (path) {
		length += path->step_weight;
		path = path->next;
	}
	
	//free(path);
	return length;
}

int getLengthBetween(int node1, int node2) {
	return getLengthBetweenNodes(oddNodes[node1], oddNodes[node2]);
}

void shiftRight() {
	int i;
	int *temp;

	temp = malloc(sizeof (oddNodes));

	for (i = 0; i < oddNodesSize; i++) {
		temp[i] = oddNodes[i];
	}

	for (i = 1; i < oddNodesSize; i++) {
		oddNodes[i - 1] = temp[i];
	}

	oddNodes[oddNodesSize - 1] = temp[0];

}

void improveGenetically() {
	int i, j, k;


	/**
	 * MUTATION: SWITCH
	 */

	for (i = 0; i < oddNodesSize / 2; i++) {
		for (j = 0; j < oddNodesSize / 2; j++) {
			if (i != j) {
				for (k = 0; k < 2; k++) {
					int scorebefore = rateCombs();
					save();

					int temp = oddNodes[j * 2 + k];
					oddNodes[j * 2 + k] = oddNodes[i * 2];
					oddNodes[i * 2] = temp;


					int scoreafter = rateCombs();

					if (scoreafter > scorebefore) { // No improvement, revert
						revert();
					} else {
						printf("Mutation: switch\n");
					}
					//					}
				}
			}
		}
	}

		/**
	 * MUTATION: SHIFT
	 */

	for (i = 0; i < oddNodesSize; i++) {
		int scorebefore = rateCombs();
		save();

		for (j = 0; j < i; j++) shiftRight();

		int scoreafter = rateCombs();

		if (scoreafter > scorebefore) { // No improvement, revert
			revert();
		} else {
			printf("Mutation: shift\n");
		}
	}

	/**
	 * MUTATION: RANDOM ROTATION (LVL 3)
	 */

	int iterations = 500;

	for (i = 0; i < iterations; i++) {
		int first = rand() % oddNodesSize;
		int second = -1, third = -1;

		while (second == first || second == -1) {
			second = rand() % oddNodesSize;
		}

		while (third == first || third == second || third == -1) {
			third = rand() % oddNodesSize;
		}

		int scorebefore = rateCombs();
		save();

		int temp = oddNodes[second];
		oddNodes[second] = oddNodes[first];
		oddNodes[first] = oddNodes[third];
		oddNodes[third] = temp;

		int scoreafter = rateCombs();

		if (scoreafter > scorebefore) {
			revert();
		} else {
			printf("Mutation: rotation (lvl 3)\n");
		}
	}
}

int *saveData;

void initGen() {
	saveData = malloc(oddNodesSize * sizeof (int));
}

void save() {
	int i;
	for (i = 0; i < oddNodesSize; i++) {
		saveData[i] = oddNodes[i];
	}
}

void revert() {
	int i;
	for (i = 0; i < oddNodesSize; i++) {
		oddNodes[i] = saveData[i];
	}
}

void firstExpansion() {
	int scorebefore = rateCombs();
	save();

	int i, j;

	// Shuffle oddNodes
	for (i = oddNodesSize - 1; i > 0; i--) {
		j = rand() % (i + 1);
		int temp = oddNodes[i];
		oddNodes[i] = oddNodes[j];
		oddNodes[j] = temp;
		// switch i and j
	}

	int *newCombs = malloc(sizeof (oddNodes));
	int newCombsSize = 0;

	while (newCombsSize < oddNodesSize) {
		//		printf("Current size: %d\n", newCombsSize);

		// Find a starting node which is not in newCombs yet
		int now = -1;

		int *toPairWith = malloc(sizeof (int)*oddNodesSize);
		int toPairWithSize = 0;

		for (i = 0; i < oddNodesSize; i++) {
			int inSet = 0;

			for (j = 0; j < newCombsSize; j++) {
				if (newCombs[j] == oddNodes[i]) inSet = 1;
			}

			if (!inSet && now == -1) {
				now = oddNodes[i];
			} else if (!inSet) {
				toPairWith[toPairWithSize++] = oddNodes[i];
			}
		}

		// now = node to pair to closest node
		int closest = -1, dist;

		/**
		 * TO PAIR WITH PRINT
         */
//		printf("Content of toPairWith, size: %d:\n", toPairWithSize);
//		
//		for (i = 0; i < toPairWithSize; i++) {
//			printf("toPairWith[%d] = %d", i, toPairWith[i]);
//		}
		
		
		for (i = 0; i < toPairWithSize; i++) {
			
			if ((closest == -1 || dist > getLengthBetweenNodes(toPairWith[i], now)) && getLengthBetweenNodes(toPairWith[i], now) != 0) {
				//				printf("Update!\n");
				closest = toPairWith[i];
				dist = getLengthBetweenNodes(toPairWith[i], now);
			}

		}
		// save em

		newCombs[newCombsSize++] = now;
		newCombs[newCombsSize++] = closest;
	}

	// Apply
	for (i = 0; i < oddNodesSize; i++) {
		oddNodes[i] = newCombs[i];
	}

	if (rateCombs() > scorebefore) {
		revert();
	} else {
		printf("First expanded\n");
	}
}

/**
 * Execute
 */

void spExec() {
	printf("\n--------------------------------------------------\n-- Analysing odd nodes\n--------------------------------------------------\n");
	if (!AUTORUN) printf("Press <ENTER> to start...");
	if (!AUTORUN) getchar();
	getOddNodes();
	printf("[section end]\n");

	int maxBF = 150000, disabled = 1;
	printf("\n--------------------------------------------------\n-- Pairing odd nodes [brute force, estimation: %d]\n--------------------------------------------------\n", spPairCombs(2, oddNodesSize));
	if (spPairCombs(2, oddNodesSize) <= maxBF && !disabled) {
		if (!AUTORUN) printf("Press <ENTER> to start...\n");
		if (!AUTORUN) getchar();
		initGen();
		pairOddNodesBruteForce(0, NULL);
		printf("[section end]\n");
		goto skippairing;
	} else {
		printf("[section skipped - LIMITED AT %d | DISABLED]\n", maxBF);
	}


	int iterations = 1000;
	printf("\n--------------------------------------------------\n-- Pairing odd nodes [first expansion, iterations: %d]\n--------------------------------------------------\n", iterations);
	if (!AUTORUN) printf("Press <ENTER> to start...");
	if (!AUTORUN) getchar();
	initGen();
	printf("Score: %d\n", rateCombs());
	while (iterations--) {
		firstExpansion();
	}
	printf("Score: %d\n", rateCombs());

	//	iterations = 10;
	printf("\n--------------------------------------------------\n-- Pairing odd nodes [genetic algorithm, iterations: until saturation]\n--------------------------------------------------\n");
	printf("Mutation: switch [enabled]\n");
	printf("Mutation: rotation (lvl 3) [enabled]\n");
	printf("Mutation: shift [enabled]\n");
	if (!AUTORUN) printf("Press <ENTER> to start...");
	if (!AUTORUN) getchar();
	printf("Starting score: %d\n", rateCombs());

	//	while (iterations--) {
	//		printf("Running genetic algorithm... [%d iteration(s) left]\n", iterations + 1);
	//		improveGenetically();
	//		printf("Score: %d\n", rateCombs());
	//	}

	int prev = rateCombs(), threshold = 5, count = 1;

	while (count != threshold + 1) {
		printf("Running genetic algorithm... [saturation: %d/%d]\n", count, threshold);
		improveGenetically();
		printf("Score: %d\n", rateCombs());

		if (rateCombs() == prev) {
			count++;
		} else {
			prev = rateCombs();
			count = 1;
			printf("Improved!\n");
		}
	}

	printf("Final report:\n");

	int i;
	for (i = 0; i < oddNodesSize; i += 2) {
		Node* from = getNodeById(oddNodes[i]);
		Node* to = getNodeById(oddNodes[i + 1]);

		printf("(%d,%d) paired with (%d,%d) [%d]\n",
				from->positionGrid.x,
				from->positionGrid.y,
				to->positionGrid.x,
				to->positionGrid.y,
				getLengthBetweenNodes(oddNodes[i], oddNodes[i + 1])
				);
	}
	printf("Score: %d\n", rateCombs());

	printf("[section end]\n");

skippairing: // Label for skipping the genetic algorithm


	printf("\n\n\n[EXECUTION FINISHED]\n");
}

/**
 * After execution
 */

void spAfterExec() {
	while (1) {
		//		printf("Dist between <x1><SPACE><y1><SPACE><x2><SPACE><y2>\n");
		//		int x1, x2, y1, y2;
		//		scanf("%d %d %d %d", x1, y1, x2, y2);
		//		Path_element *path = findShortestPath(x1,y1,1,x2,y2);
		//		int length = 0;
		//		while (path) {
		//			length += path->step_weight;
		//			path = path->next;
		//		}
		//		//free(path);
		//		printf("Length: %d\n", length);
		int x1, x2, y1, y2;
				scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
		Path_element* path = findShortestPath(x1, y1, 1, x2, y2);
		int len = 0;
		while (path) {
			len += path->step_weight;
			path = path->next;
		}
		printf("Length: %d\n", len);
	}
}