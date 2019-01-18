#pragma once

#include "peice.h"

class elephant: public peice {

public:
	elephant();
	elephant(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
