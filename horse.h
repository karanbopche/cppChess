#pragma once

#include "peice.h"

class horse : public peice {
public:
	horse();
	horse(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
