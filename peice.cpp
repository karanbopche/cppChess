#include "peice.h"
// peice generic code..........
bool peice::canMove(Point p) {
	return true;
}

void peice::onCreate() {
	width = boxColSize;
	height = boxRowSize;
	sprite = new char*[height];
	for(int i=0;i<height;i++)
		sprite[i] = new char[width];
	strncpy(sprite[0], "|-------|", width);
	strncpy(sprite[1], "|       |", width);
	strncpy(sprite[2], "| peice |", width);
	strncpy(sprite[3], "|-------|", width);
}

void peice::setPlayerId(int Id) { playerId = Id; }

int peice::getPlayerId() { return playerId; }

void peice::setLocation(Point p) {
	location = p;
	set(location.x*width, location.y*height);	// set x y location of entity for printing......
}

Point peice::getLocation() {
	return location; 
}

peice::~peice() { }

char peice::getPeiceId() {	return 'x'; }
