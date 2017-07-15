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
 
void ds1302_set_compile_time(uint8_t dow) {
	
	char s_month[5];
	int month, day, year, hour, minute;
	
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
	sscanf(__TIME__, "%d:%d", &hour, &minute);

	month = (strstr(month_names, s_month)-month_names)/3;
	
	  // set time;
	ds1302_write_register(REG_HOUR, ds1302_encode(hour));
	ds1302_write_register(REG_MIN, ds1302_encode(minute));
	ds1302_write_register(REG_SEC, ds1302_encode(0));
	 	 
	year -= 2000;
	ds1302_write_register(REG_YEAR, ds1302_encode(year));
	ds1302_write_register(REG_MON, ds1302_encode(month + 1));
	ds1302_write_register(REG_DAY, ds1302_encode(day));

	ds1302_write_register(REG_DOW, dow);
 }


void ds1302_set_time(uint8_t hour, uint8_t min, uint8_t sec)
 {
	 if (((hour>=0) && (hour<24)) && ((min>=0) && (min<60)) && ((sec>=0) && (sec<60)))
	 {
		 ds1302_write_register(REG_HOUR, ds1302_encode(hour));
		 ds1302_write_register(REG_MIN, ds1302_encode(min));
		 ds1302_write_register(REG_SEC, ds1302_encode(sec));
	 }
 }

 void ds1302_set_date(uint8_t day, uint8_t mon, uint16_t year)
 {
	 if (((day>0) && (day<=31)) && ((mon>0) && (mon<=12)) && ((year>=2000) && (year<3000)))
	 {
		 year -= 2000;
		 ds1302_write_register(REG_YEAR, ds1302_encode(year));
		 ds1302_write_register(REG_MON, ds1302_encode(mon));
		 ds1302_write_register(REG_DAY, ds1302_encode(day));
	 }
 }
void ds1302_set_dow(uint8_t dow)
{
	if ((dow>0) && (dow<8))
	ds1302_write_register(REG_DOW, dow);
}

void ds1302_get_time_str(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_get_time(&t);
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

void ds1302_get_date_str(uint8_t slformat, uint8_t eformat, char divider, char *output)
{	
	int yr, offset;
	ds_time_t t;
	ds1302_get_time(&t);
	switch (eformat)
	{
		case FORMAT_LITTLEENDIAN:
		if (t.day<10)
		output[0]=48;
		else
		output[0]=(char)((t.day / 10)+48);
		output[1]=(char)((t.day % 10)+48);
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
		if (t.day<10)
		output[6+offset]=48;
		else
		output[6+offset]=(char)((t.day / 10)+48);
		output[7+offset]=(char)((t.day % 10)+48);
		output[8+offset]=0;
		break;
		case FORMAT_MIDDLEENDIAN:
		if (t.mon<10)
		output[0]=48;
		else
		output[0]=(char)((t.mon / 10)+48);
		output[1]=(char)((t.mon % 10)+48);
		output[2]=divider;
		if (t.day<10)
		output[3]=48;
		else
		output[3]=(char)((t.day / 10)+48);
		output[4]=(char)((t.day % 10)+48);
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

void ds1302_get_dow_str(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_get_time(&t);
		
	
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

void ds1302_get_complete_time(ds_time_t *t){
	
	ds1302_get_time(t);
	ds1302_set_month_str(t);
	ds1302_set_day_of_week_str(t);
}

void ds1302_set_month_str(ds_time_t *t){

	switch (t->mon)
	{
		case 1:
		strncpy(t->monl, janurary, sizeof(janurary));
		break;
		case 2:
		strncpy(t->monl, february, sizeof(february));
		break;
		case 3:
		strncpy(t->monl, march, sizeof(march));
		break;
		case 4:
		strncpy(t->monl, april, sizeof(april));
		break;
		case 5:
		strncpy(t->monl, may, sizeof(may));
		break;
		case 6:
		strncpy(t->monl, june, sizeof(june));
		break;
		case 7:
		strncpy(t->monl, july, sizeof(july));
		break;
		case 8:
		strncpy(t->monl, august, sizeof(august));
		break;
		case 9:
		strncpy(t->monl, september, sizeof(september));
		break;
		case 10:
		strncpy(t->monl, october, sizeof(october));
		break;
		case 11:
		strncpy(t->monl, november, sizeof(november));
		break;
		case 12:
		strncpy(t->monl, december, sizeof(december));
		break;
	}
	
		strncpy(t->mons, t->monl,3);
	
}

void ds1302_set_day_of_week_str(ds_time_t *t){
	switch (t->dow)
	{
		case MONDAY:
		strncpy(t->dowl, monday, sizeof(monday));
		break;
		case TUESDAY:
		strncpy(t->dowl, tuesday, sizeof(tuesday));
		break;
		case WEDNESDAY:
		strncpy(t->dowl, wednesday, sizeof(wednesday));
		break;
		case THURSDAY:
		strncpy(t->dowl,thursday, sizeof(thursday));
		break;
		case FRIDAY:
		strncpy(t->dowl, friday, sizeof(friday));
		break;
		case SATURDAY:
		strncpy(t->dowl, saturday, sizeof(saturday));
		break;
		case SUNDAY:
		strncpy(t->dowl, sunday, sizeof(sunday));
		break;
	}
	strncpy(t->dows, t->dowl,3);

}

void ds1302_get_month_str(uint8_t format, char *output)
{
	
	ds_time_t t;
	ds1302_get_time(&t);

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
	uint8_t _reg = ds1302_read_register(REG_SEC);
	_reg &= ~(1 << 7);
	_reg |= (enable << 7);
	ds1302_write_register(REG_SEC, _reg);

}

void ds1302_write_protect(bool enable)
{
	uint8_t _reg = (enable << 7);
	ds1302_write_register(REG_WP, _reg);
}

void ds1302_set_tcr(uint8_t value)
{
	ds1302_write_register(REG_TCR, value);
}

void ds1302_get_time(ds_time_t *t){
	
	ds1302_burst_read();		

	t->sec	= ds1302_decode(_burstArray[0]);
	t->min	= ds1302_decode(_burstArray[1]);
	t->hour	= ds1302_decode_h(_burstArray[2]);
	t->day	= ds1302_decode(_burstArray[3]);
	t->mon	= ds1302_decode(_burstArray[4]);
	t->dow	= _burstArray[5];
	t->year	= ds1302_decode_y(_burstArray[6])+2000;
	
 }
 
uint8_t ds1302_read_byte()
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

uint8_t ds1302_read_register(uint8_t reg)
 {
	 uint8_t cmdByte = 129;
	 cmdByte |= (reg << 1);

	 uint8_t readValue;

	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_write_byte(cmdByte);	
	 readValue = ds1302_read_byte();
	 
	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
	
	 return readValue;
 }

void ds1302_write_register(uint8_t reg, uint8_t value)
 {
	 uint8_t cmdByte = (128 | (reg << 1));

	  REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	  REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_write_byte(cmdByte);
	 ds1302_write_byte(value);

	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
 }

 void ds1302_write_byte(uint8_t value)
 {
  
	 ds1302_set_data_port_direction(OUTPUT);
	 ds1302_shift_out(LSBFIRST, value);
 }

 void ds1302_shift_out(uint8_t bitOrder, uint8_t val)
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

void ds1302_burst_read()
 {
	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_write_byte(191);
	 for (int i=0; i<8; i++)
	 {
		 _burstArray[i] = ds1302_read_byte();		
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
 
 uint8_t ds1302_decode_h(uint8_t value)
 {
	 if (value & 128)
	 value = (value & 15) + (12 * ((value & 32) >> 5));
	 else
	 value = (value & 15) + (10 * ((value & 48) >> 4));
	 return value;
 }

 uint8_t ds1302_decode_y(uint8_t value)
 {
	 uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	 return decoded;
 }

 uint8_t ds1302_encode(uint8_t value)
 {
	 uint8_t encoded = ((value / 10) << 4) + (value % 10);
	 return encoded;
 }

 void ds1302_write_buffer(ds1302_ram_t *r)
 {
	  REG_PORT_OUTCLR1 = SCLK;	//   digitalWrite(_sclk_pin, LOW);
	  REG_PORT_OUTSET1 = CE;	// digitalWrite(_ce_pin, HIGH);

	 ds1302_write_byte(254);
	 for (int i=0; i<31; i++)
	 {
		 ds1302_write_byte(r->cell[i]);
	 }
	 REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);
 }

void ds1302_read_buffer(ds1302_ram_t *r)
 { 

	 REG_PORT_OUTCLR1 = SCLK;//   digitalWrite(_sclk_pin, LOW);
	 REG_PORT_OUTSET1 = CE;// digitalWrite(_ce_pin, HIGH);

	 ds1302_write_byte(255);
	 for (int i=0; i<31; i++)
	 {
		 r->cell[i] = ds1302_read_byte();
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

		 ds1302_write_byte(addr);
		 ds1302_write_byte(value);

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

		 ds1302_write_byte(addr);
		 readValue = ds1302_read_byte();
		 
		  REG_PORT_OUTCLR1 = CE;// digitalWrite(_ce_pin, LOW);

		 return readValue;
	 }
	 else
	 return 0;
 }
