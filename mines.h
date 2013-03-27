/* 
 * File:   mines.h
 * Author: Wessel Bruinsma
 *
 * Created on March 25, 2013, 7:01 PM
 */

#ifndef MINES_H
#define	MINES_H

int mineAtConnection(int, int, int, int);
void createMines(void);
int discoverMines(int,int);
void revealMines(void);

#endif	/* MINES_H */

