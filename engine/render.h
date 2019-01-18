#pragma once

#include <termios.h>
#include <sys/ioctl.h>
#include "headers.h"
#include "entity.h"

class render {
	char **buffer;	// character buffer to render.....
	const char ***colorBuffer;	// char* buffer for background forground colorChange....
	int width, height;
	struct termios original;// original settings of terminal....
	void termSet();		// enter raw display mode on terminal....
	void termReset();		// reset display setting of terminal....

public:
	render();
	render(int w, int h);
	~render();
	void create(int w, int h);// create buffer for display...
	void update();	// flush buffer to display...
	int getEvents(keyboard*);	// take keypress from terminal..
	void clear();	// clear buffer. and screen.....
	void print(entity&);	// print entity on buffer..
	// rudementry draw functions............
	void drawPixel(int x, int y, const char pixel);
	void drawLine(Point start, Point end, char pixel);
	void drawText(int x, int y, const char *str);
};


