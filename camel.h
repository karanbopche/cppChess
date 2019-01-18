#pragma once

#include "peice.h"

class camel : public peice {
public:
	camel();
	camel(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
