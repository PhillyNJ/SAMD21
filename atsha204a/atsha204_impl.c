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
	 volatile uint8_t ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		 sha204_parser_rc(ret);
		 return ret;
	 }
	 memcpy(&details->serial_number[0], &data, 4);
	 //sha204_print_buffer(data, 4);
	 offset = 1;
	 ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		 sha204_parser_rc(ret);
		 return ret;
	 }
	 memcpy(&details->rev_num[0], &data, 4);
	 offset = 2;
	 
	 ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		 sha204_parser_rc(ret);
		 return ret;
	 }
	 memcpy(&details->serial_number[4], &data, 4);
	 offset = 3;
	 ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		 sha204_parser_rc(ret);
		 return ret;
	 }
	 // get CheckMacConfig and i2c setting	
	 offset = 4;
	 ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		  sha204_parser_rc(ret);
		  return ret;
	  }
	  
	 details->i2c_address = data[0];
	 details->check_mac_config = data[1];
	 details->otp_mode = data[2];
	 details->select_mode = data[3];
	 
	 
	 memcpy(&details->serial_number[8], &data, 1);
	 // get lock and lockConfig
	 block = 2;
	 offset = 5;
	 ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	 if(ret != ATCA_SUCCESS){
		 sha204_parser_rc(ret);
		 return ret;
	 }
	 details->lock_value = data[2];
	 details->lock_config = data[3];

	 return ret;
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
	 
	 printf("\tWrite Key: %d Read Key: %d Secret: %s Encrypt Read: %s", (lower & 0x0F), (hi >> 4), (is_secret == 1) ? "Yes" : "No", (encrypt_read == 1 && is_secret == 1) ? "Yes" : "No");
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

	uint8_t ret = 0;
	uint8_t data[4];
	uint8_t data_display[4];
	uint8_t block = 0;
	uint8_t offset = 0;
    uint8_t slot = 0;
	
	for(offset = 5; offset < 8; offset++){

		ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		if(ret != ATCA_SUCCESS){
			sha204_parser_rc(ret);
			return ret;
		}
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
		ret = atcab_read_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
		if(ret != ATCA_SUCCESS){
			sha204_parser_rc(ret);
			return ret;
		}
		data_display[0] = data[1];
		data_display[1] = data[0];
		data_display[2] = data[3];
		data_display[3] = data[2];
		sha204_parse_config_section(slot, data_display[1],data_display[0]);
		slot++;
		sha204_parse_config_section(slot, data_display[3],data_display[2]);
		slot++;
	}
	return ret;
}
uint8_t sha204_personalize(void){
	
	uint8_t ret = 0;
	uint8_t data[4];
	uint8_t block = 0;
	uint8_t offset = 5;
	// Slots 0 & 1
	data[0] = 0x8F; // LSB on spreadsheet
	data[1] = 0x8F;
	data[2] = 0x89; // LSB on spreadsheet
	data[3] = 0xF2;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		sha204_parser_rc(ret);
		return ret;
	}

	// Slots 2 & 3
	offset = 6;
	data[0] = 0x89;
	data[1] = 0xF2;
	data[2] = 0x89;
	data[3] = 0xF2;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

	// Slots 4 & 5
	offset = 7;
	data[0] = 0x89;
	data[1] = 0xF2;
	data[2] = 0x99; // key 5
	data[3] = 0xB2;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

	// Slots 6 & 7
	block = 1;
	offset = 0;
	data[0] = 0x89;
	data[1] = 0x8F;
	data[2] = 0xDA;
	data[3] = 0x3B;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}
	// Slots 8 & 9
	offset = 1;
	data[0] = 0xCA;
	data[1] = 0x4A;
	data[2] = 0x9F;
	data[3] = 0x3B;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

	// Slots 10 & 11
	offset = 2;
	data[0] = 0x9F;
	data[1] = 0x8F;
	data[2] = 0x8F; // second root key for checkmac
	data[3] = 0x8F;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

	// Slots 12 & 13
	offset = 3;
	data[0] = 0xCA;
	data[1] = 0x4A;
	data[2] = 0xCA;
	data[3] = 0x4A;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

	// Slots 14 & 15
	offset = 4;
	data[0] = 0x0F;
	data[1] = 0x00;
	data[2] = 0xCD;
	data[3] = 0x4D;
	// write to each slot config
	ret = atcab_write_zone(ATCA_ZONE_CONFIG,0,block,offset,data, 4);
	if(ret != ATCA_SUCCESS){
		printf("Error: 0x%02x\n\r", ret);
		return ret;
	}

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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to read from OTP: \n\r");
		return status;
	}

	return status;
}
/** \brief Creates the diverse keys for the demo. The keys are diversified keys, root key to the ATSHA204a
 *	keys 0-9, 12-15 are diversified. The root key is saved to slot 10 & 11 for demo
 * \param[in] root   pointer to the root key 
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

uint8_t sha204_write_keys(uint8_t *root){
	
	uint8_t data[32] = {0};
	uint8_t padding = 0x77;
	status = ATCA_SUCCESS;
		
	// write keys (slots)
	status = sha204_get_details(&myDetails);
	if(status != ATCA_SUCCESS){
		printf("Unable to read device: ");
		return status;
	}
	if(myDetails.lock_config != 0x00){
		printf("Lock Config Must be Locked to perform function\n\r");
		return status;
	}
	memset(&data, 0, sizeof(data));
   
	// the rest of the keys are diversified 0-5
	for(uint8_t i = 0; i < 6; i++){
		
		status= sha204_create_diverse_key(root, i, data, padding);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to calculate key ");
			return status;
		}
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to write to slot %d: ", i);
			return status;
		}
		printf("Diverse Key 0x%02X: ", i);
		print_buffer(data, 32);
	}
	
	// create the key for slot 7  (host and save in slot 6 client)
	status= sha204_create_diverse_key(root, 7, data, padding);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to calculate key 7\n\r");
		return status;
	}
	//save to slot 7
	status = atcab_write_zone(ATCA_ZONE_DATA, 7, 0, 0, data, 32);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to write to slot 7\n\r");
		return status;
	}
	printf("Diverse Key 0x%02X: ", 7);
	print_buffer(data, 32);
	// save the key in slot 6 client
	status = atcab_write_zone(ATCA_ZONE_DATA, 6, 0, 0, data, 32);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to write to slot 6\n\r ");
		return status;
	}
	printf("Diverse Key 0x%02X: ", 6);
	print_buffer(data, 32);
	
	// slot 8 & 9
	for(uint8_t i = 8; i < 10; i++){
		
		status= sha204_create_diverse_key(root, i, data, padding);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to calculate key \n\r");
			return status;
		}
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to write to slot %d:\n\r ", i);
			return status;
		}
		printf("Diverse Key 0x%02X: ", i);
		print_buffer(data, 32);
	}
	
	// the rest of the keys are diversified 12-14
	for(uint8_t i = 12; i < 16; i++){
			
		status= sha204_create_diverse_key(root, i, data, padding);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to calculate key %d\n\r", i);
			return status;
		}
		status = atcab_write_zone(ATCA_ZONE_DATA, i, 0, 0, data, 32);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to write to slot %d:\n\r ", i);
			return status;
		}
		printf("Diverse Key 0x%02X: ", i);			
		print_buffer(data, 32);
	}
	
	 //  write the root key for demo - Slot 10
	 status = atcab_write_zone(ATCA_ZONE_DATA, 10, 0, 0, root, ATCA_KEY_SIZE);
	 if (status != ATCA_SUCCESS)
	 {
		 printf("Unable to write to slot 10\n\r ");
		 return status;
	 }
	 printf("Root Key written to slot 10\n\r");
	 
	 status = atcab_write_zone(ATCA_ZONE_DATA, 11, 0, 0, root, ATCA_KEY_SIZE);
	 if (status != ATCA_SUCCESS)
	 {
		 printf("Unable to write to slot 11: ");
		 return status;
	 }
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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		return status;
	}
	data[34] = (uint8_t)target; // param 2 (target key)
	data[35] = 0x00;			// param 2

	data[36] = sn[8];
	data[37] = sn[0];
	data[38] = sn[1];

	memcpy(&data[64], sn, sizeof(sn));
	memset(&data[73], padding, 24);

	status = atcac_sw_sha2_256(data, sizeof(data), diverse_key);
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		return status;
	}
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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to write to OTP: \n\r");
		return status;
	}

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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to read serial number: \n\r");
		return status;
	}

	status = atcab_read_bytes_zone(ATCA_ZONE_OTP, 0, 0, otp, ATCA_OTP_SIZE);
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to read atcab_read_bytes_zone: \n\r");
		return status;
	}

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
		if (status != ATCA_SUCCESS)
		{
			sha204_parser_rc(status);
			printf("Unable to create random nonce: \n\r");
			return status;
		}

		status = atcah_nonce(&nonce_params);
		if (status != ATCA_SUCCESS)
		{
			sha204_parser_rc(status);
			printf("Unable to create nonce: \n\r");
			return status;
		}

		hmac_params.mode = modes[i];
		hmac_params.key_id = slot;
		hmac_params.key = key;
		hmac_params.otp = otp;
		hmac_params.sn = sn;
		hmac_params.response = hmac_digest_host;
		hmac_params.temp_key = &temp_key;
		status = atcab_hmac(hmac_params.mode, hmac_params.key_id, hmac_digest); // hardware
		if (status != ATCA_SUCCESS)
		{
			sha204_parser_rc(status);
			printf("Unable to atcab_hmac: \n\r");
			return status;
		}

		//Calculate hmac on host - software
		status = atcah_nonce(&nonce_params);
		if (status != ATCA_SUCCESS)
		{
			sha204_parser_rc(status);
			printf("Unable to atcah_nonce: \n\r");
			return status;
		}

		status = atcah_hmac(&hmac_params); // host
		if (status != ATCA_SUCCESS)
		{
			sha204_parser_rc(status);
			printf("Unable to atcah_hmac: \n\r");
			return status;
		}

		//Compare Chip and Host calculations
		if(memcmp (hmac_digest, hmac_params.response, sizeof(hmac_digest))== 0){
			printf("Equal for Mode 0x%02x\n\r",modes[i]);
			} else {
			printf("Not Equal for Mode 0x%02x\n\r",modes[i]);
		}
	}
	
	return status;
}


/*
	Based on the example in the application note: 2.3 Validate Using the GenDig Command 
	
	This demonstrates how a challenge is done with 2 devices where the host and client do not share keys. 
	
	The host has no knowledge of the clients keys, however the host knows the root key and the client's keys where derived form the root when the 
	keys where written to the client.
	
	This provides a very secure way to authenticate a client without knowing the clients keys. The only data exposed is the client's MAC and SN.
	
	For Example, a host could have many clients like a printer (host) and print cartridges (client).
	
	How it works:
	- The host stores the root key. In this case, slot 10.
	- The client's keys were created using the root key, but the root key is not stored on the client.
	- The host reserves a slot for authenticating its clients.
	- When the host detects a client, we preform a gendig challange.
	
	The slot config for this example is set as follows for the this to run correctly:
	
	slot 0x00 - aka the client - Is a diverse key, created with root key. This slot	cannot be read, isSecret, 
				can run mac challenge against it - (checkonly bit is NOT set)
				
	slot 0x07 and 0x09 - aka the host - These slot are reserved for creating the ephemeral diverse key, created with root key which is also stored in slot 10.
				Slot 0x09 cannot be read, isSecret, cannot run challenge against it (checkonly bit is set), parent key is slot 10. The checkmac is run against this slot.
				Slot 0x07 enc read, isSecret, cannot run challenge against it (checkonly bit is set), parent key is slot 10. The checkmac is run against this slot.
				The ephemeral keys is create by looking at the targets parent, which is the root key. 
				
	slot 0x0A - Root Key, cannot be read, isSecret, cannot run software challenge against it. This key is shared between client and host. (checkonly bit is set)
	
	Steps: 
		1. The client's keys are diversified from the root key before locking
		2. The host requests the SN from the client	
		3. A random number is requested from the client to be used for the challenge
		4. A MAC command is requested from the client using the challenge from step 2. The digest will be used in step 7
		5. The host loads the root key and the clients serial number with the nonce command to create the TempKey
		6. The host generates a digest with the gendig command. The digest is created by generating the ephemeral key (e.g. the same key as client - slot 0 in this case).
		   The digest is generated internally, placed into the TempKey register and can never be read.
		7. The host calls the checkmac, which compares the MAC generated by the client and the gendig digest result now stored in TempKey register		
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
	mac_command_data[0] = ATCA_MAC;	// other data is the command to create the mac on the target
	mac_command_data[1] = mac_mode;		//mode used to create mac
	mac_command_data[2] = mac_key;		// key used to create mac - upper byte only
	mac_command_data[3] = 0x00;		    // key used to create mac - always zero	
	
	printf("- Reading SN from client\n\r");
	status = atcab_read_serial_number(serialNumber); // get the serial number
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Read Serial Number ");
		sha204_parser_rc(status);
		return status;
	}
	printf("- SN received from client\n\r");
	
	printf("- Requesting a random number for challenge.\n\r");
	status = atcab_random(challenge);
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Error! Unable to create random number!!\n\r");
		return status;
	}
	printf("- Random number for challenge received\n\r");	
   	
	printf("- Creating mac for client\n\r");	
	status = atcab_mac(mac_mode, mac_key, challenge, mac);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Generate HW Mac ");
		sha204_parser_rc(status);
		return status;
	}	
    // Host events
	memset(&nonce_in, padding, sizeof(nonce_in));
	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn
	
	printf("- Creating the TempKey via the nonce command (pass-through) on the host...loading the root key and client's sn\n\r");
   
	status = atcab_nonce_load(root_key, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to create load nonce ");
		sha204_parser_rc(status);
		return status;
	}
	printf("- Creating the digest internally on the host (gendig)\n\r");
	// execute a gendig on key 10 - master key stored on slot 10

	status = atcab_gendig(GENDIG_ZONE_DATA, root_key, derive_key_command, sizeof(derive_key_command));
	if (status != ATCA_SUCCESS)
	{
		printf("Unable Run GenDig ");
		sha204_parser_rc(status);
		return status;
	}
	printf("- Checking the digest internally on the host (checkmac) with the mac generated by the client\n\r");
	status = atcab_checkmac(0x06, target_key_id, challenge, mac, mac_command_data);	

	if (status != ATCA_SUCCESS)
	{
		printf("CheckMac Failed!");
		sha204_parser_rc(status);
		return status;
	}
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
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Read Serial Number ");
		sha204_parser_rc(status);
		return status;
	}

	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn
	
	status = atcab_nonce_load(root_key, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to create load nonce ");
		sha204_parser_rc(status);
		return status;
	}

	status = atcab_derivekey(DERIVE_KEY_MODE, target, mac);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to run derive key ");
		sha204_parser_rc(status);
		return status;
	}

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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to read serial number: \n\r");
		return status;
	}	
	
	// request a random number for the challenge
	status = atcab_random(challenge);
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to create random number!!\n\r");
		return status;
	}

	status = atcab_mac(mode, 0x00 + slot, challenge, digest);
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to create mac!!\n\r");
		return status;
	}
	
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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to create digest!!\n\r");
		return status;
	}	
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
	if (status != ATCA_SUCCESS)
	{
		sha204_parser_rc(status);
		printf("Unable to create mac!!\n\r");
		return status;
	}	
	
	printf("HW Digest: ");
	print_buffer(digest, sizeof(digest));
	
	return memcmp ( digest, sw_digest, sizeof(digest) );
}

/*
 This example checks the mac per the app note. The mac must be run against the diverse key slot,
 in this case slot 00. The mac is a sha256 hash against the diverse key. In the examples the diverse is in 
 slot 0 

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
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Read Serial Number ");
		sha204_parser_rc(status);
		return status;
	}
	status = atcab_random(challange);
	if(status != ATCA_SUCCESS){		
		sha204_parser_rc(status);
		return status;
	}	

	status = atcab_mac(0x00, mac_key, challange, mac);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Generate HW Mac ");
		sha204_parser_rc(status);
		return status;
	}
	printf("Generated Mac: ");
	 for(uint8_t i = 0; i < 32;i++){
		 printf("0x%02X ", mac[i]);
	 }
	 printf("\n\r");

	 // Generate random number for nonce
	for (uint8_t i = 0; i < 32; i++) {
		 nonce_in[i] = rand() % 0xFF;
	 }	
	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn

	status = atcab_nonce_load(0x03, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes
	if (status != ATCA_SUCCESS)
	{		
		printf("Unable to create load nonce ");
		sha204_parser_rc(status);
		return status;
	}
	

	status = atcab_checkmac(CHECKMAC_MODE_SOURCE_FLAG_MATCH, key_id, challange, mac ,mac_command_data);
	
	if (status != ATCA_SUCCESS)
	{
		printf("Unable Run CheckMac ");
		sha204_parser_rc(status);
		return status;
	}
	
	return status;
}

// (Hardware)
// The device combines the current value of a key with the nonce stored in TempKey using SHA-256 and
// places the result into the target key slot. SlotConfig<TargetKey>.Bit13 must be set or DeriveKey returns
// an error.
uint8_t sha204_diverse_key_example(uint16_t target, uint8_t *mac, uint8_t padding){
	
	status = ATCA_GEN_FAIL;
	uint8_t serialNumber[9] = { '\0'};
	
	uint8_t nonce_in[32]= { '\0'};
	memset(&nonce_in[0], padding, sizeof(nonce_in)); // we padded with 77 per the App note -
	status = atcab_read_serial_number(serialNumber); // get the serial number
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to Read Serial Number ");
		sha204_parser_rc(status);
		return status;
	}

	memcpy(&nonce_in[0], &serialNumber, sizeof(serialNumber)); // replace first 9 bytes with sn

	status = atcab_nonce_load(0x03, nonce_in, sizeof(nonce_in)); // load the temp key with sn and random bytes - pass through
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to create load nonce ");
		sha204_parser_rc(status);
		return status;
	}

	status = atcab_derivekey(DERIVE_KEY_MODE, target, mac);
	if (status != ATCA_SUCCESS)
	{
		printf("Unable to run derive key ");
		sha204_parser_rc(status);
		return status;
	}

	return status;
}

void sha204_compare_digests(uint8_t *dig1,uint8_t *dig2, int len){
	
	// compare hashes
	int n=memcmp ( dig1, dig2, len );

	if (n == 0){
		printf("\n\rDigests are the same!\n\r");
		} else {
		printf("\n\rDigests are the NOT the same!\n\r");
	}

}


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
	size_t i;
	// change this next time you run personalization!
	//uint8_t key_05[ATCA_KEY_SIZE] = {0x55, 0x55, 0x86, 0xF2, 0xB3, 0x20, 0x98, 0xA6, 0xE1, 0xE6, 0x33, 0x7A, 0x52, 0x01, 0x03, 0x6A, 0x0D, 0xB5, 0x04, 0x02, 0x02, 0x1C, 0x55, 0xB2, 0x57, 0xDF, 0x0C, 0x73, 0x5F, 0x05, 0x55, 0x55};
	// calculate the key based on our setup
	for(uint16_t x = 0; x < 16; x++){		
		key_id = 0x00 + x;
		switch (key_id){
			case 0:
			case 1:				
			case 2:
			case 3:
			case 4:
				status= sha204_create_diverse_key(root, key_id, key, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to calculate key: ");
					sha204_parser_rc(status);
					return status;
				}
			break;
			case 5:			
				
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,0,7,config, 4);
				if(status != ATCA_SUCCESS){
					sha204_parser_rc(status);
					return status;
				}
				parent_key_id = (config[3] & 0x0F);
				printf("Parent Key: %d \n\r", (config[3] & 0x0F));
			
				// get parent key
				status = sha204_create_diverse_key(root, parent_key_id, parent_key, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to create diverse key: ");
					sha204_parser_rc(status);
					return status;
				}
				// key is created with the parent key, not the root
				status = sha204_create_diverse_key(parent_key, key_id, key, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to create diverse key: ");
					sha204_parser_rc(status);
					return status;
				}
				break;
			break;			
			case 6:
			case 7:
				status= sha204_create_diverse_key(root, 7, key, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to calculate key: ");
					sha204_parser_rc(status);
					return status;
				}				
			break;
			case 8:
			case 9:
				status= sha204_create_diverse_key(root, key_id, key, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to calculate key: ");
					sha204_parser_rc(status);
					return status;
				}
			break;
			case 10:
			case 11:
				memcpy(&key, root, ATCA_KEY_SIZE); // root key for slots
			break;
			default:
			status= sha204_create_diverse_key(root, key_id, key, 0x77);
			if (status != ATCA_SUCCESS)
			{
				printf("Unable to calculate key: ");
				sha204_parser_rc(status);
				return status;
			}
			break;

		}
				
		status = atcab_random(challenge);
		if(status != ATCA_SUCCESS){
			printf("Unable to calculate random challenge:");
			sha204_parser_rc(status);
		}

		memset(other_data, 0, sizeof(other_data));
		other_data[0] = ATCA_MAC;
		other_data[2] = (uint8_t)key_id;
		
		status = atcab_read_serial_number(sn);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcab_mac:\n\r");
			sha204_parser_rc(status);
			return status;
		}

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
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcab_nonce_rand:\n\r");
			sha204_parser_rc(status);
			return status;
		}

		// Calculate nonce value
		status = atcah_nonce(&nonce_params);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcah_nonce:\n\r");
			sha204_parser_rc(status);
			return status;
		}

		// Calculate response
		for (i = 0; i < sizeof(other_data); i++)
		{
			other_data[i] = (uint8_t)(i + 0xF0); // needed?
		}
		
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
			//printf("Software CheckMac passed for slot %d\n\r", key_id);			
		}

		// Perform CheckMac Hardware
		status = atcab_checkmac(checkmac_params.mode,checkmac_params.key_id,checkmac_params.client_chal,
		checkmac_params.client_resp,checkmac_params.other_data);
		if (status != ATCA_SUCCESS)
		{
			printf("Unable to atcab_checkmac for key %d!!!\n\r", key_id);
			sha204_parser_rc(status);
			//return ret_status;
		} else {

			printf("Challenge Success for key %d!\n\r", key_id);
		}
	}

	return status;
}
