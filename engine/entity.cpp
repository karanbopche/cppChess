#include "entity.h"

using namespace std;

// entity class...............
entity::entity() {
	sprite = NULL;
	color[0] = '\0';
	width = 0;
	height = 0;
	set(0, 0);
}

entity::entity(int x, int y) {
	sprite = NULL;
	color[0] = '\0';
	width = 0;
	height = 0;
	set(x, y);
}

entity::~entity() {
	freeSprite();
}

void entity::setColor(const char* col, const char* bg) {
	color[0] = '\0';	// for new color string.........
	if(col)
		strcpy(color, col);
	if(bg)
		strcat(color, bg);
}

void entity::freeSprite() {
	for(int h=0;h<height;h++)
		delete[] sprite[h];
	delete[] sprite;
	height = 0;
	width = 0;
	sprite = NULL;
}

void entity::testSprite(int w, int h) {
	sprite = new char*[h];
	for(int i=0;i<h;i++) {
		sprite[i] = new char[w];
		for(int j=0;j<w;j++)
			sprite[i][j] = '0';
	}
	width = w;
	height = h;	
}

void entity::set(int x, int y) {
	this->x = x;
	this->y = y;
}

void entity::move(int dx, int dy) {
	x += dx;
	y += dy;
}

bool entity::collision(entity& object) { 

	if(((object.x > x)&&(object.x < x+width)) || ((object.x+object.width > x)&&(object.x+object.width < x+width)))
		if(((object.y > y)&&(object.y < y+height)) || ((object.y+object.height > y)&&(object.y+object.height < y+height)))
			return true;
	return false;
}
