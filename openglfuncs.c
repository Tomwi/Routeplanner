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

extern li **nodes;

void drawLine(double x1, double y1, double x2, double y2) {
	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void drawCircle(double x, double y, double r) {
	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);

	int t;
	double t_inrad;
	
	for (t = 0; t < 360; t++) {
		t_inrad = (double)t/180*3.1415;
		glVertex2f(x+cos(t_inrad)*r, y+sin(t_inrad)*r);
	}
	
	glEnd();
}

void openWindow() {
	
	int execed = 0;
	
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
	glfwSetWindowPos(800,100);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 500, 500, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	/* Loop until the user closes the window */
	while (glfwGetWindowParam(GLFW_OPENED)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw();

		/* Swap front and back buffers and process events */
		glfwSwapBuffers();
		
		if (!execed) {
			execed = 1;
			
			printf("---------------\n\nPress <ENTER> to start...\n\n");
			getchar();
			execute();
		}
		
	}
}

void draw() {
	int i,j;
	
	for (i = 0; i < NUMBER_OF_NODES; i++) {
		drawCircle((double)nodes[i]->pos->x, (double)nodes[i]->pos->y, 5);
		
		for (j = 0; nodes[i]->nbs[j] != NULL; j++) {
			drawLine(
					(double)nodes[i]->pos->x,
					(double)nodes[i]->pos->y,
					(double)nodes[i]->nbs[j]->pos->x,
					(double)nodes[i]->nbs[j]->pos->y
			);
		}
	}
	
	
}
