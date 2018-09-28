/*
 * aes132_impl.h
 *
 * Created: 9/15/2018 7:11:10 AM
 *  Author: pvallone
 */ 


#ifndef AES132_IMPL_H_
#define AES132_IMPL_H_

#define AES132SHOWCOMMANDBLOCK

#include "stdlib.h"
#include <string.h>
#include "aes132_helper.h"
#include "aes132_comm.h"
#include "aes132_comm_marshaling.h"
#include "keys.h"
struct aes132a_details mydetails;

uint8_t g_tx_buffer[AES132_COMMAND_SIZE_MAX];
uint8_t g_rx_buffer[AES132_RESPONSE_SIZE_MAX];
struct aes132h_nonce_s g_nonce;

void aes132_print_command_block(int ret_code);
void aes132_read_device_details(struct aes132a_details * details);
void aes132_read_small_zone(uint8_t *data, uint8_t bytesToRead);
void aes132_read_info(uint8_t selector, uint8_t out_data[2]);

void aes132_inbound_auth_key(uint8_t keyid, uint8_t *key, uint16_t usage);
void aes132_personalize(void);
void aes132_read_configuration(void);
void aes132_read_config(uint16_t addr);
void aes132_read_counter_config(uint16_t addr);
void aes132_lock_zone(uint8_t mode);
void aes132_nonce(void);
int aes132_key_create(uint8_t *newKey);
void aes132_keyload_to_key_memory(uint16_t childKeyID, uint8_t *parentKey, uint8_t *newKey);
void aes132_encrypt_decrypt_key(uint16_t eKeyId, uint8_t *ekey);
void aes132_outbound_auth(uint16_t keyid, uint8_t *key, uint8_t mode, uint16_t usage);
void aes132_mutual_auth(uint16_t keyid, uint8_t *key, uint8_t mode, uint8_t usage);
void aes132_reset_auth(void);
void aes132_encrypt_decrypt(void);
void aes132_encrypt_encwrite(void);
void aes132_encread_decrypt(void);
void aes132_write_user_zone(void);
void aes132_volatile_keyload(void);
void aes132_volatile_key_encrypt_decrypt(void);
void aes132_create_random_number(uint8_t * random);
void aes132_read_user_zone(void);
void aes132_write_user_zone_sn(void);
void aes132_encread_decrypt_sn(void);
void aes132_read_serial(uint8_t *sn);
void aes132_decrypt_read(void);
void aes132_write_user_zone_04(void);
void aes132_crunch(uint16_t count, uint8_t *outResult);
#endif /* AES132_IMPL_H_ */