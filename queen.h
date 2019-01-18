#pragma once

#include "peice.h"

class queen : public peice {
public:
	queen();
	queen(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
