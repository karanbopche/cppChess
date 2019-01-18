#pragma once

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "color.h"
#include <unistd.h>


enum keyboard {
	KEY_NULL = 0,
	KEY_0 = '0', KEY_1 , 
	KEY_2, KEY_3, KEY_4, KEY_5 ,
	KEY_6, KEY_7, KEY_8, KEY_9 ,
	 
	KEY_A = 'A',
	KEY_B ,KEY_C ,KEY_D ,KEY_E ,
	KEY_F ,KEY_G ,KEY_H ,KEY_I ,
	KEY_J ,KEY_K ,KEY_L ,KEY_M ,
	KEY_N ,KEY_O ,KEY_P ,KEY_Q ,
	KEY_R ,KEY_S ,KEY_T ,KEY_U ,
	KEY_V ,KEY_W ,KEY_X ,KEY_Y ,
	KEY_Z ,
	KEY_UP=1000,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ENTER = 13,
	KEY_SPACE = 32
};

struct Point {
	int x, y;
};


