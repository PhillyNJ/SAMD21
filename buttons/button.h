/*
 * button.h
 *
 * Created: 11/18/2017 3:16:14 PM
 *  Author: pvallone
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_
#include "asf.h"
#define NUMBEROFBUTTONS 4

void init_buttons(void);

struct b_color_t{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

enum button_type{

	BUTTON = 0,
	IMAGE = 1,
	IMAGEFROMMEMORY = 2,

};

struct button_t{
	
	enum button_type btype;	
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	char text[20];
	char src[20];
	uint8_t value;
	struct b_color_t fontColor;
	struct b_color_t backColor;
	const uint16_t *img;
	int imgSize;
	void (* event)(int);
};

struct button_t buttons[NUMBEROFBUTTONS];

#endif /* BUTTON_H_ */