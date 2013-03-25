/* 
 * File:   places.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>

#include "places.h"
#include "astar.h"

int places_to_visit[3][3];
int no_total_path_possible = 0;

/**
 * Asks the user which places need to be visited.
 */

void getPlacesToVisit() {
    int i = 0;

    // Walk through all places and get which coordinates are associated with it
    for (i = 0; i < 3; i++) {
	int x, y;
	cls();
	printf("Please enter place %d/%d to visit (<x><space><y>):\n", (i + 1), NUMBER_OF_PLACES_TO_VISIT);
	scanf("%d %d", &x, &y);
	places_to_visit[i][0] = x;
	places_to_visit[i][1] = y;
	places_to_visit[i][2] = 0;
    }
}

/**
 * Gets the destination considering the currect location and state of the field.
 * @param x <int> current x
 * @param y <int> current y
 * @param facing_direction <int> currect facing direction
 * @param debug <int> print stack
 * @return index of places_to_visit which is also the destination
 */

int getTargetPlace(int x, int y, int facing_direction, int debug) {
    // Initialization
    int empty_data[] = {-1, -1, -1}, constant_data[] = {x, y, facing_direction}, max_depth, i;
    Total_path *output = malloc(sizeof (Total_path));
    output->length = -1;

    // Set output places
    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	output->places[i] = -1;
    }

    // Max depth is number of unvisited places
    max_depth = 0;

    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	if (places_to_visit[i][2] == 0) max_depth++;
    }

    // Run the recursive algorithm to determine the best total path
    findBestTotalPath(0, max_depth, empty_data, constant_data, &output, debug);

    // Debugging output
    if (debug) {
	printf("\n\nShortest path: ");

	for (i = 0; i < max_depth; i++) {
	    printf("%d", output->places[i]+1);

	    if (i != max_depth - 1) printf(" - ");
	}

	printf(" => %.3d\n", output->length);
    }

    // Return next destination
    return output->places[0];
}

/**
 * Recursive algorithm to determine which total path is the most efficient.
 * Uses brute force to determine recursivly the most efficient total path.
 * @param depth <int> current depth (should be 0 in its initial call)
 * @param max_depth <int> maximum depth of the algorithm
 * @param data <int[]> stack of the places to be visited
 * @param constant_data <int[]> array containing the current x position, y position and facing direction
 * @param output <Total_path**> contains the most efficient path
 * @param debug <int> print stack
 */

void findBestTotalPath(int depth, int max_depth, int data[], int constant_data[], Total_path** output, int debug) {
    // Initialization
    int i, j, skip, length, facing_direction = -1, corrupt;
    Path *path;

    if (depth == max_depth) {
	// Determine total path length
	length = 0;
	corrupt = 0;
	for (j = -1; j < depth - 1; j++) {
	    // If no initial facing direction was set yet, set the current one
	    if (facing_direction == -1) facing_direction = constant_data[2];

	    // Get the details of the path between data[j] and data[j+1], data[-1] corresponds to the currect location
	    path = getPathDetails(
		    (j == -1 ? constant_data[0] : places_to_visit[data[j]][0]),
		    (j == -1 ? constant_data[1] : places_to_visit[data[j]][1]),
		    facing_direction,
		    places_to_visit[data[j + 1]][0],
		    places_to_visit[data[j + 1]][1]
		    );

	    facing_direction = path->facing_direction;
	    length += path->length;

	    // If the length of the path is 0, the findShortestPath returned NULL, which means no path between the two locations is possible
	    if (path->length == 0) {
		no_total_path_possible = 1;
		corrupt = 1;
	    }
	}

	// Debugging
	if (debug) {
	    for (j = 0; j < depth; j++) {
		printf("%d", data[j]+1);

		if (j != depth - 1) printf(" - ");
	    }

	    printf(" [%.3d]", length);
	}

	// If the path is not corrupt and better than the current one, store it
	if (((*output)->length == -1 || length < (*output)->length) && corrupt == 0) {
	    for (j = 0; j < depth; j++) {
		(*output)->places[j] = data[j];
	    }

	    (*output)->length = length;

	    if (debug) printf(" (Shortest so far)\n");
	} else {
	    if (debug) printf("\n");
	}
    } else {
	// The depth of the algorithm is not yet on its maximum, so go on
	for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	    skip = 0;

	    // Specifies that only places which has not been visited yet can be added to the data
	    for (j = 0; j < depth; j++) {
		if (data[j] == i) skip = 1;
	    }

	    if (skip) continue;

	    // If a valid new data entry has been found, add it to the data
	    if (places_to_visit[i][2] == 0) {
		data[depth] = i;
		findBestTotalPath(depth + 1, max_depth, data, constant_data, output, debug);
	    }
	}
    }
}

/**
 * Determines whether a place-to-visit has been visited, and if nessecary, marks it.
 * @param x <int> current x location
 * @param y <int> current y location
 */
void visit(int x, int y) {
    int i;

    // Walks through all places-to-visit, comparing coordinates
    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	if (places_to_visit[i][0] == x && places_to_visit[i][1] == y) {
	    places_to_visit[i][2] = 1;
	}
    }
}

/**
 * Marks all places as unvisited.
 */
void unvisitAll() {
    int i;

    // Walks through all places, marking them as unvisited
    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	places_to_visit[i][2] = 0;
    }
}

/**
 * Determines wether all places-to-visit have been visited.
 * @return <int> all places-to-visit have been visited
 */
int visitedAllPlaces() {
    int i;

    // Walk thourgh all places, checking if they have been visited
    for (i = 0; i < NUMBER_OF_PLACES_TO_VISIT; i++) {
	if (places_to_visit[i][2] == 0) return 0;
    }

    return 1;
}

