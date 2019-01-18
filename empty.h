#pragma once

#include "peice.h"

class empty : public peice {
public:
	empty();
	empty(int x, int y);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
