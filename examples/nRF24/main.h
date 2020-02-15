/*
 * main.h
 *
 * Created: 12/19/2016 6:21:48 AM
 *  Author: pvallone
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <asf.h>
#include "sercom_usart_config.h"
#include "nRF24L01.h"
#include "nrf24.h"

void send_data(void);
void receive_data(void);
uint8_t	getAckPayLoad_cb(void * buf, uint8_t len);
enum mode_t {
	auth = 0, 
	data = 1
}; 

typedef struct  {
	enum mode_t mode;
	uint8_t sn[9];
	uint8_t data[22];

} packet_t;

packet_t packet;

#endif /* MAIN_H_ */