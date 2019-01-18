#include "empty.h"

empty::empty() {
	onCreate();
}

empty::empty(int x, int y) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = 0;		// set to which player it belongs......
}

void empty::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "|       |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool empty::canMove(Point to) {
	return false;
}

char empty::getPeiceId() {	return ' ';	}


