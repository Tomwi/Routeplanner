/* 
 * File:   smartpath.h
 * Author: Wessel Bruinsma
 *
 * Created on May 16, 2013, 5:20 PM
 */

#ifndef SMARTPATH_H
#define	SMARTPATH_H

#define X_SPACING 100
#define Y_SPACING 100
#define X_MARGIN 50
#define Y_MARGIN 50

#define AUTORUN 0

#define NUM_NODES GRID_SIZE_X*GRID_SIZE_Y

int spPairCombs(int,int);
void spInitGridGraphical();
void getOddNodes();
void msgLarge(char*);
void spExec();
void spAfteExec();

int getLengthBetween(int,int);
Node* getNodeById(int);

void revert(void);
void save(void);

int rateCombs();

#endif	/* SMARTPATH_H */

