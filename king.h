#pragma once

#include "peice.h"

class king : public peice {
public:
	king();
	king(int x, int y, int Id);
	void onCreate();
	bool canMove(Point To);
	char getPeiceId();
};
