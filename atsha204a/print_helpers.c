/*
 * print_helpers.c
 *
 * Created: 5/25/2019 8:07:32 AM
 *  Author: pvallone
 */ 
#include "print_helpers.h"

void print_buffer(uint8_t *buff, uint8_t size){
	 for(uint8_t i = 0; i < size;i++){
		 printf("0x%02X ", buff[i]);
	 }
	 printf("\n\r");
}

void print_raw_buffer(uint8_t *buff, uint8_t size){
	for(uint8_t i = 0; i < size;i++){
		printf("%02X ", buff[i]);
	}
	printf("\n\r");
}
