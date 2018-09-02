/*
 * ataes132a_i2c.h
 *
 * Created: 6/18/2018 6:35:16 AM
 *  Author: pvallone
 */ 


#ifndef ATAES132A_I2C_H_
#define ATAES132A_I2C_H_

#include "string.h"

uint8_t aes_read_buffer[64];
uint8_t aes_write_buffer[64];

typedef enum ataes132a_event_status_t {
	
	SUCCESS = 0x00,
	BOUNDRY_ERROR = 0x02,
	RW_CONFING = 0x04,
	BAD_ADDRESS = 0x08,
	COUNT_ERROR = 0x10,
	NONCE_ERROR = 0x20,
	MAC_ERROR = 0x40,
	PARSE_ERROR = 0x50,
	DARA_MATCH = 0x60,
	LOCK_ERROR = 0x70,
	KEY_ERROR = 0x80,
	BAD_CALL = 0xFF
} ataes132a_event_status_t;


typedef struct {
	
	uint16_t write_command;
	uint8_t	length; // length of data not including mode, param1 & param2
	uint8_t opCode;
	uint8_t mode;
	uint16_t param1;
	uint16_t param2;
	uint8_t * data_buffer;
	uint8_t size_of_data_buffer;
	uint16_t checkSum[2];
	uint8_t rx_buffer_length;
} ataes_tranaction_t;

// page 34 of ATAAES132A datasheet
typedef struct {
	
	ataes132a_event_status_t aes_transaction_status;
	bool device_ready; // WIP
	bool i2c_mode; // WEN
	bool sleep_standby_mode; // WAKEb
	bool checksum_error; // CRCE
	bool memory_buffer_changed; // RRDY
	bool error; // EERR,
	uint8_t *rx_buffer; // pointer to the return buffer
	uint8_t return_count;

} ataes_transaction_status_t;

void aes_write_command(ataes_tranaction_t * packet, ataes_transaction_status_t * status);

struct i2c_master_packet aes_rd_packet;
struct i2c_master_packet aes_w_packet;
struct i2c_master_module aes_i2c_master_instance;
void aes_configure_i2c(void);
void aes_configure_i2c_callbacks(void);
void aes_i2c_write_complete_callback(struct i2c_master_module *const module);
void aes_calculate_checksum(uint8_t length, uint8_t *data, uint8_t *crc);
void aes_print_status(ataes_transaction_status_t * status);
void aes_read_status(ataes_transaction_status_t *status);
void aes_parse_status(ataes_transaction_status_t * status);
int aes_memory_write(uint16_t address, uint8_t *data, uint8_t length, ataes_transaction_status_t *status);
int aes_key_mem__write(uint16_t address, uint8_t *data, uint8_t length, ataes_transaction_status_t *status);
void aes_memory_read(uint16_t address, uint8_t *out_data, uint8_t length);
void aes_block_read(ataes_tranaction_t * packet, ataes_transaction_status_t * status);
const char * aes_get_transaction_status(ataes132a_event_status_t * status);
#endif /* ATAES132A_I2C_H_ */