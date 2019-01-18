#include "horse.h"

horse::horse() {
	onCreate();
}

horse::horse(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void horse::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| Horse |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool horse::canMove(Point to) {
	int dx = abs(to.x - location.x);	// x distance....
	int dy = abs(to.y - location.y);	// y distance....
	if( (dx == 2 && dy == 1) || (dx == 1 && dy == 2) )
		return true;
	return false;
}

char horse::getPeiceId() {	return 'h';	}
