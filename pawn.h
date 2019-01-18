#pragma once

#include "peice.h"

class pawn : public peice {
public:
	pawn();
	pawn(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
