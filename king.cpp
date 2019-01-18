#include "king.h"

king::king() {
	onCreate();
}

king::king(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void king::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| King  |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool king::canMove(Point to) {
	// can move arround one block......
	if(abs(to.x - location.x) <= 1 && abs(to.y - location.y) <= 1) 
		return true;
	return false;
}

char king::getPeiceId() {	return 'k';	}

