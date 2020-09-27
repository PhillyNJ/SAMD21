/*
 * cd74hc4067_multiplexer.h
 *
 * Created: 9/11/2020 7:36:17 AM
 *  Author: pvallone
 */ 


#ifndef CD74HC4067_MULTIPLEXER_H_
#define CD74HC4067_MULTIPLEXER_H_

#include <asf.h>

uint64_t mp_enable, mp_s0, mp_s1, mp_s2, mp_s3, mp_sig_port;
uint8_t mp_sig;
uint8_t mp_channels[16];

#define MP_DIRSET_PORT REG_PORT_DIRSET1
#define MP_PORT_OUTSET REG_PORT_OUTSET1
#define MP_PORT_OUTCLR REG_PORT_OUTCLR1

#define MP_OUT REG_PORT_OUT1
#define MP_PORT 1 // 0= PORTA, 1 = PORTB


void cd74hc4067_init(uint64_t en, uint8_t sig_pin, uint64_t s0, uint64_t s1, uint64_t s2, uint64_t s3, uint64_t sig_port);
void cd74hc4067_enable(uint8_t val);
uint8_t cd74hc4067_read_channel(uint8_t channel);
void cd74hc4067_set_channel(uint8_t channel);
#endif /* CD74HC4067_MULTIPLEXER_H_ */