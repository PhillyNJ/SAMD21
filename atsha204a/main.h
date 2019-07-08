/*
 * main.h
 *
 * Created: 5/25/2019 7:54:04 AM
 *  Author: pvallone
 */ 


#ifndef MAIN_H_
#define MAIN_H_



uint8_t serialNumber[9];

char const *megenta = "\x1B[35m";
char const *reset = "\x1B[0m";
const char *otp_data_1 = "53cr3t5Data For The OTP Zone";
const char *otp_data_2 = "More data The OTP Zone";
uint8_t serial_data;
uint8_t input;
uint8_t slot;
uint8_t host;
uint8_t client;
uint8_t mode;
uint8_t block;
uint8_t offset;
uint8_t write_data[ATCA_KEY_SIZE];
uint8_t parent_key[ATCA_KEY_SIZE];
uint8_t random_data[OUTNONCE_SIZE];
uint8_t digest[ATCA_SHA_DIGEST_SIZE];
uint8_t sw_digest[ATCA_SHA_DIGEST_SIZE];
uint8_t otp_data[32];
uint8_t derive_key_mac[39];
uint8_t request[88]  = {0};
uint8_t message[] = "This Message to be hashed - PhillyNJ";
uint8_t num_in[20] = {0};
uint8_t padding = 0x77;	
uint8_t read_data[ATCA_KEY_SIZE];
uint8_t calculated_key[ATCA_KEY_SIZE];
atca_nonce_in_out_t nonce_params;
uint8_t rand_out[ATCA_KEY_SIZE];
uint8_t temp_key_in[55] = {0};
uint8_t temp_key_out[32]= {0};
uint16_t address;
struct atca_temp_key temp_key;
void sha204_app_command_dispatch(void);
void sha204_print_menu(void);
#endif /* MAIN_H_ */