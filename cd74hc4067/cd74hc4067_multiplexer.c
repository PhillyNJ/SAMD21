/*
 * cd74hc4067_multiplexer.c
 *
 * Created: 9/11/2020 7:36:27 AM
 *  Author: pvallone
 */ 
#include "cd74hc4067_multiplexer.h"

void cd74hc4067_init(uint64_t en, uint8_t sig_pin, uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3, uint64_t sig_port){
	
	// set up output pins first 
	MP_DIRSET_PORT =  en | s0 | s1 | s2 | s3;
	MP_PORT_OUTSET =  en | s0 | s1 | s2 | s3;
	MP_PORT_OUTCLR =  en | s0 | s1 | s2 | s3;
	
	PORT->Group[1].PINCFG[7].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	mp_enable = en;
	mp_s0 = s0;
	mp_s1 = s1;
	mp_s2 = s2;
	mp_s3 = s3;
	mp_sig = sig_pin;
	mp_sig_port = sig_port;
	cd74hc4067_enable(HIGH);// turn off 	
}

// Set EN pin to LOW to enable
void cd74hc4067_enable(uint8_t val){
	if(val == 0){
		MP_OUT &= ~(mp_enable);		
	} else {		
		MP_OUT |= mp_enable; // set
	}	
}

uint8_t cd74hc4067_read_channel(uint8_t channel){
	
	cd74hc4067_enable(LOW);
	delay_ms(10);
	uint8_t b0 = (((channel) >> (0)) & 0x01);
	uint8_t b1 = (((channel) >> (1)) & 0x01);
	uint8_t b2 = (((channel) >> (2)) & 0x01);
	uint8_t b3 = (((channel) >> (3)) & 0x01);
			
	if(b0 == 0){
		MP_OUT &= ~(mp_s0);
	} else {
		MP_OUT |= mp_s0; // set
	}
	if(b1 == 0){
		MP_OUT &= ~(mp_s1);
	} else {
		MP_OUT |= mp_s1; // set
	}
	if(b2 == 0){
		MP_OUT &= ~(mp_s2);
	} else {
		MP_OUT |= mp_s2; // set
	}
	if(b3 == 0){
		MP_OUT &= ~(mp_s3);
	} else {
		MP_OUT |= mp_s3; // set
	}	
	
	uint8_t val = 0;
	if (port_pin_get_input_level(mp_sig) == false) {
		val = 0;
	} else {
		val = 1;
	}
	
	
	cd74hc4067_enable(HIGH);	
	//delay_ms(100);
	return val;
}

void cd74hc4067_set_channel(uint8_t channel){
	
	cd74hc4067_enable(LOW);
	delay_ms(1);
	uint8_t b0 = (((channel) >> (0)) & 0x01);
	uint8_t b1 = (((channel) >> (1)) & 0x01);
	uint8_t b2 = (((channel) >> (2)) & 0x01);
	uint8_t b3 = (((channel) >> (3)) & 0x01);
	
	if(b0 == 0){
		MP_OUT &= ~(mp_s0);
	} else {
		MP_OUT |= mp_s0; // set
	}
	
	if(b1 == 0){
		MP_OUT &= ~(mp_s1);
	} else {
		MP_OUT |= mp_s1; // set
	}
	
	if(b2 == 0){
		MP_OUT &= ~(mp_s2);
	} else {
		MP_OUT |= mp_s2; // set
	}
	
	if(b3 == 0){
		MP_OUT &= ~(mp_s3);
	} else {
		MP_OUT |= mp_s3; // set
	}	
	
	cd74hc4067_enable(HIGH);

	
}
