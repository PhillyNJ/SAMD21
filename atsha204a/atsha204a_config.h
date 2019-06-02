/*
 * atsha204a_config.h
 *
 * Created: 5/25/2019 8:11:23 AM
 *  Author: pvallone
 */ 


#ifndef ATSHA204A_CONFIG_H_
#define ATSHA204A_CONFIG_H_

#include <asf.h>



#define SLOT00 0x00 // SLOT Address = 0 (0x00)
#define SLOT01 0x08// SLOT Address = 8 (0x08)
#define SLOT02 0x10// SLOT Address = 16 (0x10)
#define SLOT03 0x18// SLOT Address = 24 (0x18)
#define SLOT04 0x20// SLOT Address = 32 (0x20)
#define SLOT05 0x28// SLOT Address = 40 (0x28)
#define SLOT06 0x30// SLOT Address = 48 (0x30)
#define SLOT07 0x38// SLOT Address = 56 (0x38)
#define SLOT08 0x40// SLOT Address = 64 (0x40)
#define SLOT09 0x48// SLOT Address = 72 (0x48)
#define SLOT10 0x50// SLOT Address = 80 (0x50)
#define SLOT11 0x58// SLOT Address = 88 (0x58)
#define SLOT12 0x60// SLOT Address = 96 (0x60)
#define SLOT13 0x68// SLOT Address = 104 (0x68)
#define SLOT14 0x70// SLOT Address = 112 (0x70)
#define SLOT15 0x78// SLOT Address = 120 (0x78)

// SlotConfig Byte #0
#define SHA204_CHECK_ONLY		              (1 << 4)
#define SHA204_LIMIT_USE		              (1 << 5)
#define SHA204_ENCRYPT_READ		              (1 << 6)
#define SHA204_IS_SECRET				      (1 << 7)

struct sha204_details {
	uint8_t serial_number[9];
	uint8_t rev_num[4];
	uint8_t lock_value;
	uint8_t lock_config;
	uint8_t i2c_address;
	uint8_t chip_config;
	uint8_t check_mac_config;
	uint8_t otp_mode;
	uint8_t select_mode;
	uint8_t small_zone[4];
	
};
struct sha204_details myDetails;



#endif /* ATSHA204A_CONFIG_H_ */