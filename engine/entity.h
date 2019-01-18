#pragma once

#include "headers.h"

class entity {
protected:
	int x, y;
	char **sprite;
	int width, height;
	char color[15];
public:
	entity();
	entity(int x, int y);
	~entity();
	
	void testSprite(int w, int h);
	void set(int x, int y);
	void setColor(const char* c, const char* bg=NULL);
	void move(int dx, int dy);
	bool collision(entity&);
	void freeSprite();
	friend class render;

	// user need to override following functions.......
	//virtual void onCreate()=0;
/*	virtual void onEvents(keyboard)=0;
	virtual void onUpdate()=0;
	virtual void onExit()=0;
*/
};

