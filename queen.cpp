#include "queen.h"

queen::queen() {
	onCreate();
}

queen::queen(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void queen::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| queen |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool queen::canMove(Point to) {
	// check adjacent
	if(abs(to.x - location.x) <= 1 && abs(to.y - location.y) <= 1)
		return true;
	// EX-OR the distance in X and Y to only allow move in horizontal or vertical......
	if((abs(to.x - location.x)!=0) ^ (abs(to.y - location.y)!=0))
		return true;
	// point should be digonal hence X and Y distance is equal...
	if(abs(to.x - location.x) == abs(to.y - location.y))
		return true;	
	return false;
}

char queen::getPeiceId() {	return 'q';	}
