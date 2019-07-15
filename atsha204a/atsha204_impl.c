/*
 * atsha204_impl.c
 *
 * Created: 5/25/2019 8:10:20 AM
 *  Author: pvallone
 */ 
#include "atsha204_impl.h"

 uint8_t sha204_get_details(struct sha204_details *details){
	 
	 uint8_t block = 0;
	 uint8_t offset = 0;
	 uint8_t data[4];
	 memset(&data, 0, sizeof(data));
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);
	 memcpy(&details->serial_number[0], &data, 4);
	 //sha204_print_buffer(data, 4);
	 offset = 1;
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);
	 memcpy(&details->rev_num[0], &data, 4);
	 offset = 2;
	 
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);
	 memcpy(&details->serial_number[4], &data, 4);
	 offset = 3;
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);
	 details->serial_number[8] = data [0];
	 // get CheckMacConfig and i2c setting	
	 offset = 4;
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);	  
	 details->i2c_address = data[0];
	 details->check_mac_config = data[1];
	 details->otp_mode = data[2];
	 details->select_mode = data[3];	  
	
	 // get lock and lockConfig
	 block = 2;
	 offset = 5;
	 status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 RET_STATUS(status);
	 details->lock_value = data[2];
	 details->lock_config = data[3];

	 return status;
 }
 
 void sha204_parser_rc(uint8_t ret){
	 printf(" (");
	 switch (ret)
	 {
		 case 0x00:
		 printf("ATCA_SUCCESS");
		 break;
		 case 0x01:
		 printf("ATCA_CONFIG_ZONE_LOCKED");
		 break;
		 case 0x02:
		 printf("ATCA_DATA_ZONE_LOCKED");
		 break;
		 case 0xD0:
		 printf("ATCA_WAKE_FAILED");
		 break;
		 case 0xD1:
		 printf("ATCA_CHECKMAC_VERIFY_FAILED");
		 break;
		 case 0xD2:
		 printf("ATCA_PARSE_ERROR");
		 break;
		 case 0xD4:
		 printf("ATCA_STATUS_CRC");
		 break;
		 case 0xD5:
		 printf("ATCA_STATUS_UNKNOWN");
		 break;
		 case 0xD6:
		 printf("ATCA_STATUS_ECC");
		 break;
		 case 0xD7:
		 printf("ATCA_STATUS_SELFTEST_ERROR");
		 break;
		 case 0xE0:
		 printf("ATCA_FUNC_FAIL");
		 break;
		 case 0xE1:
		 printf("ATCA_GEN_FAIL");
		 break;
		 case 0xE2:
		 printf("ATCA_BAD_PARAM");
		 break;
		 case 0xE3:
		 printf("ATCA_INVALID_ID");
		 break;
		 case 0xE4:
		 printf("ATCA_INVALID_SIZE");
		 break;
		 case 0xE5:
		 printf("ATCA_RX_CRC_ERROR");
		 break;
		 case 0xE6:
		 printf("ATCA_RX_FAIL");
		 break;
		 case 0xE7:
		 printf("ATCA_RX_NO_RESPONSE");
		 break;
		 case 0xE8:
		 printf("ATCA_RESYNC_WITH_WAKEUP");
		 break;
		 case 0xE9:
		 printf("ATCA_PARITY_ERROR");
		 break;
		 case 0xEA:
		 printf("ATCA_TX_TIMEOUT");
		 break;
		 case 0xEB:
		 printf("ATCA_RX_TIMEOUT");
		 break;
		 case 0xEC:
		 printf("ATCA_TOO_MANY_COMM_RETRIES");
		 break;
		 case 0xED:
		 printf("ATCA_SMALL_BUFFER");
		 break;
		 case 0xF0:
		 printf("ATCA_COMM_FAIL");
		 break;
		 case 0xF1:
		 printf("ATCA_TIMEOUT");
		 break;
		 case 0xF2:
		 printf("ATCA_BAD_OPCODE");
		 break;
		 case 0xF3:
		 printf("ATCA_WAKE_SUCCESS");
		 break;
		 case 0xF4:
		 printf("ATCA_EXECUTION_ERROR");
		 break;
		 case 0xF5:
		 printf("ATCA_UNIMPLEMENTED");
		 break;
		 case 0xF6:
		 printf("ATCA_ASSERT_FAILURE");
		 break;
		 case 0xF7:
		 printf("ATCA_TX_FAIL");
		 break;
		 case 0xF8:
		 printf("ATCA_NOT_LOCKED");
		 break;
		 case 0xF9:
		 printf("ATCA_NO_DEVICES");
		 break;
		 case 0xFA:
		 printf("ATCA_HEALTH_TEST_ERROR");
		 break;
		 default:
		 printf("Error Unknown");
		 break;
	 }
	 printf(")\n\r$");
 }
 
 void sha204_parse_config_section(uint8_t slot, uint8_t hi, uint8_t lower){
	 //((a >> 3)  & 0x01)
	 /*
	 byte b = 0xAB;	 
	 var low = b & 0x0F;
	 var high = b >> 4;	 
	 */
	// uint8_t read_key = lower & 0x0F;
	 uint8_t read_key_0 = ((hi >> 0)  & 0x01);
	 uint8_t read_key_1 = ((hi >> 1)  & 0x01);
	 uint8_t read_key_2 = ((hi >> 2)  & 0x01);
	 uint8_t read_key_3 = ((hi >> 3)  & 0x01);
	 uint8_t check_only = ((hi >> 4)  & 0x01);
	 uint8_t limited_use = ((hi >> 5)  & 0x01);
	 uint8_t encrypt_read = ((hi >> 6)  & 0x01);
	 uint8_t is_secret = ((hi >> 7)  & 0x01);
	// uint8_t write_key = hi >> 4;
	
	 uint8_t write_key_8 = ((lower >> 0)  & 0x01);
	 uint8_t write_key_9 = ((lower >> 1)  & 0x01);
	 uint8_t write_key_10 = ((lower >> 2)  & 0x01);
	 uint8_t write_key_11 = ((lower >> 3)  & 0x01);
	 uint8_t write_config_12 = ((lower >> 4)  & 0x01);
	 uint8_t write_config_13 = ((lower >> 5)  & 0x01);
	 uint8_t write_config_14 = ((lower >> 6)  & 0x01);
	 uint8_t write_config_15 = ((lower >> 7)  & 0x01);	 
	
	
	 printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t0x%02X 0x%02X", slot,
	 write_config_15,write_config_14,write_config_13,write_config_12,write_key_11,  
	 write_key_10, write_key_9,write_key_8,is_secret,encrypt_read,limited_use, check_only,
	 read_key_3,read_key_2,read_key_1,read_key_0, lower, hi);
	 
	 printf("\tWrite Key: %d Read Key: %d Secret: %s Encrypt Read: %s Encrypt Write: %s", (lower & 0x0F), (hi >> 4), (is_secret == 1) ? "Yes" : "No", (encrypt_read == 1 && is_secret == 1) ? "Yes" : "No", write_config_14 == 1 ? "Yes" : "No");
	 printf("\n\r");
};
 
/*  \brief Reads the device Configuration section
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_read_config(){
	printf("\tWrite\tWrite\tWrite\tWrite\tWrite\tWrite\tWrite\tWrite\tIs\tEncrypt\tLimit\tCheck\tRead\tRead\tRead\tRead\n\r");
	printf("\tConfig\tConfig\tConfig\tConfig\tkey\tkey\tkey\tkey\tSecret\tRead\tUse\tOnly\tKey\tKey\tKey\tKey\n\r");
	printf("-------------------------------------------------------------------------------------------------------------------------------------\n\r");	 
	printf("Slot\t15\t14\t13\t12\t11\t10\t9\t8\t7\t6\t5\t4\t3\t2\t1\t0\n\r");
	printf("-------------------------------------------------------------------------------------------------------------------------------------\n\r");

	status = ATCA_SUCCESS;
	uint8_t data[4];
	uint8_t data_display[4];
	uint8_t block = 0;
	uint8_t offset = 0;
    uint8_t slot = 0;
	
	for(offset = 5; offset < 8; offset++){

		status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		RET_STATUS(status);
		data_display[0] = data[1];
		data_display[1] = data[0];				
		data_display[2] = data[3];
		data_display[3] = data[2];
				
		sha204_parse_config_section(slot, data_display[1],data_display[0]);
		slot++;
		sha204_parse_config_section(slot, data_display[3],data_display[2]);		
		slot++;
	}
	
	block = 1;
	for(offset = 0; offset < 5; offset++){	
		status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		RET_STATUS(status);
		data_display[0] = data[1];
		data_display[1] = data[0];
		data_display[2] = data[3];
		data_display[3] = data[2];
		sha204_parse_config_section(slot, data_display[1],data_display[0]);
		slot++;
		sha204_parse_config_section(slot, data_display[3],data_display[2]);
		slot++;
	}
	return status;
}
uint8_t sha204_personalize(void){
	
	status = ATCA_SUCCESS;
	uint8_t data[4];
	uint8_t block = 0;
	uint8_t offset = 5;
	// Slots 0 & 1
	data[0] = 0x8F; // LSB on spreadsheet
	data[1] = 0x8F;
	data[2] = 0x89; // LSB on spreadsheet
	data[3] = 0xF2;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 2 & 3
	offset = 6;
	data[0] = 0x89;
	data[1] = 0xF2; 
	data[2] = 0x89; // slot 3
	data[3] = 0xF2;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 4 & 5
	offset = 7;
	data[0] = 0xD9;
	data[1] = 0xE2;
	data[2] = 0x99; // key 5
	data[3] = 0xB2;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 6 & 7
	block = 1;
	offset = 0;
	data[0] = 0x89;
	data[1] = 0x8F;
	data[2] = 0xDA;
	data[3] = 0x3B;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);
	// Slots 8 & 9
	offset = 1;
	data[0] = 0xCA;
	data[1] = 0x4A;
	data[2] = 0x9F;
	data[3] = 0x3B;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 10 & 11
	offset = 2;
	data[0] = 0x9F;
	data[1] = 0x8F;
	data[2] = 0x8F; // second root key for checkmac
	data[3] = 0x8F;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 12 & 13
	offset = 3;
	data[0] = 0xCA;
	data[1] = 0x4A;
	data[2] = 0xCA;
	data[3] = 0x4A;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	// Slots 14 & 15
	offset = 4;
	data[0] = 0x0F;
	data[1] = 0x00;
	data[2] = 0xCD;
	data[3] = 0x4D;
	// write to each slot config
	status = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	RET_STATUS(status);

	return ATCA_SUCCESS;
}
/*  \brief
	 Reads the OTP Zone. Please note: 
	 Prior to locking the configuration section using Lock(LockConfig), 
	 the OTP zone is inaccessible and can be neither read nor written. 
	 After configuration locking, but prior to locking of the OTP zone using 
	 Lock(LockValue), the entire OTP zone can be written using the Write command. 
	 If desired, the data to be written can be encrypted. 
	 When unlocked the OTP zone cannot be read. 

 * \param[in] len			length of data to read - can be 4 or 32 only
 * \param[in] block			block to read. Can be 0 or 1
 * \param[out] read_data   pointer to where the data will be read to
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_read_otp_zone(uint8_t len, uint8_t block, uint8_t *read_data){

	status = ATCA_SUCCESS;
	if ((len != 4 && len != 32) || block > 1)
	{
		return ATCA_BAD_PARAM;
	}

	status = atcab_read_zone(ATCA_ZONE_OTP, 0, block, 0, read_data, len);
	RET_STATUS(status);

	return status;
}
/** \brief Creates the diverse keys for the demo. The keys are diversified keys, root key to the ATSHA204a
 *	keys 0-9, 12-15 are diversified. The root key is saved to slot 10 & 11 for demo
 *  Key 5 is initially derived from the root, however, running the ATCA_DERIVE_KEY against this slot, will 
	change the key because the SlotConfig<15> bit = 1. The key is no derived from the parent key.  
 * \param[in] root   pointer to the root key 
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_write_keys(uint8_t *root){
	
	uint8_t data[32] = {0};
	uint8_t padding = 0x77;
	status = ATCA_SUCCESS;
		
	// write keys (slots)
	status = sha204_get_details(&myDetails);
	RET_STATUS(status);
	if(myDetails.lock_config != 0x00){
		printf("Lock Config Must be Locked to perform function\n\r");
		return status;
	}
	memset(&data, 0, sizeof(data));
   
	// the rest of the keys are diversified 0-5
	for(uint8_t i = 0; i < 6; i++){
		
		status= sha204_create_diverse_key(root, i, data, padding);
		RET_STATUS(status);
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		RET_STATUS(status);
		printf("Diverse Key 0x%02X: ", i);
		print_buffer(data, 32);
	}
	
	// create the key for slot 7  (host and save in slot 6 client)
	status= sha204_create_diverse_key(root, 7, data, padding);
	RET_STATUS(status);
	//save to slot 7
	status = atcab_write_zone(ATCA_ZONE_DATA, 7, 0, 0, data, 32);
	RET_STATUS(status);
	printf("Diverse Key 0x%02X: ", 7);
	print_buffer(data, 32);
	// save the key in slot 6 client
	status = atcab_write_zone(ATCA_ZONE_DATA, 6, 0, 0, data, 32);
	RET_STATUS(status);
	printf("Diverse Key 0x%02X: ", 6);
	print_buffer(data, 32);
	
	// slot 8 & 9
	for(uint8_t i = 8; i < 10; i++){
		
		status= sha204_create_diverse_key(root, i, data, padding);
		RET_STATUS(status);
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		RET_STATUS(status);
		printf("Diverse Key 0x%02X: ", i);
		print_buffer(data, 32);
	}
	
	// the rest of the keys are diversified 12-14
	for(uint8_t i = 12; i < 16; i++){
			
		status= sha204_create_diverse_key(root, i, data, padding);
		RET_STATUS(status);
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		RET_STATUS(status);
		printf("Diverse Key 0x%02X: ", i);			
		print_buffer(data, 32);
	}
	
	 //  write the root key for demo - Slot 10
	 status = atcab_write_zone(ATCA_ZONE_DATA, 10, 0, 0, root, ATCA_KEY_SIZE);
	 RET_STATUS(status);
	 printf("Root Key written to slot 10\n\r");
	 
	 status = atcab_write_zone(ATCA_ZONE_DATA, 11, 0, 0, root, ATCA_KEY_SIZE);
	 RET_STATUS(status);
	 printf("Root Key written to slot 11\n\r"); 
	
	return ATCA_SUCCESS;
}

/** \brief Creates a diverse key same as the sha204 hardware
 * \param[in] rootkey   pointer to the root key used to create diversified keys
 * \param[in] target	slot to be created
 * \param[out] outkey	the key that was created
 * \param[out] padding	the padding used to create the keys.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_create_diverse_key(uint8_t *rootkey, uint16_t target, uint8_t *outkey, uint8_t padding){
	
	status = ATCA_GEN_FAIL;
	uint8_t data[96] = {0};
	uint8_t diverse_key[32] = {0};
	uint8_t sn[9] = {0};
	memset(&data, 0, sizeof(data));
	memcpy(&data[0], &rootkey[0], 32);
	data[32] = ATCA_DERIVE_KEY;  //opcode
	data[33] = CHECKMAC_MODE_SOURCE_FLAG_MATCH; // param 1 mode - The value of this bit must match the value in TempKey.SourceFlag or the command returns an error.

	status = atcab_read_serial_number(sn);
	RET_STATUS(status);
	data[34] = (uint8_t)target; // param 2 (target key)
	data[35] = 0x00;			// param 2

	data[36] = sn[8];
	data[37] = sn[0];
	data[38] = sn[1];

	memcpy(&data[64], sn, sizeof(sn));
	memset(&data[73], padding, 24);

	status = atcac_sw_sha2_256(data, sizeof(data), diverse_key);
	RET_STATUS(status);
	memcpy(&outkey[0], &diverse_key[0], sizeof(diverse_key));

	return status;

}
/** \brief Creates a diverse key same as the sha204 hardware
 * \param[in] data		pointer to the data to be written
 * \param[in] len		length of data - must be 4 or 32
 * \param[in] block		OTP block to write to. Must be 0 or 1
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_write_otp_zone(uint8_t *data, uint8_t len, uint8_t block){
	
	
	status = ATCA_SUCCESS;
	if ((len != 4 && len != 32) || block > 1)
	{
		return ATCA_BAD_PARAM;
	}

	status = atcab_write_zone(ATCA_ZONE_OTP, 0, block, 0, data, len);
	RET_STATUS(status);

	return status;
}

/** \brief Preforms a hmac challange by creating a HMAC on the ATSHA204a and Software to simulate
 *	\brief a host client challange
 *  \param[in] slot		slot to run the challenge against
 *  \param[in] key		pointer to the key to run the challenge against
 *  \return				ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_hmac_challenge(uint16_t slot, uint8_t *key){
	
	status = ATCA_GEN_FAIL;
	uint8_t sn[ATCA_SERIAL_NUM_SIZE];
	uint8_t otp[ATCA_OTP_SIZE];
	uint8_t num_in[20];
	struct atca_temp_key temp_key;
	atca_nonce_in_out_t nonce_params;
	uint8_t rand_out[32];
	uint8_t hmac_digest[32];
	struct atca_hmac_in_out hmac_params;
	uint8_t hmac_digest_host[32];
	uint8_t modes[] = {
		HMAC_MODE_FLAG_TK_RAND,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP88,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP88 | HMAC_MODE_FLAG_OTP64,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP88 | HMAC_MODE_FLAG_OTP64 | HMAC_MODE_FLAG_FULLSN,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP88 | HMAC_MODE_FLAG_FULLSN,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP64,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_OTP64 | HMAC_MODE_FLAG_FULLSN,
		HMAC_MODE_FLAG_TK_RAND | HMAC_MODE_FLAG_FULLSN,
	};
	size_t i = 0;

	status = atcab_read_serial_number(sn);
	RET_STATUS(status);

	status = atcab_read_bytes_zone(ATCA_ZONE_OTP, 0, 0, otp, ATCA_OTP_SIZE);
	RET_STATUS(status);

	for (i = 0; i < sizeof(modes) / sizeof(modes[0]); i++)
	{
		//Calculate hmac using chip
		memset(&temp_key, 0, sizeof(temp_key));
		memset(num_in, 0, sizeof(num_in));
		memset(&nonce_params, 0, sizeof(nonce_params));
		nonce_params.mode = NONCE_MODE_SEED_UPDATE;
		nonce_params.zero = 0;
		nonce_params.num_in = num_in;
		nonce_params.temp_key = &temp_key;
		nonce_params.rand_out = rand_out;
		status = atcab_nonce_rand(nonce_params.num_in, rand_out);
		RET_STATUS(status);

		status = atcah_nonce(&nonce_params);
		RET_STATUS(status);

		hmac_params.mode = modes[i];
		hmac_params.key_id = slot;
		hmac_params.key = key;
		hmac_params.otp = otp;
		hmac_params.sn = sn;
		hmac_params.response = hmac_digest_host;
		hmac_params.temp_key = &temp_key;
		status = atcab_hmac(hmac_params.mode, hmac_params.key_id, hmac_digest); // hardware
		RET_STATUS(status);

		//Calculate hmac on host - software
		status = atcah_nonce(&nonce_params);
		RET_STATUS(status);

		status = atcah_hmac(&hmac_params); // host
		RET_STATUS(status);

		//Compare Chip and Host calculations
		if(memcmp (hmac_digest, hmac_params.response, sizeof(hmac_digest))== 0){
			printf("-Equal for Mode 0x%02x\n\r",modes[i]);
		} else {
			printf("-Not Equal for Mode 0x%02x\n\r",modes[i]);
		}
	}
	
	return status;
}


/*
	Based on the example in the application note: 2.3 Validate Using the GenDig Command 
	
	This demonstrates shows how a challenge is done with 2 devices where the host and client do not share keys. 
	
	The host has no knowledge of the clients keys, however the host knows the root key and the client's 
	keys where derived from the root and written to the client during personalization.
	
	This provides a very secure way to authenticate a client without knowing the client's keys. 
	The only data exposed is the client's MAC and SN.
	
	For Example, a host could have many clients like a printer (host) and print cartridges (client).
	
	How it works:
	- The host stores the root key. In this case, slot 10.
	- The client's keys were created using the root key during personalization, but the root key is not stored on the client.
	- The host reserves a slot for authenticating its clients. This is the target for the derived key.
	- When the host detects a client, we preform a gendig challange.
	
	The slot config for this example is set as follows for this to run correctly:
	
	slot 0x00 - aka the client - Is a diverse key, created with the root key. This slot	cannot be read, isSecret, 
	can run a MAC command against it - (checkonly bit is NOT set)
				
	slot 0x07 and 0x09 - aka the host - These slot are reserved for creating the ephemeral diverse key, created 
	with the root key which is also stored in slot 10 (host). 
	
	Slot 0x09 cannot be read, isSecret, cannot run a MAC command against it (checkonly bit is set), parent key is slot 10. 
	The checkmac is run against this slot. The ephemeral keys is create by looking at the targets parent, which is the root key.
	SlotConfig<slot>.13 must be set to run derived key.
	
	Slot 0x07 enc read, isSecret, cannot run a MAC command against it (checkonly bit is set), parent key is slot 10. 
	The checkmac is run against this slot. The ephemeral keys is create by looking at the targets parent, which is the root key. 
	SlotConfig<slot>.13 must be set to run derived key
				
	slot 0x0A - Root Key, cannot be read, isSecret, cannot run MAC command against it. checkonly bit is set.
	
	Steps: 
		1. The client's keys are diversified from the root key before locking (personalization)
		2. The host requests the SN from the client	
		3. A random number is requested from the client to be used for the challenge
		4. A MAC command is requested from the client using the challenge from step 2. The digest will be used in step 7
		5. The host loads the root key and the clients serial number with the nonce command to create the TempKey
		6. The host generates a digest with the gendig command. The digest is created by generating the ephemeral key 
		   (e.g. the same key as client - slot 0 in this case). The digest is generated internally, placed into the TempKey 
		   register and can never be read.
		7. The host calls the checkmac, which compares the MAC generated by the client and the gendig digest result now stored
		   in TempKey register.		
*/
/** \brief Preforms a gendig challange 
 *  \param[in] target_key_id		Host target slot 
 *  \param[in] mac_key				Client slot 
 *  \return							ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_gendig_example(uint16_t target_key_id, uint16_t mac_key){
	
	status = ATCA_SUCCESS;	
	uint8_t serialNumber[9] = { '\0'};
	uint8_t mac[32] = { '\0'};
	uint8_t challenge[32] = { '\0'};
	uint8_t mac_command_data[13] = {'\0'};
	uint8_t nonce_in[32]= { '\0'};
	uint8_t root_key = 0x0A; // root key
	uint8_t padding = 0x77;
	uint8_t mac_mode = MAC_MODE_CHALLENGE;
	
	// this is the command used to create the client's key - used to create the ephemeral key in gendig
	
	uint8_t derive_key_command[4] = {	// this is command used by derivekey command to create the diverse key for target slot
		ATCA_DERIVE_KEY,				// opcode derivekey
		DERIVE_KEY_MODE,				// mode param1 - The value of this bit must match the value in TempKey.SourceFlag or the command returns an error.
		mac_key, 0x00					// param2 target key (upper byte)
	}; // 1C 04 00 00
	
	memset(&mac_command_data, 0, sizeof(mac_command_data));
	mac_command_data[0] = ATCA_MAC;	    // other data is the command to create the mac on the target
	mac_command_data[1] = mac_mode;		//mode used to create mac
	mac_command_data[2] = mac_key;		// key used to create mac - upper byte only
	mac_command_data[3] = 0x00;		    // key used to create mac - always zero	
	
	printf("- Reading SN from client\n\r");
	status = atcab_read_serial_number(serialNumber); // get the serial number
	RET_STATUS(status);
	printf("- SN received from client\n\r");
	
	printf("- Requesting a random number for challenge.\n\r");
	status = atcab_random(challenge);
	RET_STATUS(status);
	printf("- Random number for challenge received\n\r");	
   	
	printf("- Creating mac for client\n\r");	
	status = atcab_mac(mac_mode, mac_key, challenge, mac);
	RET_STATUS(status);
    // Host events
	memset(&nonce_in, padding, sizeof(nonce_in));
	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn
	
	printf("- Creating the TempKey via the nonce command (pass-through) on the host...loading the root key and client's sn\n\r");
   
	status = atcab_nonce_load(root_key, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes
	RET_STATUS(status);
	printf("- Creating the digest internally on the host (gendig)\n\r");
	// execute a gendig on key 10 - master key stored on slot 10

	status = atcab_gendig(GENDIG_ZONE_DATA, root_key, derive_key_command, sizeof(derive_key_command));
	RET_STATUS(status);
	printf("- Checking the digest internally on the host (checkmac) with the mac generated by the client\n\r");
	status = atcab_checkmac(0x06, target_key_id, challenge, mac, mac_command_data);	
	RET_STATUS(status);
	
	printf("- checkmac success!\n\r");
	return status;
}

/** \brief	Creates a diverse key with the derivekey command (hardware)
 *			The device combines the current value of a key with the nonce stored in TempKey using SHA-256 and
 *			places the result into the target key slot. SlotConfig<TargetKey>.Bit13 must be set or DeriveKey returns
 *			an error.
 *  \param[in] target		target slot for derived key to be written to
 *  \param[in] mac			pointer to the mac for validation - can be null
 *  \param[in] padding		Padding used to create the diverse key
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_create_derived_key(uint16_t target, uint8_t *mac, uint8_t padding){
	
	status = ATCA_GEN_FAIL;
	uint8_t serialNumber[9] = { '\0'};
	uint8_t root_key = 10; // stored on the host
	uint8_t nonce_in[32]= { '\0'};
	memset(&nonce_in[0], padding, sizeof(nonce_in)); // we padded with 77 per the App note -
	status = atcab_read_serial_number(serialNumber); // get the serial number
	RET_STATUS(status);

	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn
	
	status = atcab_nonce_load(root_key, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through
	RET_STATUS(status);

	status = atcab_derivekey(DERIVE_KEY_MODE, target, mac);
	RET_STATUS(status);

	return status;
}

/** \brief	Performs a random challenge on a slot that is diversified
 *  \param[in] slot			target slot to run challenge against
 *  \param[in] secretKey	diverse key created in software

 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_random_challenge(uint8_t slot, uint8_t *secretKey){
	
	status = ATCA_SUCCESS;
	uint8_t digest[32]= { '\0'};
	uint8_t sw_digest[32]= { '\0'};
	uint8_t serialNumber[9] = { '\0'};
	uint8_t challenge[32];
	uint8_t request[88] = { '\0'};
	uint8_t mode = 0x00;
	status = atcab_read_serial_number(serialNumber);
	RET_STATUS(status);
	
	// request a random number for the challenge
	status = atcab_random(challenge);
	RET_STATUS(status);

	status = atcab_mac(mode, 0x00 + slot, challenge, digest);
	RET_STATUS(status);
	
	memset(request, 0, sizeof request); // set all with 0	
	memcpy(request, secretKey, 32);
	memcpy(&request[32], &challenge, 32);

	request[64] = ATCA_MAC; //opcode
	request[65] = mode; //param1 mac mode
	request[66] = slot; //param2 upper byte Key Slot ID
	request[67] = 0x00; //param2 Lower byte always 00
	request[79] = serialNumber[8]; //0xEE; // SN<8>
	request[84] = serialNumber[0]; //0x01; // SN<0:1>
	request[85] = serialNumber[1]; //0x23; // SN<0:1>
	request[86] = 0x00;
	request[87] = 0x00;

	status = atcac_sw_sha2_256(request, 88, sw_digest);
	RET_STATUS(status);
	return memcmp ( digest, sw_digest, sizeof(sw_digest) );

}

/** \brief					Performs a random  mac challenge on a slot (client) that is diversified
 *  \param[in] slot			target slot to run challenge against
 *  \param[in] challenge	pointer to random challenge
 *  \param[in] sw_digest	pointer to digest created with software (host)

 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_random_mac_challenge(uint8_t slot, uint8_t mode, uint8_t *challenge,  uint8_t *sw_digest){
		
	status = ATCA_SUCCESS;
	uint8_t digest[32]= { '\0'};		
	
	status = atcab_mac(mode, 0x00 + slot, challenge, digest);
	RET_STATUS(status);
	
	printf("- HW Digest: ");
	print_buffer(digest, sizeof(digest));
	
	return memcmp ( digest, sw_digest, sizeof(digest) );
}


/** \brief					This example checks the mac per the app note (Atmel-8841-CryptoAuth-ATSHA204-Unique-Keys-ApplicationNote.pdf). 
							Both the client and host must be same. Slot 7 is derived from the root and written to slot 6 & 7. See sha204_write_keys().  
							
							This example simulates host/client where the key in host slot x, is the same as the key in client slot x
							In this case we use the slots 6 (client) & 7 (host) because they are same. 	
							
 *  \param[in] key_id		Key id for host 
 *  \param[in] mac_key		Key id for mac calculation (client) 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_checkmac_example(uint16_t key_id,uint16_t mac_key){
	
	status = ATCA_SUCCESS;
	
	uint8_t serialNumber[9] = { '\0'};
	uint8_t mac[32] = { '\0'};
	uint8_t challange[32] = { '\0'};	
	
	uint8_t nonce_in[32]= { '\0'};
	uint8_t mac_command_data[13] = {'\0'};	
	uint8_t mac_mode = MAC_MODE_CHALLENGE;
	
	memset(&mac_command_data, 0, sizeof(mac_command_data));
	mac_command_data[0] = ATCA_MAC;	// other data is the command to create the mac on the target
	mac_command_data[1] = mac_mode;		//mode used to create mac
	mac_command_data[2] = mac_key;		// key used to create mac - upper byte only
	mac_command_data[3] = 0x00;		    // key used to create mac - always zero
	
	status = atcab_read_serial_number(serialNumber); // get the serial number
	if (status != ATCA_SUCCESS)	{printf("Unable to Read Serial Number ");sha204_parser_rc(status);return status;}
	status = atcab_random(challange);
	if(status != ATCA_SUCCESS){	sha204_parser_rc(status);return status;	}	

	status = atcab_mac(0x00, mac_key, challange, mac);
	if (status != ATCA_SUCCESS){printf("Unable to Generate HW Mac ");sha204_parser_rc(status);return status;}
	printf("-Generated Mac: ");
	print_buffer(mac, sizeof(mac));
	 // Generate random number for nonce
	for (uint8_t i = 0; i < 32; i++) {
		 nonce_in[i] = rand() % 0xFF;
	 }	
	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn

	status = atcab_nonce_load(0x03, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes
	RET_STATUS(status);

	status = atcab_checkmac(CHECKMAC_MODE_SOURCE_FLAG_MATCH, key_id, challange, mac ,mac_command_data);	
	RET_STATUS(status);
	
	return status;
}


/** \brief					DeriveKey Example - The method combines the current value of a key with the nonce stored in TempKey using SHA-256 and
							places the result into the target key slot. SlotConfig<TargetKey>.Bit13 must be set or DeriveKey returns
							an error.
 *  \param[in] target		target slot for derived key
 *  \param[in] mac			pointer to mac - can be null
 *  \param[in] padding		padding used to create the key
 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_diverse_key_example(uint16_t target, uint8_t *mac, uint8_t padding){
	
	status = ATCA_GEN_FAIL;
	uint8_t serialNumber[9] = { '\0'};
	
	uint8_t nonce_in[32]= { '\0'};
	memset(&nonce_in[0], padding, sizeof(nonce_in)); // we padded with 0 per the App note -
	status = atcab_read_serial_number(serialNumber); // get the serial number
	RET_STATUS(status);

	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn

	status = atcab_nonce_load(0x03, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through	
	RET_STATUS(status);

	status = atcab_derivekey(DERIVE_KEY_MODE, target, mac);
	RET_STATUS(status);

	return status;
}
/** \brief					Compares 2 digests 
 *  \param[in] dig1			pointer to digest 1
 *  \param[in] dig2			pointer to digest 2
 *  \param[in] len			length of digest to compare
 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
void sha204_compare_digests(uint8_t *dig1,uint8_t *dig2, int len){
	
	// compare hashes
	int n=memcmp ( dig1, dig2, len );

	if (n == 0){
		printf("\n\r- Digests are the same!\n\r");
	} else {
		printf("\n\r- Digests are the NOT the same!\n\r");
	}

}
/** \brief					Validates all the keys written to the device with the checkmac command
							Keys 0-9, 12-15 are diversified. The root key is saved to slot 10 & 11 for demo
							Key 5 is initially derived from the root, however, running the ATCA_DERIVE_KEY against this slot, will
							change the key because the SlotConfig<15> bit = 1. The key is no derived from the parent key.
 *  \param[in] root			pointer to the root key
 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_validate_keys(uint8_t *root){
		
	uint16_t key_id = 0x0000;
	uint8_t challenge[RANDOM_NUM_SIZE];
	uint8_t key[ATCA_KEY_SIZE];
	uint8_t response[MAC_SIZE];
	uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];
	atca_temp_key_t temp_key;
	uint8_t num_in[NONCE_NUMIN_SIZE];
	uint8_t rand_out[RANDOM_NUM_SIZE];
	atca_nonce_in_out_t nonce_params;
	uint8_t sn[ATCA_SERIAL_NUM_SIZE];
	atca_check_mac_in_out_t checkmac_params;
	uint8_t parent_key[ATCA_KEY_SIZE];
	uint8_t config[4];
	uint8_t parent_key_id = 0x00; // default
	uint8_t derived_key[32] = {0};	
	uint8_t nonce_in[32] = {0};		
	size_t i;
	struct atca_derive_key_in_out derivekey_params;
	//uint8_t key04[32] = {0x72,0x25,0x7C,0x5D,0xEB,0x4E,0x5B,0x81,0xEF,0x35,0xCF,0xCF,0x63,0xDD,0xEC,0x57,0x6A,0xAB,0x62,0x25,0x00,0x14,0x93,0x4B,0x22,0xF0,0x7C,0x23,0xDA,0x79,0x19,0xA6};
	//memcpy(&key[0],&key04[0], sizeof(key));
	// calculate the key based on our setup
	for(uint16_t x = 0; x < 16; x++){		
		key_id = 0x00 + x;
		switch (key_id){
			case 0:
			case 1:				
			case 2:
			case 3:			
				status= sha204_create_diverse_key(root, key_id, key, 0x77);
				RET_STATUS(status);
			break;
			case 4:
			
				// need to calculate the derived key
				//struct atca_derive_key_in_out derivekey_params;
				status= sha204_create_diverse_key(root, key_id, key, 0x77);
				RET_STATUS(status);
				status = atcab_read_serial_number(sn);
				if(status != ATCA_SUCCESS){	sha204_parser_rc(status);return status;}	
				// need to set the source flag
				temp_key.source_flag = 1;
				temp_key.valid = 1;
				memset(&nonce_in[0], 0x77, sizeof(nonce_in)); // we padded with 0x77 -
				memcpy(&nonce_in[0], &sn[0], sizeof(sn)); // replace first 9 bytes with sn					
				memcpy(&temp_key.value[0], &nonce_in[0], sizeof(nonce_in));		
								
				derivekey_params.mode = DERIVE_KEY_MODE; // Random nonce generated TempKey // 0x04
				derivekey_params.target_key_id = key_id;
				derivekey_params.parent_key = key;
				derivekey_params.sn = sn;
				derivekey_params.target_key = derived_key;
				derivekey_params.temp_key = &temp_key;
				status = atcah_derive_key(&derivekey_params);				
				RET_STATUS(status);								
				
				memcpy(&key[0],&derived_key[0], sizeof(key));
			
				
			break;	
			case 5:		
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,0,7,config, 4);
				RET_STATUS(status);
				parent_key_id = (config[3] & 0x0F);
				printf("Parent Key: %d \n\r", (config[3] & 0x0F));
			
				// get parent key
				status = sha204_create_diverse_key(root, parent_key_id, parent_key, 0x77);
				RET_STATUS(status);
				// key is created with the parent key, not the root
				status = sha204_create_diverse_key(parent_key, key_id, key, 0x77);
				RET_STATUS(status);
				break;
			break;			
			case 6:
			case 7:
				status= sha204_create_diverse_key(root, 7, key, 0x77);
				RET_STATUS(status);				
			break;
			case 8:
			case 9:
				status= sha204_create_diverse_key(root, key_id, key, 0x77);
				RET_STATUS(status);
			break;
			case 10:
			case 11:
				memcpy(&key, root, ATCA_KEY_SIZE); // root key for slots
			break;
			default:
			status= sha204_create_diverse_key(root, key_id, key, 0x77);
			RET_STATUS(status);
			break;
		}		
				
		status = atcab_random(challenge);
		RET_STATUS(status);
				
		status = atcab_read_serial_number(sn);
		RET_STATUS(status);

		// Perform random nonce
		memset(&temp_key, 0, sizeof(temp_key));
		memset(num_in, 0, sizeof(num_in));
		memset(&nonce_params, 0, sizeof(nonce_params));
		nonce_params.mode = NONCE_MODE_SEED_UPDATE;
		nonce_params.zero = 0;
		nonce_params.num_in = num_in;
		nonce_params.rand_out = rand_out;
		nonce_params.temp_key = &temp_key;
		status = atcab_nonce_rand(nonce_params.num_in, rand_out);
		RET_STATUS(status);
		// Calculate nonce value
		status = atcah_nonce(&nonce_params);
		RET_STATUS(status);
		
		for (i = 0; i < sizeof(other_data); i++)
		{
			other_data[i] = (uint8_t)(i + 0xF0); 
		}
		// Calculate response
		checkmac_params.mode = CHECKMAC_MODE_BLOCK2_TEMPKEY;
		checkmac_params.key_id = key_id;
		checkmac_params.client_chal = NULL;
		checkmac_params.client_resp = response;
		checkmac_params.other_data = other_data;
		checkmac_params.sn = sn;
		checkmac_params.otp = NULL;		
		checkmac_params.slot_key = key;			
		checkmac_params.target_key = NULL;			
		checkmac_params.temp_key = &temp_key;
		status = atcah_check_mac(&checkmac_params); // software
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcah_check_mac for key %d:\n\r", key_id);
			sha204_parser_rc(status);
			// return ret_status;
		} else {
			printf("-Generated Mac for slot %d\n\r", key_id);			
		}

		// Perform CheckMac Hardware
		status = atcab_checkmac(checkmac_params.mode,checkmac_params.key_id,checkmac_params.client_chal,
		checkmac_params.client_resp,checkmac_params.other_data);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcab_checkmac for key %d!!!\n\r", key_id);
			sha204_parser_rc(status);
			
			if(key_id == 0x04){
				printf("-You need to reset the rolled key back for key 4. Run command (AE)\n\r");				
			} else if(key_id == 0x05){				
				printf("-You need to reset the rolled key back for Key 5. Run command (AB)\n\r");
			}			
		} else {

			printf("-Challenge Success for key %d!\n\r", key_id);
		}
	}

	return status;
}
/** \brief					Performs a Key Roll operation on key 04, then validates the calculated key with a checkmac.
							After the checkmac is run, the key is also validated with the gendig/checkmac commands.
							
							The SlotConfig<4> is set to 4  0xE2 0xD9 - A mac is required to run the derived key 
							
							Because the key rolls, you would need to keep track of how many times the derived key was run.
							This is normally done by checking the UpdateCount (run command (AC)). To keep things simple,
							we calculated the diversified key from the root, then do an encrypt write. This resets the key to
							a known value before rolling it. After the keys is "rolled" we validate it.  
 *  \param[in] root			pointer to the root key
 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_key_roll_example_validation(uint8_t *rootkey){
	 
	 printf("Performs a Key Roll operation on key 04, then validates the calculated key with a checkmac.\n\r");
	 printf("After the checkmac is run, the key is also validated with the gendig/checkmac commands.\n\r");
	 printf("**************\n\r");
	 
	 status = ATCA_SUCCESS;		
	 uint16_t target_key_id = 4;
	 uint8_t key_04[ATCA_KEY_SIZE] = {0};
	 uint8_t parent_key[ATCA_KEY_SIZE] = {0};
	 uint8_t test_key[ATCA_KEY_SIZE] = {0};
	 uint8_t nonce_in[NONCE_NUMIN_SIZE_PASSTHROUGH] = {0};
	 uint8_t mac_param[88] = {0};	
	 uint8_t mac_out[MAC_CHALLENGE_SIZE] = {0};	 
	 uint8_t mac_mode = CHECKMAC_MODE_BLOCK1_TEMPKEY | CHECKMAC_MODE_SOURCE_FLAG_MATCH;	 		 
	 uint8_t mac[ATCA_SHA_DIGEST_SIZE] = { '\0'};	 
	 uint8_t calc_derived_key_input[ATCA_MSG_SIZE_DERIVE_KEY] = { '\0'};	
	 uint8_t nonce_seed_input[NONCE_NUMIN_SIZE_PASSTHROUGH] = {0};		 
	 uint8_t nonce_seed[NONCE_NUMIN_SIZE] = {0};
	 uint8_t derived_key[ATCA_KEY_SIZE] = {0};
	 uint8_t challenge[CHECKMAC_CLIENT_CHALLENGE_SIZE] = {0};  
	 uint8_t sn[ATCA_SERIAL_NUM_SIZE]= {0};  
	
	 atca_temp_key_t temp_key_params;	
	 struct atca_gen_dig_in_out gen_dig_params;
	 struct atca_derive_key_in_out derivekey_params;
	
	 uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE] = {
		 ATCA_MAC, mac_mode, target_key_id, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };	 	 
	 status = atcab_random(nonce_seed_input);
	 if (status != ATCA_SUCCESS) { sha204_parser_rc(status); return status; }			 
	 memcpy(&nonce_seed[0], &nonce_seed_input[0], sizeof(nonce_seed));	 
	
	 status = atcab_random(challenge);
	 RET_STATUS(status);		
	
	 // this can be anything
	 uint8_t other_data_offset[CHECKMAC_CLIENT_COMMAND_SIZE] = { ATCA_DERIVE_KEY, 0x05, target_key_id, 0x00 };

	// calculate the parent key (0x02 - used for the encrypting the key
	status = sha204_create_diverse_key(rootkey, 0x02, parent_key, 0x77);
	RET_STATUS(status);
	printf("- Created Parent needed for Encrypt Write...\n\r");	
	// calculate the known original key 4 - this key is derived from the root
	status = sha204_create_diverse_key(rootkey, target_key_id, key_04, 0x77);
	RET_STATUS(status);
	printf("- Created Key %d\n\r", target_key_id);
	 // Read the device serial number
	status = atcab_read_serial_number(sn);
	RET_STATUS(status);
	printf("- Read Serial Number complete:\t");
	print_buffer(sn, sizeof(sn));
	
	// load the nonce for the temp key - used later
	memset(&nonce_in[0], 0x77, sizeof(nonce_in)); // we padded with 0x77 -
	memcpy(&nonce_in[0], &sn[0], sizeof(sn)); // replace first 9 bytes with sn	
	
	// Initialize the slot with a known key - this basically starts the key roll back at the beginning	
	status = atcab_write_enc(target_key_id, 0, key_04, parent_key, 0x02);
	RET_STATUS(status);
	printf("- Slot %d reset to original value: ", target_key_id);
	print_buffer(key_04, 32);
   
	// create the mac required for derive key SlotConfig<TargetKey>.Bit15 == 1
	struct atca_derive_key_mac_in_out derivekey_mac_params;
	derivekey_mac_params.mode = DERIVE_KEY_MODE; // 0x04
	derivekey_mac_params.target_key_id = target_key_id;
	derivekey_mac_params.sn = sn;
	derivekey_mac_params.parent_key = parent_key;
	derivekey_mac_params.mac = mac;
	status = atcah_derive_key_mac(&derivekey_mac_params);
	RET_STATUS(status);
	printf("- Mac required for derived key complete....\n\r");
	
	memset(&temp_key_params, 0, sizeof(temp_key_params));  
	// Calculate the derived key
	temp_key_params.source_flag = 1;
	temp_key_params.valid = 1;
	memcpy(&temp_key_params.value[0], &nonce_in[0], sizeof(nonce_in));	
	derivekey_params.mode = DERIVE_KEY_MODE; // Random nonce generated TempKey // 0x04
	derivekey_params.target_key_id = target_key_id;
	derivekey_params.parent_key = key_04;
	derivekey_params.sn = sn;
	derivekey_params.target_key = derived_key;
	derivekey_params.temp_key = &temp_key_params;	 	
	status = atcah_derive_key(&derivekey_params); 	 	
	RET_STATUS(status);
		
	printf("- Calculating the derived key complete...\n\r");  
	printf("- Calculated Derived Key (Roll Key):");	
	print_buffer(derived_key, 32); 
	 
	// how to manually calculate the key - calculate the new derived key - only leaving for tutorial
	memset(&calc_derived_key_input, 0, sizeof(calc_derived_key_input));
	memcpy(&calc_derived_key_input[0], key_04, sizeof(parent_key));
	calc_derived_key_input[32] = ATCA_DERIVE_KEY;
	calc_derived_key_input[33] = DERIVE_KEY_MODE; // param 1 0x04
	calc_derived_key_input[34] = (uint8_t)target_key_id; // param 2
	calc_derived_key_input[35] = 0x00;
	calc_derived_key_input[36] = sn[8];
	calc_derived_key_input[37] = sn[0];
	calc_derived_key_input[38] = sn[1];
	memcpy(&calc_derived_key_input[64], &temp_key_params.value[0], 32);
	status = atcac_sw_sha2_256(calc_derived_key_input, sizeof(calc_derived_key_input), test_key);
	RET_STATUS(status);
		
	 //create the key with the deriveKey command
	status=sha204_diverse_key_example(target_key_id, mac, 0x77);
	RET_STATUS(status);
	 		
	// load the derived key into the temp key register (pass-through)
	status = atcab_nonce_load(NONCE_MODE_PASSTHROUGH, derived_key, sizeof(derived_key)); // load the temp key with sn and random bytes - pass through		 
	RET_STATUS(status);
		
	// build mac for checkmac 	
	memset(&mac_param[0], 0, sizeof(mac_param));
	memcpy(&mac_param[0], &derived_key[0], 32);
	memcpy(&mac_param[32], &challenge[0], 32);		
	
	mac_param[64] = other_data[0];
	mac_param[65] = other_data[1];
	mac_param[66] = other_data[2];
	mac_param[67] = other_data[3];	
	mac_param[76] = other_data[4];
	mac_param[77] = other_data[5];
	mac_param[79] = other_data[6];
	mac_param[79] = sn[8];
	mac_param[80] = other_data[7];
	mac_param[81] = other_data[8];
	mac_param[82] = other_data[9];
	mac_param[83] = other_data[10];
	mac_param[84] = sn[0];
	mac_param[85] = sn[1];
	mac_param[86] = other_data[11];
	mac_param[87] = other_data[12];
	
	status = atcac_sw_sha2_256(mac_param, sizeof(mac_param), mac_out);
	RET_STATUS(status);	
	
	// Run the checkmac command to validate the calculated derived key	-
	
	status = atcab_checkmac(mac_mode, target_key_id, challenge, mac_out, other_data);
		
	RET_STATUS(status);
	printf("- Simple validation complete... now try a gendig\n\r");	
	
	status = atcab_nonce_load(NONCE_MODE_PASSTHROUGH, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through	
	RET_STATUS(status);
	printf("- TempKey loaded with a Pass-Through Nonce...\n\r");	
	
	// create the temp key in the client tempkey register		
	status = atcab_gendig(GENDIG_ZONE_DATA, target_key_id, other_data_offset, sizeof(other_data_offset));
	RET_STATUS(status);
	printf("- GenDig complete. The calculated digest has been loaded into the TempKey Register...\n\r");	
	// calculated the temp key on the host - this should match what the gendig command created 	
	gen_dig_params.zone = ATCA_ZONE_DATA;
	gen_dig_params.key_id = target_key_id;
	gen_dig_params.stored_value = derived_key;
	gen_dig_params.sn = sn;
	gen_dig_params.temp_key = &temp_key_params;
	gen_dig_params.other_data = other_data_offset;
	gen_dig_params.is_key_nomac = true;
	status = atcah_gen_dig(&gen_dig_params);
	printf("- Calculated the digest on the host - this should match what the gendig command created...\n\r");		
	// now calculate the mac with the tempkey.	
	memset(&gen_dig_params, 0, sizeof(gen_dig_params));
	struct atca_check_mac_in_out test_mac_host;
	test_mac_host.mode = 0x06;
	test_mac_host.key_id = target_key_id;
	test_mac_host.client_chal = challenge;
	test_mac_host.client_resp = mac_out;
	test_mac_host.target_key = derived_key;
	test_mac_host.sn = sn;
	test_mac_host.temp_key = &temp_key_params;
	test_mac_host.other_data = other_data_offset;	
	status = atcah_check_mac(&test_mac_host);
	RET_STATUS(status);				
	printf("- Created mac complete. Now checking the digest internally with checkmac command...\n\r");
	status = atcab_checkmac(0x06, target_key_id, challenge, mac_out, other_data_offset);
	RET_STATUS(status);
			
	return status;
}

/*


SlotConfig<TargetKey>.Bit13 must be set or DeriveKey returns an error.
If SlotConfig<TargetKey>.Bit12 is zero, the source key that is combined with TempKey is the target key
specified in the command line (Roll-Key operation).

*/
uint8_t sha204_calculated_rolled_key(uint8_t *root, uint8_t key_id, uint8_t roll_count, uint8_t *derived_key){
	
	status = ATCA_SUCCESS;		
	uint8_t config_data[32] = {0};
	uint8_t rolling_key[ATCA_KEY_SIZE] = {0};
	uint8_t temporary_key[ATCA_KEY_SIZE] = {0};	// used to keep track of the key as its rolled
	uint8_t nonce_in[NONCE_NUMIN_SIZE_PASSTHROUGH] = {0};
	uint8_t sn[ATCA_SERIAL_NUM_SIZE]= {0};  
	uint8_t mac_out[MAC_CHALLENGE_SIZE] = {0};		
	atca_temp_key_t temp_key_params;	
	struct atca_derive_key_in_out derivekey_params;	
	struct atca_gen_dig_in_out gen_dig_params;
	uint8_t challenge[CHECKMAC_CLIENT_CHALLENGE_SIZE] = {0};  
	 // this can be anything
	uint8_t other_data_offset[CHECKMAC_CLIENT_COMMAND_SIZE] = { ATCA_DERIVE_KEY, 0x05, key_id, 0x00 };

	// first check if it is a derived key
	status = sha204_get_slot_config(config_data);
	RET_STATUS(status);		
	printf("- Config Data: ");
	print_buffer(config_data, sizeof(config_data));
	uint8_t idx = key_id * 2;
	printf("- Key %d config: 0x%02X %02X\n\r", key_id, config_data[idx], config_data[idx+1]);	
	
	uint8_t write_config_12 = ((config_data[idx] >> 4)  & 0x01);
	uint8_t write_config_13 = ((config_data[idx] >> 5)  & 0x01);
	uint8_t write_config_14 = ((config_data[idx] >> 6)  & 0x01);
	uint8_t write_config_15 = ((config_data[idx] >> 7)  & 0x01);

	printf("- SlotConfig<%d>.15 = %d\n\r", key_id,write_config_15);
	printf("- SlotConfig<%d>.14 = %d\n\r", key_id,write_config_14);
	printf("- SlotConfig<%d>.13 = %d\n\r", key_id,write_config_13);
	printf("- SlotConfig<%d>.12 = %d\n\r", key_id, write_config_12);	
	if(write_config_13 != 1 || write_config_12 != 0){
		printf("** SlotConfig<%d>.12 must be 0 and SlotConfig<%d>.13 must be 1 to run this function\n\r", key_id, key_id);	
		return ATCA_GEN_FAIL;
	} 
	printf("-SlotConfig<%d> is configured correctly...\n\r", key_id);			
	printf("-Mac required for derived key? %s\n\r", write_config_15 == 1 ? "Yes" : "No");
	
	status = atcab_random(challenge);
	RET_STATUS(status);
	
	status = atcab_read_serial_number(sn);
	RET_STATUS(status);
	
	memset(&nonce_in[0], 0x77, sizeof(nonce_in)); // we padded with 0x77 -
	memcpy(&nonce_in[0], &sn[0], sizeof(sn)); // replace first 9 bytes with sn	
	
	// calculate the original key which was derived form the root	
	status = sha204_create_diverse_key(root, key_id, rolling_key, 0x77);
	RET_STATUS(status);
	printf("- Created Key %d:\t", key_id);
	print_buffer(rolling_key, sizeof(rolling_key));
	
	// not we check how many times the key was rolled in this session. 
	if(roll_count == 0){
		printf("** Key was never rolled...\n\r");		
		return status;
	} else {			
		// start to roll the key
		for(int i = 0; i < roll_count; i++){
						
			memset(&temp_key_params, 0, sizeof(temp_key_params)); // reset
			// Calculate the derived key
			temp_key_params.source_flag = 1;
			temp_key_params.valid = 1;
			memcpy(&temp_key_params.value[0], &nonce_in[0], sizeof(nonce_in));
			derivekey_params.mode = DERIVE_KEY_MODE; // Random nonce generated TempKey // 0x04
			derivekey_params.target_key_id = key_id;
			derivekey_params.parent_key = rolling_key; // last rolled key
			derivekey_params.sn = sn;
			derivekey_params.target_key = temporary_key;
			derivekey_params.temp_key = &temp_key_params;
			status = atcah_derive_key(&derivekey_params);
			RET_STATUS(status);		
			
			printf("- Calculated Derived Key (Key was rolled %d times):\t", i+1);
			print_buffer(temporary_key, 32);
			memcpy(&derived_key[0], &temporary_key[0], sizeof(temporary_key));
			memcpy(&rolling_key[0], &temporary_key[0], sizeof(temporary_key));		
		}
		
		printf("- Calculated Final Key:\t");
		print_buffer(derived_key, 32);
		
		// now we validate the rolled key
		status = atcab_nonce_load(NONCE_MODE_PASSTHROUGH, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through
		RET_STATUS(status);
		printf("-TempKey loaded with a Pass-Through Nonce...\n\r");
		
		// create the temp key in the client tempkey register
		status = atcab_gendig(GENDIG_ZONE_DATA, key_id, other_data_offset, sizeof(other_data_offset));
		RET_STATUS(status);
		printf("-GenDig complete. The calculated digest has been loaded into the TempKey Register...\n\r");
		// calculated the temp key on the host - this should match what the gendig command created
		gen_dig_params.zone = ATCA_ZONE_DATA;
		gen_dig_params.key_id = key_id;
		gen_dig_params.stored_value = derived_key;
		gen_dig_params.sn = sn;
		gen_dig_params.temp_key = &temp_key_params;
		gen_dig_params.other_data = other_data_offset;
		gen_dig_params.is_key_nomac = true;
		status = atcah_gen_dig(&gen_dig_params);
		RET_STATUS(status);
		printf("- Calculated the digest on the host - this should match what the gendig command created...\n\r");
		// now calculate the mac with the tempkey.
		memset(&gen_dig_params, 0, sizeof(gen_dig_params));
		struct atca_check_mac_in_out test_mac_host;
		test_mac_host.mode = 0x06;
		test_mac_host.key_id = key_id;
		test_mac_host.client_chal = challenge;
		test_mac_host.client_resp = mac_out;
		test_mac_host.target_key = derived_key;
		test_mac_host.sn = sn;
		test_mac_host.temp_key = &temp_key_params;
		test_mac_host.other_data = other_data_offset;
		status = atcah_check_mac(&test_mac_host);
		RET_STATUS(status);
		
		printf("- Created mac complete. Now checking the digest internally with checkmac command...\n\r");
		status = atcab_checkmac(0x06, key_id, challenge, mac_out, other_data_offset);
		RET_STATUS(status);
		
    };
	return status;
	
}

uint8_t sha204_get_slot_config(uint8_t* config_data){
	
	status = ATCA_SUCCESS;
	uint8_t data[4];	
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t slot = 0;
	uint8_t ct = 0;
	for(offset = 5; offset < 8; offset++){
		status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		RET_STATUS(status);
		config_data[ct] =  data[1];
		config_data[ct+1] =  data[0];
		config_data[ct+2] =  data[3];
		config_data[ct+3] =  data[2];		
		slot+=2; 
		ct+=4;		
	}
	
	block = 1;
	for(offset = 0; offset < 5; offset++){
		status = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		RET_STATUS(status);
		config_data[ct] =  data[1];
		config_data[ct+1] =  data[0];
		config_data[ct+2] =  data[3];
		config_data[ct+3] =  data[2];		
		slot+=2;
		ct+=4;
	}
	return status;	
}

uint8_t sha204_validate_derived_key(uint8_t *root, uint8_t key_id, uint8_t padding, uint8_t *derived_key){
	
	status = ATCA_SUCCESS;
	uint8_t config_data[32] = {0};
	uint8_t rolling_key[ATCA_KEY_SIZE] = {0};
	uint8_t parent_key[ATCA_KEY_SIZE] = {0};	// used to keep track of the key as its rolled
	uint8_t nonce_in[NONCE_NUMIN_SIZE_PASSTHROUGH] = {0};
	uint8_t sn[ATCA_SERIAL_NUM_SIZE]= {0};
	uint8_t mac_out[MAC_CHALLENGE_SIZE] = {0};
	uint8_t parent_key_id  = 0; // default
	atca_temp_key_t temp_key_params;
	struct atca_derive_key_in_out derivekey_params;
	struct atca_gen_dig_in_out gen_dig_params;
	uint8_t challenge[CHECKMAC_CLIENT_CHALLENGE_SIZE] = {0};
	// this can be anything
	uint8_t other_data_offset[CHECKMAC_CLIENT_COMMAND_SIZE] = { ATCA_DERIVE_KEY, 0x05, key_id, 0x00 };

	// first check if it is a derived key
	status = sha204_get_slot_config(config_data);
	RET_STATUS(status);
	print_buffer(config_data, sizeof(config_data));
	uint8_t idx = key_id * 2;
	printf("Key %d config: 0x%02X %02X\n\r", key_id, config_data[idx], config_data[idx+1]);
	
	uint8_t write_config_12 = ((config_data[idx] >> 4)  & 0x01);
	uint8_t write_config_13 = ((config_data[idx] >> 5)  & 0x01);
	uint8_t write_config_14 = ((config_data[idx] >> 6)  & 0x01);
	uint8_t write_config_15 = ((config_data[idx] >> 7)  & 0x01);

	printf("SlotConfig<%d>.15 = %d\n\r", key_id,write_config_15);
	printf("SlotConfig<%d>.14 = %d\n\r", key_id,write_config_14);
	printf("SlotConfig<%d>.13 = %d\n\r", key_id,write_config_13);
	printf("SlotConfig<%d>.12 = %d\n\r", key_id, write_config_12);	
	
	if(write_config_13 != 1 ){
		printf("SlotConfig<%d>.13 must be 1 to run this function\n\r", key_id);
		return ATCA_GEN_FAIL;
	}
	printf("SlotConfig<%d>.13 is configured correctly...\n\r", key_id);	
	parent_key_id = config_data[idx] & 0x06; // get lower to bits of byte, which is the parent key
	
	printf("SlotConfig<5>.12 = 1 uses the parent key %d to calculate the derived key.\n\r", parent_key_id);
	
	status = sha204_create_diverse_key(root, parent_key_id, parent_key, padding);
	RET_STATUS(status);
	
	status = atcab_random(challenge);
	RET_STATUS(status);
	
	status = atcab_read_serial_number(sn);
	RET_STATUS(status);
	
	memset(&nonce_in[0], 0x77, sizeof(nonce_in)); // we padded with 0x77 -
	memcpy(&nonce_in[0], &sn[0], sizeof(sn)); // replace first 9 bytes with sn
	
	// calculate the original key which was derived form the root
	status = sha204_create_diverse_key(root, key_id, rolling_key, 0x77);
	RET_STATUS(status);
	printf("Created Key %d: ", key_id);
	print_buffer(rolling_key, sizeof(rolling_key));	
			
	memset(&temp_key_params, 0, sizeof(temp_key_params)); // reset
	// Calculate the derived key
	temp_key_params.source_flag = 1;
	temp_key_params.valid = 1;
	memcpy(&temp_key_params.value[0], &nonce_in[0], sizeof(nonce_in));
	derivekey_params.mode = DERIVE_KEY_MODE; // Random nonce generated TempKey // 0x04
	derivekey_params.target_key_id = key_id;
	derivekey_params.parent_key = parent_key; // SlotConfig<5>.12 = 1 use the parentkey
	derivekey_params.sn = sn;
	derivekey_params.target_key = derived_key;
	derivekey_params.temp_key = &temp_key_params;
	status = atcah_derive_key(&derivekey_params);
	RET_STATUS(status);
			
	printf("Calculated Derived Key (Roll Key):");
	print_buffer(derived_key, 32);
		
	// now we validate the rolled key
	status = atcab_nonce_load(NONCE_MODE_PASSTHROUGH, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through
	RET_STATUS(status);
	printf("TempKey loaded with a Pass-Through Nonce...\n\r");
		
	// create the temp key in the client tempkey register
	status = atcab_gendig(GENDIG_ZONE_DATA, key_id, other_data_offset, sizeof(other_data_offset));
	RET_STATUS(status);
	printf("GenDig complete. The calculated digest has been loaded into the TempKey Register...\n\r");
	// calculated the temp key on the host - this should match what the gendig command created
	gen_dig_params.zone = ATCA_ZONE_DATA;
	gen_dig_params.key_id = key_id;
	gen_dig_params.stored_value = derived_key;
	gen_dig_params.sn = sn;
	gen_dig_params.temp_key = &temp_key_params;
	gen_dig_params.other_data = other_data_offset;
	gen_dig_params.is_key_nomac = true;
	status = atcah_gen_dig(&gen_dig_params);
	RET_STATUS(status);
	printf("Calculated the digest on the host - this should match what the gendig command created...\n\r");
	// now calculate the mac with the tempkey.
	memset(&gen_dig_params, 0, sizeof(gen_dig_params));
	struct atca_check_mac_in_out test_mac_host;
	test_mac_host.mode = 0x06;
	test_mac_host.key_id = key_id;
	test_mac_host.client_chal = challenge;
	test_mac_host.client_resp = mac_out;
	test_mac_host.target_key = derived_key;
	test_mac_host.sn = sn;
	test_mac_host.temp_key = &temp_key_params;
	test_mac_host.other_data = other_data_offset;
	status = atcah_check_mac(&test_mac_host);
	RET_STATUS(status);
		
	printf("Created mac complete. Now checking the digest internally with checkmac command...\n\r");
	status = atcab_checkmac(0x06, key_id, challenge, mac_out, other_data_offset);
	
	return status;
	
}
/** \brief	This example shows how a mac is generated on a host and validated on the client
			with a gendig/checkmac. Because the host knows the root key, it can generate the 
			mac the client is expecting. The mac is sent to the client, where it generates 
			a mac with the gendig command and validates it with thr checkmac. 				
 
 *  \param[in] root			pointer to the root key
 *  \param[in] key_id		Slot to run gendig/checkmac against
 *  \param[in] mac_key		Slot to run MAC against
 *  \param[in] padding		padding used to create keys
 
 *  \return					ATCA_SUCCESS on success, otherwise an error code.
 */
uint8_t sha204_validate_key_gendig(uint8_t *root, uint16_t key_id, uint16_t mac_key, uint8_t padding){
		
	status = ATCA_SUCCESS;
	uint8_t config_data[32] = {0};
	uint8_t otp_data[32] = {0};
	uint8_t serialNumber[9] = { '\0'};
	uint8_t mac[32] = { '\0'};
	uint8_t checkmac_mode = CHECKMAC_MODE_BLOCK1_TEMPKEY | CHECKMAC_MODE_SOURCE_FLAG_MATCH | CHECKMAC_MODE_INCLUDE_OTP_64;
	uint8_t challenge[32] = { '\0'};
	
	uint8_t derived_key[32] = { '\0'};
	uint8_t mac_command_data[13] = {'\0'};
	uint8_t nonce_in[32]= { '\0'};
	uint8_t root_key_id = 0x0A; // root key	
	uint8_t mac_mode = MAC_MODE_CHALLENGE | MAC_MODE_INCLUDE_SN | MAC_MODE_INCLUDE_OTP_64; // include SN	
	struct atca_temp_key temp_key;
	
	status = sha204_get_slot_config(config_data);
	RET_STATUS(status);
	
	uint8_t idx = key_id * 2;
	printf("-Key %d config: 0x%02X %02X\n\r", key_id, config_data[idx], config_data[idx+1]);
	
	uint8_t write_config_4 = ((config_data[idx+1] >> 4)  & 0x01);
	uint8_t write_config_5 = ((config_data[idx+1] >> 5)  & 0x01);
	uint8_t write_config_6 = ((config_data[idx+1] >> 6)  & 0x01);
	uint8_t write_config_7 = ((config_data[idx+1] >> 7)  & 0x01);

	printf("-SlotConfig<%d>.4 = %d\n\r", key_id,write_config_4);
	printf("-SlotConfig<%d>.5 = %d\n\r", key_id,write_config_5);
	printf("-SlotConfig<%d>.6 = %d\n\r", key_id,write_config_6);
	printf("-SlotConfig<%d>.7 = %d\n\r", key_id, write_config_7);
			
	status = sha204_create_diverse_key(root, key_id, derived_key, padding);
	RET_STATUS(status);
		
	// this is the command used to create the client's key - used to create the ephemeral key in gendig	
	uint8_t derive_key_command[4] = {	// this is command used by derivekey command to create the diverse key for target slot
		ATCA_DERIVE_KEY,				// opcode derivekey
		DERIVE_KEY_MODE,				// mode param1 - The value of this bit must match the value in TempKey.SourceFlag or the command returns an error.
		key_id, 0x00					// param2 target key (upper byte)
	}; // 1C 04 key_id 00
	
	memset(&mac_command_data, 0, sizeof(mac_command_data));
	mac_command_data[0] = ATCA_MAC;	//  0x08 other data is the command to create the mac on the target
	mac_command_data[1] = mac_mode;	//	0x00 mode used to create mac
	mac_command_data[2] = mac_key;	//  0x0A	// key used to create mac - upper byte only
	mac_command_data[3] = 0x00;		//  0x00 key used to create mac - always zero
	
	printf("- Reading SN from client\n\r");
	status = atcab_read_serial_number(serialNumber); // get the serial number
	RET_STATUS(status);
	printf("- SN received from client\n\r");
	
	// see datasheet Table 8-10 OtherData
	mac_command_data[7] = serialNumber[4];
	mac_command_data[8] = serialNumber[5];
	mac_command_data[9] = serialNumber[6];
	mac_command_data[10] = serialNumber[7];
	mac_command_data[11] = serialNumber[2];
	mac_command_data[12] = serialNumber[3];	
	
	//opt data
	status = sha204_read_otp_zone(32, 0, otp_data); // read the first 32 bytes of the otp zone
	RET_STATUS(status);

	printf("- Requesting a random number for challenge.\n\r");
	status = atcab_random(challenge);
	RET_STATUS(status);
	printf("- Random number for challenge received\n\r");
		
	struct atca_mac_in_out mac_params;	
	
	if(write_config_4 == 1){ // check only is set. HW Mac cannot be run against it.
		printf("- Creating mac on host\n\r");
		mac_params.mode =mac_mode;
		mac_params.key_id = mac_key;
		mac_params.key = derived_key;
		mac_params.challenge = challenge;
		mac_params.response = mac;
		mac_params.sn =serialNumber;
		mac_params.otp = otp_data;
		mac_params.temp_key = &temp_key;
		status = atcah_mac(&mac_params);		
	} else {
		// this is for testing. For security, set SlotConfig<%d>.4 = 1 (CheckOnly)
		printf("- Creating mac on client\n\r");
		status = atcab_mac(mac_mode, mac_key, challenge, mac);
		RET_STATUS(status);			
	}	
	
	printf("************************************************\n\r");
	printf("- Other Data:\t\t");
	print_raw_buffer(mac_command_data, sizeof(mac_command_data));
	printf("- DerivedKey Command:\t");
	print_raw_buffer(derive_key_command, sizeof(derive_key_command));
	printf("- Challenge: \t\t");
	print_raw_buffer(challenge, sizeof(challenge));		
	printf("- MAC:\t\t\t");	
	print_raw_buffer(mac, sizeof(mac));
	printf("************************************************\n\r");
	
	// Host events
	memset(&nonce_in, padding, sizeof(nonce_in));
	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn
	
	printf("- Creating the TempKey via the nonce command (pass-through) on the host...loading the root key and client's sn\n\r");
	
	status = atcab_nonce_load(root_key_id, nonce_in, sizeof(nonce_in)); // load the temp key with sn and PADDING
	RET_STATUS(status);
		
	printf("- Creating the digest internally on the client (gendig)\n\r");
	// execute a gendig on key 10 - master key stored on slot 10 - this will create the digest for the checkmac
	status = atcab_gendig(GENDIG_ZONE_DATA, root_key_id, derive_key_command, sizeof(derive_key_command));
	RET_STATUS(status);
		
	printf("- Checking the digest internally on the client (checkmac) with the mac generated by the host\n\r");
	status = atcab_checkmac(checkmac_mode, key_id, challenge, mac, mac_command_data);

	RET_STATUS(status);
	
	printf("- Checkmac (Mode: 0x%02X)/GenDig success!\n\r", checkmac_mode);
	
	return status;
	
}
