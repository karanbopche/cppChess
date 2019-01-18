#include "elephant.h"

elephant::elephant() {
	onCreate();
}

elephant::elephant(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void elephant::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| rook  |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool elephant::canMove(Point to) {
	// EX-OR the distance in X and Y to only allow move in horizontal or vertical......
	if((abs(to.x - location.x) != 0) ^ (abs(to.y - location.y)!=0))
		return true;
	return false;
}

char elephant::getPeiceId() {	return 'e';	}

