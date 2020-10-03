/*
 * tm1637.c
 *
 * Created: 9/23/2020 6:32:41 PM
 *  Author: pvallone - Ported from Author:Frankie.Chu
 */ 
#include "tm1637.h"

static int8_t TubeTab[] = {	0x3f,0x06,0x5b,0x4f,
							0x66,0x6d,0x7d,0x07,
							0x7f,0x6f,0x77,0x7c,
							0x39,0x5e,0x79,0x71};//0~9,A,b,C,d,E,F
//							PA02		PA03
void tm1637_init(uint64_t dio, uint64_t clk){
	
	clkpin = clk;
	datapin = dio;
	// port A
	REG_PORT_DIRSET0 = clkpin | datapin ;
	REG_PORT_OUTSET0 = clkpin | datapin ;
	REG_PORT_OUTCLR0 = clkpin | datapin ;
	tm1637_set(BRIGHT_TYPICAL,0x40, 0xc0);
	tm1637_clearDisplay();
	
}

void tm1637_writeByte(uint8_t wr_data){
	uint8_t i;
	for(i=0;i<8;i++)        //sent 8bit data
	{
		CLKLOW;
		if(wr_data & 0x01) {
			DI_HIGH;
		} else {
			DI_LOW;
		}	
		wr_data >>= 1;//LSB first
		CLKHI;
		
	}
	CLKLOW; //wait for the ACK
	DI_HIGH;
	CLKHI;
	PORT->Group[0].PINCFG[2].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	tm1637_delay();
	uint8_t ack = port_pin_get_input_level(PIN_PA02);	
	if(ack == 0)
	{
		
		REG_PORT_DIRSET0 = datapin ;
		REG_PORT_OUTSET0 = datapin ; 
		REG_PORT_OUTCLR0 = datapin ;
		DI_LOW;
		
		PORT->Group[0].PINCFG[2].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;		
	}
	
	tm1637_delay();
	REG_PORT_DIRSET0 = datapin ;
	REG_PORT_OUTSET0 = datapin ;
	REG_PORT_OUTCLR0 = datapin ;
	tm1637_delay();
}

void tm1637_start(void){
	CLKHI;
	DI_HIGH;
	DI_LOW;
	CLKLOW;
}

void tm1637_stop(void){
	
	CLKLOW;
	DI_LOW;
	CLKHI;
	DI_HIGH;
}

void tm1637_display(uint8_t dispData[]){
	
	uint8_t segData[4];
	uint8_t i;
	for(i = 0;i < 4;i ++)
	{
		segData[i] = dispData[i];
	}
	tm1637_coding(segData);
	tm1637_start();          //start signal sent to TM1637 from MCU
	tm1637_writeByte(ADDR_AUTO);//
	tm1637_stop();           //
	tm1637_start();          //
	tm1637_writeByte(Cmd_SetAddr);//
	for(i=0;i < 4;i ++)
	{
		tm1637_writeByte(segData[i]);        //
	}
	tm1637_stop();           //
	tm1637_start();          //
	tm1637_writeByte(Cmd_DispCtrl);//
	tm1637_stop();           //
}

void tm1637_displayByte(uint8_t bitAddr,uint8_t dispData){
	
	 int8_t SegData;
	 SegData = tm1637_codingByte(dispData);
	 tm1637_start();          //start signal sent to TM1637 from MCU
	 tm1637_writeByte(ADDR_FIXED);//
	 tm1637_stop();           //
	 tm1637_start();          //
	 tm1637_writeByte(bitAddr|0xc0);//
	 tm1637_writeByte(SegData);//
	 tm1637_stop();            //
	 tm1637_start();          //
	 tm1637_writeByte(Cmd_DispCtrl);//
	 tm1637_stop();           //
	
}

void tm1637_coding(uint8_t dispData[]){
	uint8_t PointData;
	if(_PointFlag == POINT_ON){
		PointData = 0x80;
	} else {			 
		PointData = 0; 
	}	 
	for(uint8_t i = 0;i < 4;i ++)
	{
		if(dispData[i] == 0x7f){
			 dispData[i] = 0x00;
		}else {				 
			  dispData[i] = TubeTab[dispData[i]] + PointData;
		}
		 
	}
}
uint8_t tm1637_codingByte(uint8_t dispData){
	uint8_t PointData;
	if(_PointFlag == POINT_ON){
		PointData = 0x80;
	} else {			
		PointData = 0;
	}
	
	if(dispData == 0x7f){
		dispData = 0x00 + PointData;//The bit digital tube off
	} else {
		dispData = TubeTab[dispData] + PointData;
	}	
	return dispData;
}

void tm1637_point(bool pointFlag){
	_PointFlag = pointFlag;	
}

void tm1637_set(uint8_t brightness, uint8_t setData, uint8_t setAddr){
	 Cmd_SetData = setData;
	 Cmd_SetAddr = setAddr;
	 Cmd_DispCtrl = 0x88 + brightness;//Set the brightness and it takes effect the next time it displays.

	
}
void tm1637_clearDisplay(void)
{
	tm1637_displayByte(0x00,0x7f);
	tm1637_displayByte(0x01,0x7f);
	tm1637_displayByte(0x02,0x7f);
	tm1637_displayByte(0x03,0x7f);
}

void tm1637_delay(void){
	delay_ms(50); // set a smaller delay as time is taken up by the cd74hc4067
}