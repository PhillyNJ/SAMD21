/*
 * main.h
 *
 * Created: 9/15/2018 7:00:03 AM
 *  Author: pvallone
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <asf.h>
#include "sysTimer.h"
#include "aes132_helper.h"
#include "aes132_impl.h"
#include "sercom_usart_config.h"
void print_commands(void);
void aes132app_command_dispatch(void);
uint8_t serial_data;

char const *megenta = "\x1B[35m";
char const *reset = "\x1B[0m";


static SYS_Timer_t myTimer;
void appTimerTestHandler(SYS_Timer_t *timer);
void initMyTimer(void);
void startMyTimer(void);
void stopMyTimer(void);

#endif /* MAIN_H_ */