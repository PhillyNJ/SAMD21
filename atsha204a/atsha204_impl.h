/*
 * atsha204_impl.h
 *
 * Created: 5/25/2019 8:10:11 AM
 *  Author: pvallone
 */ 


#ifndef ATSHA204_IMPL_H_
#define ATSHA204_IMPL_H_

#include "cryptoauthlib.h"
#include "host/atca_host.h"
#include "atsha204a_config.h"
#include "print_helpers.h"

ATCA_STATUS status;

void	sha204_parser_rc(uint8_t ret);
uint8_t sha204_get_details(struct sha204_details *details);
uint8_t sha204_personalize(void);
void sha204_parse_config_section(uint8_t slot, uint8_t hi, uint8_t lower);
uint8_t sha204_read_config(void);
uint8_t sha204_read_otp_zone(uint8_t len, uint8_t block, uint8_t *read_data);
uint8_t sha204_write_keys(uint8_t *root);
uint8_t sha204_create_diverse_key(uint8_t *rootkey, uint16_t target, uint8_t *outkey, uint8_t padding);
uint8_t sha204_write_otp_zone(uint8_t *data, uint8_t len, uint8_t block);
uint8_t sha204_hmac_challenge(uint16_t slot, uint8_t *key);
uint8_t sha204_create_derived_key(uint16_t target, uint8_t *mac, uint8_t padding);
uint8_t sha204_gendig_example(uint16_t target_key_id,uint16_t mac_key);
uint8_t sha204_random_challenge(uint8_t slot, uint8_t *secretKey);
uint8_t sha204_random_mac_challenge(uint8_t slot, uint8_t mode, uint8_t *challenge,  uint8_t *sw_digest);
uint8_t sha204_checkmac_example(uint16_t key_id,uint16_t mac_key);
uint8_t sha204_diverse_key_example(uint16_t target, uint8_t *mac, uint8_t padding);
void sha204_compare_digests(uint8_t *dig1,uint8_t *dig2, int len);
uint8_t sha204_validate_keys(uint8_t *root);
uint8_t sha204_calculated_rolled_key(uint8_t *root, uint8_t key_id, uint8_t roll_count, uint8_t *derived_key);
uint8_t sha204_get_slot_config(uint8_t* config_data);
uint8_t sha204_key_roll_example_validation(uint8_t *rootkey);
uint8_t sha204_validate_derived_key(uint8_t *root, uint8_t key_id, uint8_t padding, uint8_t *derived_key);
uint8_t sha204_validate_key_gendig(uint8_t *root, uint16_t key_id, uint16_t mac_key, uint8_t padding);
#endif /* ATSHA204_IMPL_H_ */