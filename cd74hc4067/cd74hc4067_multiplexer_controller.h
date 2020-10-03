/*
 * cd74hc4067_multiplexer_controller.h
 *
 * Created: 9/29/2020 7:27:37 AM
 *  Author: pvallone
 */ 


#ifndef CD74HC4067_MULTIPLEXER_CONTROLLER_H_
#define CD74HC4067_MULTIPLEXER_CONTROLLER_H_
#include <asf.h>

#define CD74HC4067_DEVICES 3




struct cd74hc4067_config {
	
	uint8_t sig_pin;	
	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;		

	uint64_t en_devices[CD74HC4067_DEVICES];
	
};

struct cd74hc4067_config *config_ptr;

void cd74hc4067_init(struct cd74hc4067_config *const config);
void cd74hc4067_enable(uint8_t val, uint8_t index);
uint8_t cd74hc4067_read_channel(uint8_t channel,  uint8_t index);
void cd74hc4067_set_channel(uint8_t channel, uint8_t index);
void cd74hc4067_read_bus_channel(uint8_t index, uint8_t *data);
void cd74hc4067_set_bus_channel(uint8_t index, uint8_t *data);
#endif /* CD74HC4067_MULTIPLEXER_CONTROLLER_H_ */