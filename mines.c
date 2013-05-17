/* 
 * File:   main.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>

#include "mines.h"
#include "astar.h"

int mine_locations[NUMBER_OF_MINES][4];
extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];

/**
 * Creates the mines in the grid.
 */

void createMines() {
	int available_mine_spots[GRID_SIZE_X * GRID_SIZE_Y * 2][4];
	int x, y, i, j, index = 0;

	// Find available spots by walking through the grid and saving each connection of a node with his south and east neightbour.
	for (x = 0; x < GRID_SIZE_X; x++) {
		for (y = 0; y < GRID_SIZE_Y; y++) {
			// Check if the right neighbour exists and act accordingly
			if (grid[x + 1][y] && x < GRID_SIZE_X - 1) {
				available_mine_spots[index][0] = x;
				available_mine_spots[index][1] = y;
				available_mine_spots[index][2] = x + 1;
				available_mine_spots[index++][3] = y;
			}

			// Check if the bottom neighbour exists and act accordingly
			if (grid[x][y + 1] && y < GRID_SIZE_Y - 1) {
				available_mine_spots[index][0] = x;
				available_mine_spots[index][1] = y;
				available_mine_spots[index][2] = x;
				available_mine_spots[index++][3] = y + 1;
			}
		}
	}

	// Shuffle array by using the Fisher-Yates shuffle algorithm
	for (i = index - 1; i > 0; i--) {
		j = rand() % (i + 1);
		// Swap
		swap(&available_mine_spots[i][0], &available_mine_spots[j][0]);
		swap(&available_mine_spots[i][1], &available_mine_spots[j][1]);
		swap(&available_mine_spots[i][2], &available_mine_spots[j][2]);
		swap(&available_mine_spots[i][3], &available_mine_spots[j][3]);
	}

	// Save the first NUMBER_OF_MINES mines in the mine_locations array, using the shuffled array
	for (i = 0; i < NUMBER_OF_MINES; i++) {
		mine_locations[i][0] = available_mine_spots[i][0];
		mine_locations[i][1] = available_mine_spots[i][1];
		mine_locations[i][2] = available_mine_spots[i][2];
		mine_locations[i][3] = available_mine_spots[i][3];
	}
}

/**
 * Checks if there is a mine at the given location.
 * @param x1 <int> x of the first node
 * @param y1 <int> y of the first node
 * @param x2 <int> x of the second node
 * @param y2 <int> y of the second node
 * @return <int> boolean wether a mine is at the given location
 */

int mineAtConnection(int x1, int y1, int x2, int y2) {
	int i;

	// First check if the connection is valid
	if (checkConnection(x1, y1, x2, y2)) {
		// Now walk through all the mines and check if a location matches
		for (i = 0; i < NUMBER_OF_MINES; i++) {
			if (
					(mine_locations[i][0] == x1 && mine_locations[i][1] == y1 && mine_locations[i][2] == x2 && mine_locations[i][3] == y2) ||
					(mine_locations[i][0] == x2 && mine_locations[i][1] == y2 && mine_locations[i][2] == x1 && mine_locations[i][3] == y1)
					) {
				return 1;
			}
		}
	}

	// No value returned before, so no mine at the given location, so return 0
	return 0;
}

/**
 * Discovers mines at a given point.
 * @param x <int> x of the given location
 * @param y <int> y of the given location
 */

int discoverMines(int x, int y) {
	// Define return value
	int return_value = 0;

	// Now check north, south, east and west of the node if there is a mine.
	// If a mine is found, remove the connection and set the return value to 1.
	if (mineAtConnection(x, y, x, y + 1)) {
		removeConnection(x, y, x, y + 1);
		return_value = 1;
	}

	if (mineAtConnection(x, y, x + 1, y)) {
		removeConnection(x, y, x + 1, y);
		return_value = 1;
	}

	if (mineAtConnection(x, y, x - 1, y)) {
		removeConnection(x, y, x - 1, y);
		return_value = 1;
	}

	if (mineAtConnection(x, y, x, y - 1)) {
		removeConnection(x, y, x, y - 1);
		return_value = 1;
	}

	return return_value;
}

/**
 * Reveals all mines.
 */

void revealMines() {
	int i;

	// Loop through all the mine locations and then removing the connections in which the mines are located
	for (i = 0; i < NUMBER_OF_MINES; i++) {
		removeConnection(mine_locations[i][0], mine_locations[i][1], mine_locations[i][2], mine_locations[i][3]);
	}
}
