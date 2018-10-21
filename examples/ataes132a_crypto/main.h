/*
 * main.h
 *
 * Created: 10/20/2018 8:15:04 AM
 *  Author: pvallone
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <asf.h>
#include "sercom_usart_config.h"
#include "aes132_impl.h"
#include "aes132_helper.h"

void print_commands(void);
void aes132app_command_dispatch(void);
uint8_t serial_data;

char const *megenta = "\x1B[35m";
char const *reset = "\x1B[0m";
char input;

#endif /* MAIN_H_ */