/*
 * ataes132a_impl.h
 *
 * Created: 8/13/2018 6:46:41 AM
 *  Author: pvallone
 */ 


#ifndef ATAES132A_IMPL_H_
#define ATAES132A_IMPL_H_
#include <asf.h>
#include "tomcrypt.h"
#define AES_NONCE_MODE_INBOUND      (0x00)              //!< Nonce command mode inbound. Use the InSeed as the Nonce
#define AES_NONCE_MODE_RAND_RESEED  (0x01)              //!< Nonce command mode random with EEPROM RNG seed update
#define AES_NONCE_MODE_RAND         (0x03)              //!< Nonce command mode random using existing RNG seed
#define AES_MACFLAG_RANDOM          (0x01)              //!< MacFlag flag indicating the Nonce was random
#define AES_MACFLAG_INPUT           (0x02)              //!< MacFlag flag indicating an input MAC (vs output)
#define AES_ENC_MODE_FLAG_USAGE_COUNTER (0x20)          //!< Mode flag. Include the Usage Counter associated with the encryption key in the MAC.
#define AES_ENC_MODE_FLAG_SERIAL_NUM    (0x40)          //!< Mode flag. Include SerialNum in the MAC.
#define AES_ENC_MODE_FLAG_SMALL_ZONE    (0x80)          //!< Mode flag. Include the first four bytes of SmallZone in the MAC.
#define AES_ENC_MODE_FLAG_SMALL_ZONE_SERIAL_NUM    (0xC0)          //!< Mode flag. Include the first four bytes of SmallZone in the MAC and serial Number.

int ret_val;

struct aes132a_details {
	uint8_t serial_number[8];
	uint8_t lot_history[8];
	uint8_t enc_read_size;
	uint8_t enc_write_size;
	uint8_t ee_page_size;
	uint8_t device_number;
	uint8_t lock_keys;
	uint8_t lock_small;
	uint8_t lock_config;
	uint8_t manufacturing_id[2];
	uint8_t i2c_address;
	uint8_t chip_config;
};


/** \brief Structure for the parameters required by the atcah_aes132_mac() function.
 */
struct atcah_aes132_mac_params_t
{
    const uint8_t* key;              //!< Encryption key (16 bytes).
    const uint8_t* nonce;            //!< Current value of the nonce register (12 bytes).
    uint8_t        mac_count;        //!< 1 for first MAC generation.
    uint16_t       length;           //!< Always 0x0000 for authentication only.
    uint16_t       auth_only_length; //!< Number of bytes of authenticate-only data
    const uint8_t* auth_only_data;   //!< Authenticate-only data
    const uint8_t* plain_text;       //!< Plain text data size is length. Can be NULL if length is 0.
};

/** \brief Structure for the parameters required by the atcah_aes132_integrity_mac() function.
 */
struct atcah_aes132_integrity_mac_params_t
{
    uint8_t        op_code;         //!< Op code of the command the MAC is for.7
    uint8_t        mode;            //!< Mode of the command.
    uint16_t       param1;          //!< Param1 of the command.
    uint16_t       count;           //!< Param2 of the command.
    const uint8_t* key;             //!< Encryption key (16 bytes).
    const uint8_t* nonce;           //!< Current value of the nonce register (12 bytes).
    uint8_t        mac_count;       //!< 1 for first MAC generation.
    uint8_t        is_nonce_rand;   //!< TRUE if the Nonce command was run with the RNG enabled, and the Nonce is guaranteed to be unique.
    uint8_t        is_input_mac;    //!< TRUE is the MAC is being used as an input (Decrypt, EncWrite).
    uint16_t       mfg_id;          //!< Manufacturing ID from configuration zone
    uint8_t        usage_count[4];  //!< Usage counter value for the encryption key. Ignored if AES_ENC_MODE_FLAG_USAGE_COUNTER flag is not set in mode.
    uint8_t        serial_num[8];   //!< Serial number of the device. Ignored if AES_ENC_MODE_FLAG_SERIAL_NUM flag is not set in mode.
    uint8_t        small_zone[4];   //!< First 4 bytes of the device small zone. Ignored if AES_ENC_MODE_FLAG_SMALL_ZONE flag is not set in mode.
    const uint8_t* plain_text;      //!< Plain text data size is LSB of count. Can be NULL if length is 0.
} ;

/** \brief Structure for the parameters required by the atcah_aes132_encrypt_for() function.
 */
 struct atcah_aes132_encrypt_params_t
{
    uint8_t        op_code;         //!< Command op code the encrypted data is for (Decrypt or EncWrite)
	uint8_t        mode;            //!< Encrypt or EncWrite mode flags.
	uint16_t       param1;          //!< KeyId for Encrypt or Address for EncWrite
	uint16_t       count;           //!< How many bytes to write (1 to 32).
	const uint8_t* plain_text;      //!< Plain text data to be written.
	const uint8_t* key;             //!< Encryption key (16 bytes).
	const uint8_t* nonce;           //!< Current value of the nonce register (12 bytes).
	uint8_t        is_nonce_rand;   //!< TRUE if the Nonce command was run with the RNG enabled, and the Nonce is guaranteed to be unique.
	uint16_t       mfg_id;          //!< Manufacturing ID from configuration zone
	uint8_t        usage_count[4];  //!< Usage counter value for the encryption key. Ignored if AES_ENC_MODE_FLAG_USAGE_COUNTER flag is not set in mode.
	uint8_t        serial_num[8];   //!< Serial number of the device. Ignored if AES_ENC_MODE_FLAG_SERIAL_NUM flag is not set in mode.
	uint8_t        small_zone[4];   //!< First 4 bytes of the device small zone. Ignored if AES_ENC_MODE_FLAG_SMALL_ZONE flag is not set in mode.
} ;

/** \brief Structure for the parameters required by the atcah_aes132_decrypt_from() function.
 */
 struct atcah_aes132_decrypt_params_t
{
    uint8_t        op_code;         //!< EncRead or Encrypt OpCode used to encrypt the data
	uint8_t        mode;            //!< Command mode flags
	uint16_t       param1;          //!< Command param1 (address for EncRead or KeyId for Encrypt)
	uint16_t       count;           //!< How many bytes of data to decrypt
	const uint8_t* out_mac;         //!< Integrity MAC to be verified (16 bytes)
	const uint8_t* out_data;        //!< Cipher text data to be decrypted. Must be 16 bytes (count <= 16) or 32 bytes (count > 16).
	const uint8_t* key;             //!< Encryption key (16 bytes).
	const uint8_t* nonce;           //!< Current value of the nonce register (12 bytes).
	uint8_t        is_nonce_rand;   //!< TRUE if the Nonce command was run with the RNG enabled, and the Nonce is guaranteed to be unique.
	uint16_t       mfg_id;          //!< Manufacturing ID from configuration zone
	uint8_t        usage_count[4];  //!< Usage counter value for the encryption key. Ignored if AES_ENC_MODE_FLAG_USAGE_COUNTER flag is not set in mode.
	uint8_t        serial_num[8];   //!< Serial number of the device. Ignored if AES_ENC_MODE_FLAG_SERIAL_NUM flag is not set in mode.
	uint8_t        small_zone[4];   //!< First 4 bytes of the device small zone. Ignored if AES_ENC_MODE_FLAG_SMALL_ZONE flag is not set in mode.
} ;


struct i2c_master_packet scan_packet;
uint8_t scan_buffer[3];
void aes_scan_i2c(void);
void aes_read_configuration(void);
void aes_print_buffer(uint8_t *buff, uint8_t size);
void aes_print_buffer_for_aces(uint8_t *buff, uint8_t size);
void aes_print_rc(int ret_value);

int aes_read_serial_number(uint8_t *serial_number);
int aes_get_aes132a_details(struct aes132a_details * details);
int aes_config_read(uint16_t keyAddress);
int aes_create_nonce(uint8_t mode, uint8_t *nonceOut, uint8_t *inseed);
int aes_create_pass_through_nonce(uint8_t *seed);
int aes_create_random_number(uint8_t *random);
int aes_read__manufacturing_id(uint8_t * mfg_id);

int aes_config_write(uint16_t keyAddress, uint8_t * settings);
int aes_counter_config_write(uint16_t keyAddress, uint8_t * settings);
int aes_key_write(uint16_t keyAddress, uint8_t * settings);
int aes_lock_zone(uint8_t mode);
int aes_info(uint16_t selector);
void aes_reset(void);
int aes_auth(uint16_t key, uint8_t mode, uint16_t usage, uint8_t *in_mac, uint8_t *out_mac);


int aes_block_read_memory(uint16_t address, uint8_t length, uint8_t *read_data);
int aes_write_to_userzone(uint16_t address, uint8_t * data, uint8_t data_length);
int aes_encrypt(uint8_t mode, uint16_t key_id, uint16_t count, uint8_t* in_data, uint8_t out_mac[16], uint8_t* out_data);
int aes_decrypt(uint8_t mode, uint16_t key_id, uint16_t count, uint8_t in_mac[16], uint8_t* in_data, uint8_t* out_data);
int atcah_aes132_mac(const struct atcah_aes132_mac_params_t* params, uint8_t pt_mac[16]);
int atcah_aes132_integrity_mac(const struct atcah_aes132_integrity_mac_params_t* params, uint8_t pt_mac[16]);
int atcah_aes132_encrypt_for(const struct atcah_aes132_encrypt_params_t* params, uint8_t mac[16], uint8_t* ct);
int atcah_aes132_decrypt_from(const struct atcah_aes132_decrypt_params_t* params, uint8_t* plain_text);
int atcah_aes132_nonce(uint8_t mode, const uint8_t in_seed_val[12], uint16_t mfg_id, const uint8_t random[12], uint8_t nonce[12]);
int aes_encrypt_write(uint8_t mode, uint16_t address, uint16_t count, const uint8_t in_mac[16], const uint8_t* in_data);
int aes_encrypt_read(uint8_t mode, uint16_t address, uint16_t count, uint8_t out_mac[16], uint8_t* out_data);



#endif /* ATAES132A_IMPL_H_ */