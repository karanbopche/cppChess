#include "render.h"

using namespace std;

// render class...............
render::render() {
	buffer = NULL;
	colorBuffer = NULL;
	create(0, 0);
}

render::render(int w, int h) {
	buffer = NULL;
	colorBuffer = NULL;
	create(w, h);
}

void render::termSet() {
	// set terminal for raw mode...
	struct termios raw;
	tcgetattr(STDIN_FILENO, &original);
	raw = original;
	raw.c_lflag = ~(ECHO | ICANON);	// no echo keypress and enter raw mode..
	raw.c_cc[VTIME] = 1;	// read timeout = 100mSec
	raw.c_cc[VMIN] = 0;	// read minimum 0 char to return
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void render::drawPixel(int x, int y, const char pixel) {
	if(x<width && y<height)
		buffer[y][x] = pixel;
}

void render::drawText(int x, int y, const char* str) {
	while(*str) {
		drawPixel(x++, y, *str++);
	}
}

void render::drawLine(Point from, Point to, char pixel) {
	if(from.x > to.x)
		swap(from.x, to.x);
	if(from.y > to.y)
		swap(from.y, to.y);
		
	for(int row=from.y;row<=to.y;row++)
		for(int col=from.x;col<=to.x;col++)
			drawPixel(col, row, pixel);
}


int render::getEvents(keyboard* key) {
	char k[3];
	int ret =  read(STDIN_FILENO, k, 3);
	if(k[0] == 27 && k[1] == '[')	{ // escape character...
		if(k[2] == 'A')
			*key = KEY_UP;
		else if(k[2] == 'B')
			*key = KEY_DOWN;
		else if(k[2] == 'C')
			*key = KEY_RIGHT;
		else if(k[2] == 'D')
			*key = KEY_LEFT;
		else if(k[2] == 10 )
			*key = KEY_ENTER;
		else if(k[2] == 32 )
			*key = KEY_SPACE;
	}
	else	// normal character.....
		*key = (keyboard)(k[0] & ~(1<<5));	//  MAKE THEM UPPER CASE...
	return ret;
}

void render::termReset() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);	// reset terminal settings to default...
	write(STDOUT_FILENO, "\x1b[1J\x1b[H",7);	// clear screen and cursor to start.......
}

void render::create(int w, int h) {
	delete[] buffer;
	delete[] colorBuffer;
	if(w == 0 || h == 0) {
		struct winsize ws;
		if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
			exit(-1);
		}
		width = ws.ws_col;
		height = ws.ws_row;
	}
	else {
		width = w;
		height = h;
	}
	buffer = new char*[height];
	colorBuffer = new const char**[height];
	for(int i=0;i<height;i++) {
		buffer[i] = new char[width];
		colorBuffer[i] = new const char*[width+1];	// extra for reseting last pixel color........
	}
	termSet();	// set terminal in raw mode......
}
void render::update() {
	// top line.........
	for(int h=0;h<=width;h++)
		cout<<'-';
	cout<<endl;
	// flush buffer.......
	for(int h=0;h<height;h++) {
		cout<<"|";
		for(int w=0;w<width;w++) {
			if(colorBuffer[h][w])
				cout<<colorBuffer[h][w];	// set color of forground and background.....
			cout<<buffer[h][w];			// display object data.......
		}
		cout<<C_RST<<"|\n";	// avoid color over flow...........
	}
//	 bottom line.....
	for(int h=0;h<=width;h++)
		std::cout<<'-';
	std::cout<<std::endl;
}

void render::print(entity& object) {
	int w, h;
///*	// calculate  the upper and lower bounds for loop.......
	int hUpperLimit = (object.height+object.y)>height ? height :(object.height+object.y);
	int wUpperLimit = (object.width+object.x)>width ? width : (object.width+object.x);
	// if object location is negative set lower bound to 0 otherwise check if out of bound then limit to canvas dimentions....
	int hLowerLimit = (object.y < 0) ? 0 : ( (object.y<height) ? object.y : height);	 
	int wLowerLimit = (object.x < 0) ? 0 : ( (object.x<width) ? object.x : width);
	
	for(h=hLowerLimit; h<hUpperLimit; h++) {
		if(object.color[0] != '\0') {	// if color is present.....
			if( object.x+object.width >= 0 && object.x < width) {// to avoid coloring for objects outside the
				colorBuffer[h][wLowerLimit] = object.color;
				colorBuffer[h][wUpperLimit] = C_RST;
			}
		}
		for(w=wLowerLimit; w<wUpperLimit; w++) {
			buffer[h][w] = object.sprite[h - object.y][w - object.x];
		}	
	}
}

void render::clear() {
	int w, h;
	for(h=0;h<height;h++)
		for(w=0;w<width;w++) {
			buffer[h][w] = ' ';
			colorBuffer[h][w] = NULL;
		}
	write(STDOUT_FILENO, "\x1b[1J\x1b[H",7);	// clear screen and cursor to start.......
	
}

render::~render() {
	for(int i=0;i<height;i++){
		delete[] buffer[i];
		delete[] colorBuffer[i];
	}
	delete[] buffer;
	delete[] colorBuffer;
	termReset();
}

