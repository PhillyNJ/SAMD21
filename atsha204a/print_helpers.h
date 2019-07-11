/*
 * print_helpers.h
 *
 * Created: 5/25/2019 8:07:20 AM
 *  Author: pvallone
 */ 


#ifndef PRINT_HELPERS_H_
#define PRINT_HELPERS_H_
#include <asf.h>
#include "string.h"

#define RET_ERR(status, message) { 	if (status != ATCA_SUCCESS)	{printf("- Error - " message ":");  sha204_parser_rc(status); return; }}
#define CHECK_STATUS_MESS(status, message) { if (status != ATCA_SUCCESS)	{printf("- Error - " message ":");  sha204_parser_rc(status); return status;} }
#define RET_STATUS(status) { if (status != ATCA_SUCCESS)	{ sha204_parser_rc(status); return status;} }
	
void print_buffer(uint8_t *buff, uint8_t size);
void print_raw_buffer(uint8_t *buff, uint8_t size);

#endif /* PRINT_HELPERS_H_ */