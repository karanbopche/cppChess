#pragma once

#include "engine/consoleEngine.h"

// box size 8x8 each box 5 char x 5 char
extern const int boxRowSize;
extern const int boxColSize;
extern const int boardWidth;
extern const int boardHeight;


class peice: public entity {
protected:
	Point location;
	int playerId;
	void setPlayerId(int Id);	// set to which player peice belongs....
public:
	virtual ~peice();	// virtual destroyer....
	int getPlayerId();
	void setLocation(Point p);
	Point getLocation();
	
	virtual void onCreate();
	virtual bool canMove(Point To);
	virtual char getPeiceId()=0;
};

