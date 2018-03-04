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
void relay_data(void);

#endif /* MAIN_H_ */