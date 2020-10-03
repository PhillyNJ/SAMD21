/*
 * cd74hc4067_multiplexer_controller.c
 *
 * Created: 9/29/2020 7:27:46 AM
 *  Author: pvallone
 */ 
#include "cd74hc4067_multiplexer_controller.h"

void cd74hc4067_init(struct cd74hc4067_config *const config){
	
	REG_PORT_DIRSET1 =	config->s0 | config->s1 | config->s2 | config->s3;
	REG_PORT_OUTSET1 =	config->s0 | config->s1 | config->s2 | config->s3;
	REG_PORT_OUTCLR1 =  config->s0 | config->s1 | config->s2 | config->s3;
	
	// set the enable pins directions to output
	for(uint8_t i = 0; i < CD74HC4067_DEVICES; i++){		
		if(config->en_devices[i] > 0){
			REG_PORT_DIRSET1 = config->en_devices[i];
			REG_PORT_OUTSET1 = config->en_devices[i];
			REG_PORT_OUTCLR1 = config->en_devices[i];				
		}
	}				
	
	uint8_t pin = config->sig_pin - 32;	
	
	PortGroup *const port_base = port_get_group_from_gpio_pin(config->sig_pin);	
	port_base->PINCFG[pin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	
	config_ptr = config;
	// disable of the devices
	for(uint8_t i = 0; i < CD74HC4067_DEVICES; i++){
		cd74hc4067_enable(HIGH, i);	
	}
	
}

// Set EN pin to LOW to enable
void cd74hc4067_enable(uint8_t val, uint8_t index){	
	
	if(val == 0){
		REG_PORT_OUT1 &= ~(config_ptr->en_devices[index]);
	} else {
		REG_PORT_OUT1 |= config_ptr->en_devices[index]; // set
	}
}

uint8_t cd74hc4067_read_channel(uint8_t channel,  uint8_t index){
	
	cd74hc4067_enable(LOW, index);
	//delay_ms(10);
	uint8_t b0 = (((channel) >> (0)) & 0x01);
	uint8_t b1 = (((channel) >> (1)) & 0x01);
	uint8_t b2 = (((channel) >> (2)) & 0x01);
	uint8_t b3 = (((channel) >> (3)) & 0x01);
	
	if(b0 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s0);
	} else {
		REG_PORT_OUT1 |= config_ptr->s0; // set
	}
	if(b1 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s1);
	} else {
		REG_PORT_OUT1 |= config_ptr->s1; // set
	}
	if(b2 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s2);
	} else {
		REG_PORT_OUT1 |= config_ptr->s2; // set
	}
	if(b3 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s3);
	} else {
		REG_PORT_OUT1 |= config_ptr->s3; // set
	}
	
	uint8_t val = 0;
	if (port_pin_get_input_level(config_ptr->sig_pin) == false) {
		val = 0;
	
	} else {
		val = 1;		
	}	
	
	cd74hc4067_enable(HIGH, index);
	//delay_ms(100);
	return val;
}


void cd74hc4067_read_bus_channel(uint8_t index, uint8_t *data){	
	for(uint8_t i = 0; i < 16; i++){		
		data[i] = cd74hc4067_read_channel(i, index);				
	}
}
void cd74hc4067_set_bus_channel(uint8_t index, uint8_t *data){
	for(uint8_t i = 0; i < 16; i++){		
		cd74hc4067_set_channel(i, index);
	}
}

void cd74hc4067_set_channel(uint8_t channel, uint8_t index){
	
	cd74hc4067_enable(LOW, index);
	delay_ms(1);
	uint8_t b0 = (((channel) >> (0)) & 0x01);
	uint8_t b1 = (((channel) >> (1)) & 0x01);
	uint8_t b2 = (((channel) >> (2)) & 0x01);
	uint8_t b3 = (((channel) >> (3)) & 0x01);
	
	if(b0 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s0);
	} else {
		REG_PORT_OUT1 |= config_ptr->s0; // set
	}
	
	if(b1 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s1);
	} else {
		REG_PORT_OUT1 |= config_ptr->s1; // set
	}
	
	if(b2 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s2);
	} else {
		REG_PORT_OUT1 |= config_ptr->s2; // set
	}
	
	if(b3 == 0){
		REG_PORT_OUT1 &= ~(config_ptr->s3);
	} else {
		REG_PORT_OUT1 |= config_ptr->s3; // set
	}
	
	cd74hc4067_enable(HIGH, index);
	
}
