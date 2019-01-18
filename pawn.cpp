#include "pawn.h"

pawn::pawn() {
	onCreate();
}

pawn::pawn(int x, int y, int Id) {
	Point p = {x,y};
	onCreate();
	setLocation(p);
	playerId = Id;		// set to which player it belongs......
}

void pawn::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	// create sprite for pawn............
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| Pawn  |", width);
	strncpy(sprite[3], "|-------|", width);
}

bool pawn::canMove(Point to) {
	int dx = location.x - to.x;
	int dy = location.y - to.y;
	
	if(abs(dy)<=2 && (dy!=0) && abs(dx)<=1) {	// in reach only...... 
		if(playerId == 1) {
			if(dy == 1)	// normal move single step........
				return true;
			// if initial location then only jump twosteps with no digonal
			else if(location.y == 6 && abs(dx)==0)	
				return true;
		}
		if(playerId == 2) {
			if(dy == -1)	// normal move........
				return true;
			else if(location.y == 1  && abs(dx)==0)	// initial condition
				return true;
		}
	}
	return false;
}

char pawn::getPeiceId() {	return 'p';	}

