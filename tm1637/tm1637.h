/*
 * tm1637.h
 *
 * Created: 9/23/2020 6:32:31 PM
 *  Author: pvallone
 */ 


#ifndef TM1637_H_
#define TM1637_H_
#include <asf.h>
#include <stdbool.h>
#include <inttypes.h>
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44

#define STARTADDR  0xc0
/**** definitions for the clock point of the digit tube *******/
#define POINT_ON   1
#define POINT_OFF  0
/**************definitions for brightness***********************/
#define  BRIGHT_DARKEST 0
#define  BRIGHT_TYPICAL 2
#define  BRIGHTEST      7

uint8_t Cmd_SetData;
uint8_t Cmd_SetAddr;
uint8_t Cmd_DispCtrl;
bool _PointFlag;     //_PointFlag=1:the clock point on

uint64_t clkpin;
uint64_t datapin;

#define CLKLOW REG_PORT_OUT0 &= ~(clkpin);
#define CLKHI REG_PORT_OUT0 |= clkpin; // set

#define DI_HIGH REG_PORT_OUT0 |= datapin; // set
#define DI_LOW REG_PORT_OUT0 &= ~(datapin);

void tm1637_init(uint64_t dio, uint64_t clk);        //To clear the display
void tm1637_writeByte(uint8_t wr_data);//write 8bit data to tm1637
void tm1637_start(void);//send start bits
void tm1637_stop(void); //send stop bits
void tm1637_display(uint8_t dispData[]);
void tm1637_displayByte(uint8_t bitAddr,uint8_t dispData);
void tm1637_clearDisplay(void);
void tm1637_set(uint8_t brightness, uint8_t setData,uint8_t setAddr);//To take effect the next time it displays.
void tm1637_point(bool pointFlag);//whether to light the clock point ":".To take effect the next time it displays.
void tm1637_coding(uint8_t dispData[]);
uint8_t tm1637_codingByte(uint8_t dispData);
void tm1637_delay(void);



#endif /* TM1637_H_ */