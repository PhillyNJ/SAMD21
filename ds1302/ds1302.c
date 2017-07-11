/*
 * ds1302.c
 *
 * Created: 7/7/2017 9:43:57 AM
 *  Author: pvallone
 */ 
 #include "ds1302.h"

 const char janurary[] = "January";
 const char february[] = "February";
 const char march[] = "March";
 const char april[] = "April";
 const char may[] = "May";
 const char june[] = "June";
 const char july[] = "July";
 const char august[] = "August";
 const char september[] = "September";
 const char october[] = "October";
 const char november[] = "November";
 const char december[] = "December";

 const char monday[] = "Monday";
 const char tuesday[]= "Tuesday";
 const char wednesday[]= "Wednesday";
 const char thursday[]= "Thursday";
 const char friday[]= "Friday";
 const char saturday[]= "Saturday";
 const char sunday[] = "Sunday";
 

 void ds1302_init(bool addDelay){
 	additionalDelay = addDelay;
	REG_PORT_DIRSET1 = CE | SCLK;
 }

 
void ds1302_setTime(uint8_t hour, uint8_t min, uint8_t sec)
 {
	 if (((hour>=0) && (hour<24)) && ((min>=0) && (min<60)) && ((sec>=0) && (sec<60)))
	 {
		 ds1302_writeRegister(REG_HOUR, ds1302_encode(hour));
		 ds1302_writeRegister(REG_MIN, ds1302_encode(min));
		 ds1302_writeRegister(REG_SEC, ds1302_encode(sec));
	 }
 }

 void ds1302_setDate(uint8_t date, uint8_t mon, uint16_t year)
 {
	 if (((date>0) && (date<=31)) && ((mon>0) && (mon<=12)) && ((year>=2000) && (year<3000)))
	 {
		 year -= 2000;
		 ds1302_writeRegister(REG_YEAR, ds1302_encode(year));
		 ds1302_writeRegister(REG_MON, ds1302_encode(mon));
		 ds1302_writeRegister(REG_DATE, ds1302_encode(date));
	 }
 }
void ds1302_setDOW(uint8_t dow)
{
	if ((dow>0) && (dow<8))
	ds1302_writeRegister(REG_DOW, dow);
}

void ds1302_getTimeStr(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_getTime(&t);
	if (t.hour<10)
	output[0]=48;
	else
	output[0]= (char)((t.hour / 10)+48);
	output[1]= (char)((t.hour % 10)+48);
	output[2]=58;
	if (t.min<10)
	output[3]=48;
	else
	output[3]= (char)(t.min / 10)+48;
	output[4]= (char)((t.min % 10)+48) ;
	output[5]=58;
	if (format==FORMAT_SHORT)
	output[5]=0;
	else
	{
		if (t.sec<10)
		output[6]=48;
		else
		output[6]=(char)((t.sec / 10)+48);
		output[7]=(char)((t.sec % 10)+48);
		output[8]=0;
	}
	
}

void ds1302_getDateStr(uint8_t slformat, uint8_t eformat, char divider, char *output)
{	
	int yr, offset;
	ds_time_t t;
	ds1302_getTime(&t);
	switch (eformat)
	{
		case FORMAT_LITTLEENDIAN:
		if (t.date<10)
		output[0]=48;
		else
		output[0]=(char)((t.date / 10)+48);
		output[1]=(char)((t.date % 10)+48);
		output[2]=divider;
		if (t.mon<10)
		output[3]=48;
		else
		output[3]=(char)((t.mon / 10)+48);
		output[4]=(char)((t.mon % 10)+48);
		output[5]=divider;
		if (slformat==FORMAT_SHORT)
		{
			yr=t.year-2000;
			if (yr<10)
			output[6]=48;
			else
			output[6]=(char)((yr / 10)+48);
			output[7]=(char)((yr % 10)+48);
			output[8]=0;
		}
		else
		{
			yr=t.year;
			output[6]=(char)((yr / 1000)+48);
			output[7]=(char)(((yr % 1000) / 100)+48);
			output[8]=(char)(((yr % 100) / 10)+48);
			output[9]=(char)((yr % 10)+48);
			output[10]=0;
		}
		break;
		case FORMAT_BIGENDIAN:
		if (slformat==FORMAT_SHORT)
		offset=0;
		else
		offset=2;
		if (slformat==FORMAT_SHORT)
		{
			yr=t.year-2000;
			if (yr<10)
			output[0]=48;
			else
			output[0]=(char)((yr / 10)+48);
			output[1]=(char)((yr % 10)+48);
			output[2]=divider;
		}
		else
		{
			yr=t.year;
			output[0]=(char)((yr / 1000)+48);
			output[1]=(char)(((yr % 1000) / 100)+48);
			output[2]=(char)(((yr % 100) / 10)+48);
			output[3]=(char)((yr % 10)+48);
			output[4]=divider;
		}
		if (t.mon<10)
		output[3+offset]=48;
		else
		output[3+offset]=(char)((t.mon / 10)+48);
		output[4+offset]=(char)((t.mon % 10)+48);
		output[5+offset]=divider;
		if (t.date<10)
		output[6+offset]=48;
		else
		output[6+offset]=(char)((t.date / 10)+48);
		output[7+offset]=(char)((t.date % 10)+48);
		output[8+offset]=0;
		break;
		case FORMAT_MIDDLEENDIAN:
		if (t.mon<10)
		output[0]=48;
		else
		output[0]=(char)((t.mon / 10)+48);
		output[1]=(char)((t.mon % 10)+48);
		output[2]=divider;
		if (t.date<10)
		output[3]=48;
		else
		output[3]=(char)((t.date / 10)+48);
		output[4]=(char)((t.date % 10)+48);
		output[5]=divider;
		if (slformat==FORMAT_SHORT)
		{
			yr=t.year-2000;
			if (yr<10)
			output[6]=48;
			else
			output[6]=(char)((yr / 10)+48);
			output[7]=(char)((yr % 10)+48);
			output[8]=0;
		}
		else
		{
			yr=t.year;
			output[6]=(char)((yr / 1000)+48);
			output[7]=(char)(((yr % 1000) / 100)+48);
			output[8]=(char)(((yr % 100) / 10)+48);
			output[9]=(char)((yr % 10)+48);
			output[10]=0;
		}
		break;
	}
	
}

void ds1302_getDOWStr(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_getTime(&t);
		
	
	switch (t.dow)
	{
		case MONDAY:		
		strncpy(output, monday, sizeof(monday));		
		break;
		case TUESDAY:			
		strncpy(output, tuesday, sizeof(tuesday));	
		break;
		case WEDNESDAY:
		strncpy(output, wednesday, sizeof(wednesday));			
		break;
		case THURSDAY:
		strncpy(output,thursday, sizeof(thursday));			
		break;
		case FRIDAY:
		strncpy(output, friday, sizeof(friday));
		break;
		case SATURDAY:
		strncpy(output, saturday, sizeof(saturday));		
		break;
		case SUNDAY:					
		strncpy(output, sunday, sizeof(sunday));
		break;
	}
	if (format==FORMAT_SHORT){
		output[3]=0;
	}
	
	
}

void ds1302_getMonthStr(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_getTime(&t.mon);

	switch (t.mon)
	{
		case 1:		
		strncpy(output, janurary, sizeof(janurary));	
		break;
		case 2:
		strncpy(output, february, sizeof(february));			
		break;
		case 3:
		strncpy(output, march, sizeof(march));		
		break;
		case 4:
		strncpy(output, april, sizeof(april));		
		break;
		case 5:
		strncpy(output, may, sizeof(may));		
		break;
		case 6:
		strncpy(output, june, sizeof(june));			
		break;
		case 7:
		strncpy(output, july, sizeof(july));			
		break;
		case 8:
		strncpy(output, august, sizeof(august));		
		break;
		case 9:
		strncpy(output, september, sizeof(september));		
		break;
		case 10:
		strncpy(output, october, sizeof(october));			
		break;
		case 11:
		strncpy(output, november, sizeof(november));		
		break;
		case 12:
		strncpy(output, december, sizeof(december));			
		break;
	}
	if (format==FORMAT_SHORT)
	output[3]=0;
	
}

void ds1302_halt(bool enable)
{
	uint8_t _reg = ds1302_readRegister(REG_SEC);
	_reg &= ~(1 << 7);
	_reg |= (enable << 7);
	ds1302_writeRegister(REG_SEC, _reg);

}

void ds1302_writeProtect(bool enable)
{
	uint8_t _reg = (enable << 7);
	ds1302_writeRegister(REG_WP, _reg);
}

void ds1302_setTCR(uint8_t value)
{
	ds1302_writeRegister(REG_TCR, value);
}



void ds1302_getTime(ds_time_t *t){
	
	ds1302_burstRead();		

	t->sec	= ds1302_decode(_burstArray[0]);
	t->min	= ds1302_decode(_burstArray[1]);
	t->hour	= ds1302_decodeH(_burstArray[2]);
	t->date	= ds1302_decode(_burstArray[3]);
	t->mon	= ds1302_decode(_burstArray[4]);
	t->dow	= _burstArray[5];
	t->year	= ds1302_decodeY(_burstArray[6])+2000;
	
 }
 

 
uint8_t ds1302_readByte()
{	 
	ds1302_set_data_port_direction(INPUT);
		
	uint8_t value = 0;
	uint8_t currentBit = 0;
	
	for (int i = 0; i < 8; ++i)
	{
		currentBit = (PORT->Group[1].IN.reg & DATA) > 0;			
		value |= (currentBit << i);					
		REG_PORT_OUTSET1 = SCLK; 		
		delay_us(10);			
		REG_PORT_OUTCLR1 = SCLK; 	
		
	}	
	if(additionalDelay){
		delay_us(10); 
	}
				
	return value;
}

uint8_t ds1302_readRegister(uint8_t reg)
 {
	 uint8_t cmdByte = 129;
	 cmdByte |= (reg << 1);

	 uint8_t readValue;

	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_writeByte(cmdByte);	
	 readValue = ds1302_readByte();
	 
	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
	
	 return readValue;
 }

void ds1302_writeRegister(uint8_t reg, uint8_t value)
 {
	 uint8_t cmdByte = (128 | (reg << 1));

	  REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	  REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_writeByte(cmdByte);
	 ds1302_writeByte(value);

	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
 }

 void ds1302_writeByte(uint8_t value)
 {
  
	 ds1302_set_data_port_direction(OUTPUT);
	 ds1302_shiftOut(LSBFIRST, value);
 }

 void ds1302_shiftOut(uint8_t bitOrder, uint8_t val)
 {
	 uint8_t i;

	 for (i = 0; i < 8; i++)  {
		
		if(bitOrder == LSBFIRST){

			if(!!(val & (1 << i))){
				REG_PORT_OUTSET1 = DATA;
			} else {
				REG_PORT_OUTCLR1 = DATA;
			}
			
		} else {	
			
			if(!!(val & (1 << (7 - i)))){
				REG_PORT_OUTSET1 = DATA;
			} else {
				REG_PORT_OUTCLR1 = DATA;
			}
		}		
		 
		 REG_PORT_OUTSET1 = SCLK;				
		 REG_PORT_OUTCLR1 = SCLK;		
	 }
 }

void ds1302_burstRead()
 {
	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_writeByte(191);
	 for (int i=0; i<8; i++)
	 {
		 _burstArray[i] = ds1302_readByte();		
	 }
	  REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
 }

void ds1302_set_data_port_direction(direction_t dir){
	
	if(dir == INPUT){

	  REG_PORT_OUTCLR1 = DATA;		 
	  PORT->Group[1].PINCFG[1].reg = PORT_PINCFG_INEN ;

	} else {
	  
		REG_PORT_DIRSET1 = DATA;
	}
	
 }

 uint8_t ds1302_decode(uint8_t value)
 {
	 uint8_t decoded = value & 127;
	 decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	 return decoded;
 }
 
 uint8_t ds1302_decodeH(uint8_t value)
 {
	 if (value & 128)
	 value = (value & 15) + (12 * ((value & 32) >> 5));
	 else
	 value = (value & 15) + (10 * ((value & 48) >> 4));
	 return value;
 }

 uint8_t ds1302_decodeY(uint8_t value)
 {
	 uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	 return decoded;
 }

 uint8_t ds1302_encode(uint8_t value)
 {
	 uint8_t encoded = ((value / 10) << 4) + (value % 10);
	 return encoded;
 }

 void ds1302_writeBuffer(ds1302_ram_t *r)
 {
	  REG_PORT_OUTCLR1 = SCLK;	//   digitalWrite(_sclk_pin, LOW);
	  REG_PORT_OUTSET1 = CE;	// digitalWrite(_ce_pin, HIGH);

	 ds1302_writeByte(254);
	 for (int i=0; i<31; i++)
	 {
		 ds1302_writeByte(r->cell[i]);
	 }
	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
 }

void ds1302_readBuffer(ds1302_ram_t *r)
 { 

	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_writeByte(255);
	 for (int i=0; i<31; i++)
	 {
		 r->cell[i] = ds1302_readByte();
	 }
	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
	
 }


void ds1302_poke(uint8_t addr, uint8_t value)
 {
	 if ((addr >=0) && (addr<=30))
	 {
		 addr = (addr * 2) + 192;

		 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
		 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

		 ds1302_writeByte(addr);
		 ds1302_writeByte(value);

		 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
	 }
 }

 uint8_t ds1302_peek(uint8_t addr)
 {
	 if ((addr >=0) && (addr<=30))
	 {
		 addr = (addr * 2) + 193;

		 uint8_t readValue;

		  REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
		  REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

		 ds1302_writeByte(addr);
		 readValue = ds1302_readByte();
		 
		  REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);

		 return readValue;
	 }
	 else
	 return 0;
 }
