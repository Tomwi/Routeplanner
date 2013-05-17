/* 
 * File:   openglfuncs.c
 * Author: Wessel Bruinsma
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "openglfuncs.h"
#include "tsp.h"
#define GLFW_DLL
#include <GL/glfw.h>
#include "astar.h"
#include <unistd.h>
#include "smartpath.h"

extern Node *grid[GRID_SIZE_X][GRID_SIZE_Y];
extern int oddNodesSize;
extern int *oddNodes;

void drawLine(double x1, double y1, double x2, double y2, int c) {
	if (!c) glLineWidth(4);
	else glLineWidth(2);
	if (!c) glColor3f(0.8, 0.8, 0.8);
	else glColor3d(1.0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawCircle(double x, double y, double r, int c) {
	glLineWidth(1);
	if (!c) glLineWidth(1);
	else glLineWidth(2);
	if (!c) glColor3f(0.8, 0.8, 0.8);
	else glColor3f(1.0, 0, 0);
	if (c==2) glColor3f(0,0,1);
	glBegin(GL_LINE_LOOP);

	int t;
	double t_inrad;

	for (t = 0; t < 360; t++) {
		t_inrad = (double) t / 180 * 3.1415;
		glVertex2f(x + cos(t_inrad) * r, y + sin(t_inrad) * r);
	}

	glEnd();
}

void openWindow() {

	/* Initialize the library */
	if (!glfwInit()) {
		printf("An error occurred");
		return;
	}

	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	if (!glfwOpenWindow(500, 500, 8, 8, 8, 0, 24, 0, GLFW_WINDOW)) {
		printf("An error occurred");
		return;
	}

	glfwSetWindowTitle("Window");
	glfwSetWindowPos(800, 100);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 500, 500, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	int ran = 0;

	/* Loop until the user closes the window */
	while (glfwGetWindowParam(GLFW_OPENED)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawGraph();

		/* Swap front and back buffers and process events */
		glfwSwapBuffers();

		if (!ran) {
			ran = 1;
			if (!AUTORUN) printf("Grid loaded, press <ENTER> to begin...\n\n");
			if (!AUTORUN) getchar();
			spExec();
		} else {
			spAfterExec();
		}
		
		usleep(1000*10);
	}
}

void drawGraph() {
	int x, y;

	for (x = 0; x < GRID_SIZE_X; x++) {
		for (y = 0; y < GRID_SIZE_Y; y++) {
			drawCircle((double) grid[x][y]->position.x, (double) (double) grid[x][y]->position.y, 5, (x==0&&y==0?2:grid[x][y]->odd));

			if (grid[x][y]->west) {
				drawLine(
						(double) grid[x][y]->position.x,
						(double) grid[x][y]->position.y,
						(double) grid[x][y]->west->position.x,
						(double) grid[x][y]->west->position.y, 0
						);
			} else {
				//printf("Que?\n");
			}
			
			if (grid[x][y]->south) {
				drawLine(
						(double) grid[x][y]->position.x,
						(double) grid[x][y]->position.y,
						(double) grid[x][y]->south->position.x,
						(double) grid[x][y]->south->position.y, 0
						);
			}
		}
	}
	
	int i;
	
	for (i = 0; i < oddNodesSize; i += 2) {
		Node* from = getNodeById(oddNodes[i]);
		Node* to = getNodeById(oddNodes[i + 1]);

		drawLine(
				from->position.x,
				from->position.y,
				to->position.x,
				to->position.y,
				1
				);
	}

}
