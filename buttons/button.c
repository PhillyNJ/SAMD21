/*
 * button.c
 *
 * Created: 11/18/2017 3:17:24 PM
 *  Author: pvallone
 */ 
 #include "button.h"

 extern const unsigned short up24x24[0x240];
 extern const unsigned short next24x24[0x240];
 extern const unsigned short prev24x24[0x240];
 extern const unsigned short downBtn24x24[0x240];

 extern void eventUpHandler(int index);
 extern void eventDownHandler(int index);
 extern void eventNextHandler(int index);
 extern void eventPrevHandler(int index);

 void init_buttons(){
	uint8_t i = 0;
	
	struct button_t btn;
	btn.btype = IMAGEFROMMEMORY;
	btn.height = 24;
	btn.width = 24;
	btn.img = up24x24;
	btn.imgSize = 0x240;
	btn.x = 200;
	btn.y = 75;	
	btn.event = &eventUpHandler;
	buttons[i] = btn;

	i++;

	btn.img = downBtn24x24;
	btn.imgSize = 0x240;
	btn.x = 200;
	btn.y = 125;
	btn.event = &eventDownHandler;
	buttons[i] = btn;
	
	i++;

	btn.img = prev24x24;
	btn.imgSize = 0x240;
	btn.x = 175;
	btn.y = 100;
	btn.event = &eventPrevHandler;
	buttons[i] = btn;
	
	i++;

	btn.img = next24x24;
	btn.imgSize = 0x240;
	btn.x = 225;
	btn.y = 100;
	btn.event = &eventNextHandler;
	buttons[i] = btn;


 }