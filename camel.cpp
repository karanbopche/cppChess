#include "camel.h"

camel::camel() {
	onCreate();
}

camel::camel(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void camel::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| camel |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool camel::canMove(Point to) {
	// point should be digonal hence X and Y distance is equal...
	if(abs(to.x - location.x) == abs(to.y - location.y))
		return true;
	return false;
}

char camel::getPeiceId() {	return 'c';	}

