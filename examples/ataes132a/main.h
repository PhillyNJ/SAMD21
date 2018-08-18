/*
 * main.h
 *
 * Created: 8/13/2018 6:28:55 AM
 *  Author: pvallone
 */ 


#ifndef MAIN_H_
#define MAIN_H_

//#define LTC_ECB_MODE
//#define LTC_CTR_MODE

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#include <asf.h>

void show_menu(void);

/* Tests */
int run_hardware_encryption_test(void);
int run_hardware_decryption_test(void);
int run_hardware_encryp_decrypt_test_random(void);
int run_hardware_encryp_decrypt_test_input(char *input, uint8_t *cipher_text, uint8_t *dec_plain_text);
int run_encrypted_read_test(char *outout, uint16_t address, uint8_t length);
int run_encrypted_write_test(char *input, uint16_t address);
int run_tests(void);
#endif /* MAIN_H_ */