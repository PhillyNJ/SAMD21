/*
 * conf_usart.h
 *
 * Created: 2/12/2017 8:03:59 AM
 *  Author: pvallone
 */ 

#ifndef CONF_USART_H_
#define CONF_USART_H_

#include <asf.h>

#define CONF_STDIO_USART_MODULE  EDBG_CDC_MODULE

struct usart_module usart_instance;

void configure_console(uint32_t baud);
void serialRead(uint16_t *const buffer);

#endif /* CONF_USART_H_ */