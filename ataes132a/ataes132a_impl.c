/*
 * ataes132a_impl.c
 *
 * Created: 8/13/2018 6:48:48 AM
 *  Author: pvallone
 */ 
 #include "ataes132a_impl.h"
 /** \brief Reads the ATAES132a's serial number. Can be read if device is locked or unlocked
 *
 * \param[out] serial_number  
 *
 * \returns 0 on success
 */
 int aes_read_serial_number(uint8_t *serial_number){

	 uint8_t bytesToRead = 8;
	 ataes_transaction_status_t block_read_status;
	 aes_read_status(&block_read_status);
	 
	 ataes_tranaction_t aes_command_pk;
	 aes_command_pk.write_command = AESWRITE;
	 aes_command_pk.opCode = BLOCKREAD;
	 aes_command_pk.mode =	0x00;
	 aes_command_pk.param1 = 0xF000; // Serial Number location

	 aes_command_pk.param2 = bytesToRead; // bytes to read
	 aes_command_pk.size_of_data_buffer = 0; // not need for block read
	 aes_command_pk.rx_buffer_length = bytesToRead;
	 aes_block_read(&aes_command_pk, &block_read_status);
	 
	 if(block_read_status.aes_transaction_status != SUCCESS){
		 aes_print_status(&block_read_status); // for debugging
		 return ATCA_GEN_FAIL;
	 }

	 memcpy(serial_number, &block_read_status.rx_buffer[2], 8);

	 return SUCCESS;
 }
 
 /** \brief Reads the ATAES132a's device details
 *
 * \param[out] details  
 *
 * \returns 0 on success
 */

 int aes_get_aes132a_details(struct aes132a_details * details){
	 
	 // we can only read 32 bytes at a time
	 uint8_t bytesToRead = 32;
	 ataes_transaction_status_t block_read_status;
	 aes_read_status(&block_read_status);
	 
	 ataes_tranaction_t aes_command_pk;
	 aes_command_pk.write_command = AESWRITE;
	 aes_command_pk.opCode = BLOCKREAD;
	 aes_command_pk.mode =	0x00;
	 aes_command_pk.param1 = 0xF000; // start at beginning of configuration memory

	 aes_command_pk.param2 = bytesToRead; // bytes to read
	 aes_command_pk.size_of_data_buffer = 0; // not need for block read
	 aes_command_pk.rx_buffer_length = bytesToRead;
	 aes_block_read(&aes_command_pk, &block_read_status);
	 
	 if(block_read_status.aes_transaction_status != SUCCESS){
		 aes_print_status(&block_read_status); // for debugging
		 return ATCA_GEN_FAIL;
		 } else {
		 
		 memcpy(details->serial_number, &block_read_status.rx_buffer[2], 8); // serial number
		 memcpy(details->lot_history, &block_read_status.rx_buffer[10], 8); // lot number
		 
		 details->ee_page_size = block_read_status.rx_buffer[25];  // eeprom page size
		 details->enc_read_size = block_read_status.rx_buffer[26];  // encrypted read size
		 details->enc_write_size = block_read_status.rx_buffer[27];  // encrypted write size
		 details->device_number = block_read_status.rx_buffer[28];  // device_number
	 }


	 bytesToRead = 32;
	 aes_read_status(&block_read_status);
	 aes_command_pk.write_command = AESWRITE;
	 aes_command_pk.opCode = BLOCKREAD;
	 aes_command_pk.mode =	0x00;
	 aes_command_pk.param1 = 0xF020; // continue reading of configuration memory

	 aes_command_pk.param2 = bytesToRead; // bytes to read
	 aes_command_pk.size_of_data_buffer = 0; // not need for block read
	 aes_command_pk.rx_buffer_length = bytesToRead;
	 aes_block_read(&aes_command_pk, &block_read_status);
	 
	 if(block_read_status.aes_transaction_status != SUCCESS){
		 aes_print_status(&block_read_status); // for debugging
		 return ATCA_GEN_FAIL;
	} else {
		 details->lock_keys = block_read_status.rx_buffer[2];  // lock keys config status - 0x55 when unlocked
		 details->lock_small = block_read_status.rx_buffer[3];  // lock keys config status - 0x55 when unlocked
		 details->lock_config = block_read_status.rx_buffer[4];  // lock keys config status - 0x55 when unlocked
		 details->manufacturing_id[0] = block_read_status.rx_buffer[13];
		 details->manufacturing_id[1] = block_read_status.rx_buffer[14];
		 
	 }

	 // 3rd pass
	 bytesToRead = 8;
	 aes_read_status(&block_read_status);
	 aes_command_pk.write_command = AESWRITE;
	 aes_command_pk.opCode = BLOCKREAD;
	 aes_command_pk.mode =	0x00;
	 aes_command_pk.param1 = 0xF040; // continue reading of configuration memory

	 aes_command_pk.param2 = bytesToRead; // bytes to read
	 aes_command_pk.size_of_data_buffer = 0; // not need for block read
	 aes_command_pk.rx_buffer_length = bytesToRead;
	 aes_block_read(&aes_command_pk, &block_read_status);
	 
	 if(block_read_status.aes_transaction_status != SUCCESS){
		 aes_print_status(&block_read_status); // for debugging
		 return ATCA_GEN_FAIL;
		 } else {
		 details->i2c_address = block_read_status.rx_buffer[2];
		 details->chip_config = block_read_status.rx_buffer[3];
	 }

	 return SUCCESS;
 }
 
 /** \brief Reads the configuration at address
 *
 * \param[in]  keyAddress  
 *
 * \returns 0 on success
 */
 int aes_config_read(uint16_t keyAddress){
	
	uint8_t bytesToRead = 4;
	ataes_transaction_status_t block_read_status;
	aes_read_status(&block_read_status);
	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = BLOCKREAD;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = keyAddress;

	aes_command_pk.param2 = bytesToRead; // bytes to read
	aes_command_pk.size_of_data_buffer = 0; // not need for block read
	aes_command_pk.rx_buffer_length = bytesToRead;
	aes_block_read(&aes_command_pk, &block_read_status);
	if(block_read_status.aes_transaction_status != SUCCESS){
		aes_print_status(&block_read_status); // for debugging
		return ATCA_GEN_FAIL;
	
	} else {		
		for(uint8_t a = 2; a < aes_command_pk.rx_buffer_length + 2; a++){
			printf("0x%02X ", block_read_status.rx_buffer[a]);
		}
		printf("\n\r");
	}
	return SUCCESS;
 }

 /* Software AES*/
 
/** \brief Calculates MACs used by the AES132A for input or verification purposes.
 *
 * \param[in]  params  Parameters required to calculate the MAC.
 * \param[out] pt_mac  Plain-text MAC is returned here, 16 bytes.
 *
 * \returns 0 on success
 */
int atcah_aes132_mac(const struct atcah_aes132_mac_params_t* params, uint8_t pt_mac[16])
{
    
     int cipher_idx = -1;
     symmetric_CBC cbc_ctx;
     uint8_t iv[16];
     uint8_t pt_temp[16];
     uint8_t ct_temp[16];
     uint16_t offset = 0;
     
     if (params == NULL || params->key == NULL || params->nonce == NULL || params->auth_only_data == NULL || pt_mac == NULL){
	     return BAD_PARAM;
	 } else {
	   //  printf("OK 1\n\r");
     }

     cipher_idx = find_cipher("aes");
     if (cipher_idx == -1)
     return ATCA_GEN_FAIL;

     // Authenticate only data length should be 14 bytes minimum, plus multiples of 16 bytes
     if (params->auth_only_length < 14 || (params->auth_only_length - 14) % 16 != 0){
	     return BAD_PARAM;
	     } else {
	   //  printf("OK 2\n\r");
     }

     // Initialize CBC mode for calculating plain-text MAC
     memset(iv, 0, sizeof(iv));
     memset(&cbc_ctx, 0, sizeof(cbc_ctx));
     
     ret_val = cbc_start(cipher_idx, iv, params->key, sizeof(pt_temp), 0, &cbc_ctx);
     if (ret_val != 0){
	     return ret_val;
	   } 

     // MAC first pass
     pt_temp[0] = 0x79;
     memcpy(&pt_temp[1], params->nonce, 12);
     pt_temp[13] = params->mac_count;
     pt_temp[14] = (params->length >> 8) & 0xFF;
     pt_temp[15] = (params->length >> 0) & 0xFF;

     ret_val = cbc_encrypt(pt_temp, ct_temp, sizeof(pt_temp), &cbc_ctx);
     if (ret_val != 0){
	     return ret_val;
	 } 
     
     // MAC second pass
     pt_temp[0] = (params->auth_only_length >> 8) & 0xFF;
     pt_temp[1] = (params->auth_only_length >> 0) & 0xFF;
     memcpy(&pt_temp[2], params->auth_only_data, 14);
     
     ret_val = cbc_encrypt(pt_temp, ct_temp, sizeof(pt_temp), &cbc_ctx);
     if (ret_val != 0){
	     return ret_val;
	 } 

     // Optional extra auth-only data passes
     for (offset = 14; offset < params->auth_only_length; offset += 16)
     {
	     memcpy(pt_temp, &params->auth_only_data[offset], 16);
	     
	     ret_val = cbc_encrypt(pt_temp, ct_temp, sizeof(pt_temp), &cbc_ctx);
	     if (ret_val != 0){
		     return ret_val;
		 }
     }

     // MAC plain-text passes. Partial blocks are zero-padded
     offset = 0;
     while (offset < params->length)
     {
	     memset(pt_temp, 0x00, sizeof(pt_temp));
	     if ((uint16_t)(params->length - offset) >= sizeof(pt_temp))
	     {
		     memcpy(pt_temp, &params->plain_text[offset], sizeof(pt_temp));
		     offset += sizeof(pt_temp);
	     }
	     else
	     {
		     memcpy(pt_temp, &params->plain_text[offset], params->length - offset);
		     offset += params->length - offset;
	     }
	     
	     ret_val = cbc_encrypt(pt_temp, ct_temp, sizeof(pt_temp), &cbc_ctx);
	     if (ret_val != 0){
		     return ret_val;
		 }
     }

     ret_val = cbc_done(&cbc_ctx);
     if (ret_val != 0){
	     return ret_val;
	  }

     // We now have the plain-text MAC
     memcpy(pt_mac, ct_temp, sizeof(ct_temp));

     return SUCCESS;
}


/** \brief Calculates the MAC used by the Encrypt, Decrypt, EncRead, and EncWrite commands.
 *
 * \param[in]  params  Parameters required to calculate the MAC.
 * \param[out] pt_mac  Plaint-text MAC is returned here, 16 bytes.
 *
 * \returns 0 on success
 */
int atcah_aes132_integrity_mac(const struct atcah_aes132_integrity_mac_params_t* params, uint8_t pt_mac[16])
{
     struct atcah_aes132_mac_params_t mac_params;
     uint8_t auth_only_data[14 + 16];
     uint8_t is_decrypt_client_mode = false;

     if (params == NULL || params->plain_text == NULL || params->key == NULL || params->nonce == NULL || pt_mac == NULL)
     return BAD_PARAM;

     // Simplified check, but upper byte of count should be non-zero since MacCount is 1 or higher
     if (params->op_code == DECRYPT && (params->count & 0xFF00))
     is_decrypt_client_mode = true;

     memset(&mac_params, 0, sizeof(mac_params));

     // Build first authenticate only data chunk
     mac_params.auth_only_length = 14;
     memset(auth_only_data, 0, sizeof(auth_only_data));
     auth_only_data[0] = (uint8_t)(params->mfg_id >> 8);
     auth_only_data[1] = (uint8_t)(params->mfg_id & 0xFF);
     auth_only_data[2] = params->op_code;
     auth_only_data[3] = params->mode;
     auth_only_data[4] = is_decrypt_client_mode ? 0x00 : (uint8_t)(params->param1 >> 8);
     auth_only_data[5] = (uint8_t)(params->param1 & 0xFF);
     auth_only_data[6] = is_decrypt_client_mode ? 0x00 : (uint8_t)(params->count >> 8);
     auth_only_data[7] = (uint8_t)(params->count & 0xFF);
     if (params->is_nonce_rand || is_decrypt_client_mode)
     auth_only_data[8] |= AES_MACFLAG_RANDOM;
     if (params->is_input_mac && !is_decrypt_client_mode)
     auth_only_data[8] |= AES_MACFLAG_INPUT;

     // Check for optional authenticate-only fields
     if (params->mode & AES_ENC_MODE_FLAG_USAGE_COUNTER)
     {
	     mac_params.auth_only_length = 30;
	     memcpy(&auth_only_data[14], params->usage_count, 4);
     }
     if (params->mode & AES_ENC_MODE_FLAG_SERIAL_NUM)
     {
	     mac_params.auth_only_length = 30;
	     memcpy(&auth_only_data[18], params->serial_num, 8);
     }
     if (params->mode & AES_ENC_MODE_FLAG_SMALL_ZONE)
     {
	     mac_params.auth_only_length = 30;
	     memcpy(&auth_only_data[26], params->small_zone, 4);
     }

     mac_params.key = params->key;
     mac_params.nonce = params->nonce;
     mac_params.mac_count = params->mac_count;
     mac_params.length = params->count & 0xFF;
     mac_params.auth_only_data = auth_only_data;
     mac_params.plain_text = params->plain_text;

     return atcah_aes132_mac(&mac_params, pt_mac);
}

/** \brief Encrypt data and calculate the MAC required for the Decrypt or EncWrite commands.
 *
 * \param[in]  params  Plain-text data and parameters required to encrypt data and calculate the MAC.
 * \param[out] mac     Encrypted MAC is returned here, 16 bytes.
 * \param[out] ct      Encrypted data (cipher text) is returned here. 16 bytes for (1 to 16 bytes plain-text),
 *                     32 bytes for (17 to 32 bytes plain-text).
 *
 * \returns 0 on success
 */
int atcah_aes132_encrypt_for(const struct atcah_aes132_encrypt_params_t* params, uint8_t mac[16], uint8_t* ct)
{
	
	int cipher_idx = -1;
	struct atcah_aes132_integrity_mac_params_t int_mac_params;
	uint8_t iv[16];
	uint8_t pt_temp[16];
	uint16_t offset = 0;
	uint8_t pt_mac[16];
	symmetric_CTR ctr_ctx;
	//int block = 0;

	if (params == NULL || params->plain_text == NULL || params->key == NULL || params->nonce == NULL || mac == NULL || ct == NULL)
	return BAD_PARAM;

	cipher_idx = find_cipher("aes");
	if (cipher_idx == -1)
		return ATCA_GEN_FAIL;

	// Calculate the plain text integrity MAC
	memset(&int_mac_params, 0, sizeof(int_mac_params));
	int_mac_params.op_code = params->op_code;
	int_mac_params.mode = params->mode;
	int_mac_params.param1 = params->param1;
	int_mac_params.count = params->count;
	int_mac_params.key = params->key;
	int_mac_params.nonce = params->nonce;
	int_mac_params.mac_count = 1;
	int_mac_params.is_nonce_rand = params->is_nonce_rand;
	int_mac_params.is_input_mac = true;
	int_mac_params.mfg_id = params->mfg_id;
	memcpy(int_mac_params.usage_count, params->usage_count, sizeof(int_mac_params.usage_count));
	memcpy(int_mac_params.serial_num, params->serial_num, sizeof(int_mac_params.serial_num));
	memcpy(int_mac_params.small_zone, params->small_zone, sizeof(int_mac_params.small_zone));
	int_mac_params.plain_text = params->plain_text;

	// creates the plain-text-mac
	ret_val = atcah_aes132_integrity_mac(&int_mac_params, pt_mac);
	if (ret_val != 0){
		return ret_val;
	}

	// Initialize CTR mode to actually encrypt the MAC and plain-text data
	iv[0] = 0x01;
	memcpy(&iv[1], params->nonce, 12);
	iv[13] = 0x01;
	iv[14] = 0x00;
	iv[15] = 0x00;
	memset(&ctr_ctx, 0, sizeof(ctr_ctx));	
	
	ret_val = ctr_start(cipher_idx, iv, params->key, 16, 0, 2 | CTR_COUNTER_BIG_ENDIAN, &ctr_ctx);
	if (ret_val != 0)
	return ret_val;
	// Encrypt the MAC

	ret_val = ctr_encrypt(pt_mac, mac, sizeof(pt_mac), &ctr_ctx);
	if (ret_val != 0)
	return ret_val;

	// Encrypt the plain-text
	offset = 0;
	while (offset < params->count)
	{
		uint16_t old_offset = offset;
		memset(pt_temp, 0x00, sizeof(pt_temp));
		if ((uint16_t)(params->count - offset) >= sizeof(pt_temp))
		{
			memcpy(pt_temp, &params->plain_text[offset], sizeof(pt_temp));
			offset += sizeof(pt_temp);
		}
		else
		{
			memcpy(pt_temp, &params->plain_text[offset], params->count - offset);
			offset += params->count - offset;
		}		

		ret_val = ctr_encrypt(pt_temp, &ct[old_offset], sizeof(pt_temp), &ctr_ctx);
		if (ret_val != 0)
		return ret_val;
	}

	ret_val = ctr_done(&ctr_ctx);
	if (ret_val != 0)
	return ret_val;

	return SUCCESS;
}

/** \brief Decrypts data and validates the MAC returned by the Encrypt and EncRead commands.
 *
 * \params[in]  params      Data, MAC, and parameters required to decrypt and validate the MAC.
 * \params[out] plain_text  The decrypted data is returned here. Should be the same size as params->count.
 *
 * \return 0 on success
 */
int atcah_aes132_decrypt_from(const struct atcah_aes132_decrypt_params_t* params, uint8_t* plain_text)
{
	
	int cipher_idx = -1;
	struct atcah_aes132_integrity_mac_params_t int_mac_params;
	uint8_t iv[16];
	uint8_t pt_temp[16];
	uint16_t offset = 0;
	uint8_t pt_mac[16];
	uint8_t calc_pt_mac[16];
	symmetric_CTR ctr_ctx;
	//int block = 0;

	if (params == NULL || params->out_mac == NULL || params->out_data == NULL || params->key == NULL || params->nonce == NULL || plain_text == NULL)
	return BAD_PARAM;
	cipher_idx = find_cipher("aes");
	if (cipher_idx == -1)
	return ATCA_GEN_FAIL;

	// Initialize CTR mode to decrypt the MAC and plain-text data
	iv[0] = 0x01;
	memcpy(&iv[1], params->nonce, 12);
	iv[13] = 0x01;
	iv[14] = 0x00;
	iv[15] = 0x00;
	memset(&ctr_ctx, 0, sizeof(ctr_ctx));

	ret_val = ctr_start(cipher_idx, iv, params->key, 16, 0, 2 | CTR_COUNTER_BIG_ENDIAN, &ctr_ctx);
	if (ret_val != 0)
	return ret_val;

	// Decrypt the MAC

	ret_val = ctr_decrypt(params->out_mac, pt_mac, sizeof(pt_mac), &ctr_ctx);
	if (ret_val != 0)
	return ret_val;

	// Decrypt the plain-text
	offset = 0;
	while (offset < params->count)
	{
		
		ret_val = ctr_decrypt(&params->out_data[offset], pt_temp, sizeof(pt_temp), &ctr_ctx);
		if (ret_val != 0)
		return ret_val;

		if ((uint16_t)(params->count - offset) >= sizeof(pt_temp))
		{
			memcpy(&plain_text[offset], pt_temp, sizeof(pt_temp));
			offset += sizeof(pt_temp);
		}
		else
		{
			memcpy(&plain_text[offset], pt_temp, params->count - offset);
			offset += params->count - offset;
		}
	}

	ret_val = ctr_done(&ctr_ctx);
	if (ret_val != 0)
	return ret_val;

	// Calculate the plain text integrity MAC
	memset(&int_mac_params, 0, sizeof(int_mac_params));
	int_mac_params.op_code = params->op_code;
	int_mac_params.mode = params->mode;
	int_mac_params.param1 = params->param1;
	int_mac_params.count = params->count;
	int_mac_params.key = params->key;
	int_mac_params.nonce = params->nonce;
	int_mac_params.mac_count = 1;
	int_mac_params.is_nonce_rand = params->is_nonce_rand;
	int_mac_params.is_input_mac = false;
	int_mac_params.mfg_id = params->mfg_id;
	memcpy(int_mac_params.usage_count, params->usage_count, sizeof(int_mac_params.usage_count));
	memcpy(int_mac_params.serial_num, params->serial_num, sizeof(int_mac_params.serial_num));
	memcpy(int_mac_params.small_zone, params->small_zone, sizeof(int_mac_params.small_zone));
	int_mac_params.plain_text = plain_text;

	ret_val = atcah_aes132_integrity_mac(&int_mac_params, calc_pt_mac);
	if (ret_val != 0){
		return ret_val;
	}
	

	// We now have the calculated plain-text MAC. See if it matches the decrypted MAC.
	if (memcmp(calc_pt_mac, pt_mac, sizeof(calc_pt_mac)) != 0){
		return MAC_ERROR;
	}
	

	return SUCCESS;
}

/** \brief Calculate the value of the AES132A nonce register after a nonce command.
 *
 * \param[in]  mode     Nonce mode
 * \param[in]  in_seed  Input seed value (or actual nonce when using Inbound Nonce mode), 12 bytes.
 * \param[in]  mfg_id   ManufacturingID register value (0x00EE by default)
 * \param[out] random   Random number returned from the AES132A nonce command, can be NULL for Inbound Nonce mode, 12 bytes.
 * \param[out] nonce    Calculated nonce register value, 12 bytes.
 *
 * \return 0 on success
 */
int atcah_aes132_nonce(uint8_t mode, const uint8_t in_seed_val[12], uint16_t mfg_id, const uint8_t random[12], uint8_t nonce[12])
{
	uint8_t block_a[16];
	uint8_t block_b[16];
	uint8_t ct[16];
	int cipher_idx = -1;
	symmetric_ECB ctx;
	
	int i = 0;

	if (in_seed_val == NULL || nonce == NULL)
	return BAD_PARAM;

	if (mode == AES_NONCE_MODE_INBOUND) 
	{
		// This mode is a straight pass-through of the in_seed_val value into the nonce register
		memcpy(nonce, in_seed_val, 12);
		return SUCCESS;
	}

	if (random == NULL)
	return BAD_PARAM;
	
	cipher_idx = find_cipher("aes");
	if (cipher_idx == -1)
	return ATCA_GEN_FAIL;

	// Build block A
	block_a[0] = NONCE;
	block_a[1] = mode;
	memset(&block_a[2], 0, 2);
	memcpy(&block_a[4], in_seed_val, 12);

	// Build block B
	block_b[0] = (mfg_id >> 8) & 0xFF;
	block_b[1] = (mfg_id & 0xFF);
	memset(&block_b[2], 0, 2);
	memcpy(&block_b[4], random, 12);

	// Initialize ECB mode (block B is the key)
	memset(&ctx, 0, sizeof(ctx));
	
	ret_val = ecb_start(cipher_idx, block_b, sizeof(block_b), 0, &ctx);
	if (ret_val != 0)
	return ret_val;

	// Encrypt block A with block B as the key
	
	ret_val = ecb_encrypt(block_a, ct, sizeof(block_a), &ctx);
	if (ret_val != 0)
	return ret_val;

	ret_val = ecb_done(&ctx);
	if (ret_val != 0)
	return ret_val;

	// XOR the cipher text output with block_a
	for (i = 0; i < 12; i++)
	nonce[i] = ct[i] ^ block_a[i];

	return SUCCESS;
}

/* Hardware*/
/** \brief - Generates a node from the provided in-seed - You should run aes_create_random_number() to create the random inseed
 * 
 * \param[in]  mode - Mode used for generating Nonce
 * \param[out] nonceOut - 12 byte Generated Nonce 
 * \param[in]  inseed - 12 byte inseed for generating nonce
 *
 * \returns 0 on success
 */
int aes_create_nonce(uint8_t mode, uint8_t *nonceOut, uint8_t *inseed){
	
	ataes_transaction_status_t random_number_status;
	aes_read_status(&random_number_status);

	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = NONCE;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = 0x0000;
	aes_command_pk.param2 = 0x0000;
	aes_command_pk.data_buffer = inseed;
	aes_command_pk.size_of_data_buffer = 12;
	aes_command_pk.rx_buffer_length = 18;
	aes_write_command(&aes_command_pk, &random_number_status);
	
	if(random_number_status.aes_transaction_status != SUCCESS){
		aes_print_status(&random_number_status); // for debugging
		printf("failed!\n\r");
		return ATCA_GEN_FAIL;
	}

	memcpy(nonceOut, &random_number_status.rx_buffer[2], 16);
	return SUCCESS;
}

/** \brief Passes a 12 byte nonce and stored it in the nonce register
 *
 * \param[in]  seed - 12 byte inseed for generating nonce
 *
 * \returns 0 on success
 */
int aes_create_pass_through_nonce(uint8_t *seed){
	
	ataes_transaction_status_t random_number_status;
	aes_read_status(&random_number_status);
	
	ataes_tranaction_t aes_command_pk;
	
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = NONCE;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = 0x0000;
	aes_command_pk.param2 = 0x0000;
	aes_command_pk.data_buffer = seed;
	aes_command_pk.size_of_data_buffer = 12;
	aes_command_pk.rx_buffer_length = 4;
	aes_write_command(&aes_command_pk, &random_number_status);
	
	if(random_number_status.aes_transaction_status != SUCCESS){
		aes_print_status(&random_number_status); // for debugging
		return ATCA_GEN_FAIL;
	}
	return SUCCESS;
}
/** \brief Generates a Random number - If the device is not locked, the ATAES132a will return 
 *  0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5 0xA5
 *
 * \param[out] random  
 *
 * \returns 0 on success
 */
int aes_create_random_number(uint8_t *random){

	ataes_transaction_status_t random_number_status;
	aes_read_status(&random_number_status);
	
	uint8_t data[1] = {'\0'};
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = RANDOM;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = 0x0000;
	aes_command_pk.param2 = 0x0000;
	aes_command_pk.data_buffer = data;
	aes_command_pk.size_of_data_buffer = sizeof data;
	aes_command_pk.rx_buffer_length = 18;
	aes_write_command(&aes_command_pk, &random_number_status);
	
	if(random_number_status.aes_transaction_status != SUCCESS){
		aes_print_status(&random_number_status); // for debugging
		return ATCA_GEN_FAIL;
	} 
	memcpy(random, &random_number_status.rx_buffer[2], 16);
	return SUCCESS;
}

/** \brief Reads the ATAES132a's Manufacturing Id
 *
 * \param[out] mfg_id  
 *
 * \returns 0 on success
 */
int aes_read__manufacturing_id(uint8_t * mfg_id){
	uint8_t bytesToRead = 8;
	ataes_transaction_status_t block_read_status;
	aes_read_status(&block_read_status);
	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = BLOCKREAD;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = 0xF028; // block_read_test_manufacturing_id location

	aes_command_pk.param2 = bytesToRead; // bytes to read
	aes_command_pk.size_of_data_buffer = 0; // not need for block read
	aes_command_pk.rx_buffer_length = bytesToRead;
	aes_block_read(&aes_command_pk, &block_read_status);
	
	if(block_read_status.aes_transaction_status != SUCCESS){
		aes_print_status(&block_read_status); // for debugging
		return ATCA_GEN_FAIL;
	}

	memcpy(mfg_id, &block_read_status.rx_buffer[5], 2);
	return SUCCESS;
}

/** \brief Performs a block read of 1 - 32 bytes at address provided.
 *
 * \param[in]  address - The address of data to read.
 * \param[in]  length  -  No more an 32 bytes
 * \param[out] read_data  - Output data (Clear Text Only).
 *
 * \returns 0 on success
 */
int aes_block_read_memory(uint16_t address, uint8_t length, uint8_t *read_data){
	
	if(length > 32){
		printf("Length must be less than 32 bytes\n\r");
		return ATCA_GEN_FAIL;
	}
	uint8_t bytesToRead = length;
	ataes_transaction_status_t block_read_status;
	aes_read_status(&block_read_status);
	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = BLOCKREAD;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = address;
	aes_command_pk.param2 = bytesToRead; // bytes to read
	aes_command_pk.size_of_data_buffer = 0; // not needed for block read
	aes_command_pk.rx_buffer_length = bytesToRead + 2;
	aes_block_read(&aes_command_pk, &block_read_status);
	
	if(block_read_status.aes_transaction_status != SUCCESS){
		aes_print_status(&block_read_status); // for debugging
		return ATCA_GEN_FAIL;
	}	
	memcpy(read_data, &block_read_status.rx_buffer[2], length);	
	return SUCCESS;
}

/** \brief Write to either User Zone or Small Zone
 *
 * \param[in]  address - The address to write the data to
 * \param[in]  data - 1 - 32 bytes of data to write (Clear Text Only)
 * \param[in]  data_length - Length of data to be written
 *
 * \returns 0 on success
 */
int aes_write_to_userzone(uint16_t address, uint8_t * data, uint8_t data_length){
	
	ataes_transaction_status_t mystatus;
	aes_read_status(&mystatus);
	return aes_memory_write(address, data, data_length, &mystatus);

}
/** \brief Performs Hardware Encryption on the ATAES132a
 *
 * \param[in]  mode - Mode used for encryption
 * \param[in]  key_id - The number of the key used for encryption (0-15)
 * \param[in]  count - Upper byte is always 0x00. Lower byte is the number of bytes to be encrypted
 * \param[in]  in_data - Input data to be encrypted (plain text).
 * \param[out] out_mac - Integrity MAC for the output data.
 * \param[out] out_data - 16 or 32 bytes Encrypted data (cipher text).
 *
 * \returns 0 on success
 */
int aes_encrypt(uint8_t mode, uint16_t key_id, uint16_t count, uint8_t* in_data, uint8_t out_mac[16], uint8_t* out_data){

	ataes_transaction_status_t encrypt_status;
	aes_read_status(&encrypt_status);
	uint8_t eCount = (uint8_t) (count & 0xff); // get lower byte for encryption count
	
	uint8_t out_data_len = 16;
	if(eCount > 16){
		out_data_len = 32;
	}	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.length = 0;
	aes_command_pk.opCode = ENCYRPT;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = 0x00 + key_id;
	aes_command_pk.param2 = eCount;
	aes_command_pk.data_buffer = in_data;
	aes_command_pk.size_of_data_buffer = eCount;
	aes_command_pk.rx_buffer_length = out_data_len + 18;
	aes_write_command(&aes_command_pk, &encrypt_status);
	
	if(encrypt_status.aes_transaction_status != SUCCESS){
		aes_print_status(&encrypt_status); // for debugging
		return ATCA_GEN_FAIL;
	} else {
		memcpy(out_mac,&encrypt_status.rx_buffer[2], 16);
		memcpy(out_data,&encrypt_status.rx_buffer[18], out_data_len);
	}

	return SUCCESS;
}

/** \brief Performs Hardware Decryption on the ATAES132a (Client Mode)
 *
 * \param[in]  mode - Mode used for decryption
 * \param[in]  key_id - The number of the key used for decryption (0-15)
 * \param[in]  count - Upper byte is the EMacCount. Lower byte is the number of bytes to be returned after decryption
 * \param[in]  in_data - Input data to be decrypted (cipher text).
 * \param[out] out_data - 16 or 32 bytes decrypted data (plain text).
 *
 * \returns 0 on success
 */
int aes_decrypt(uint8_t mode, uint16_t key_id, uint16_t count, uint8_t in_mac[16], uint8_t* in_data, uint8_t* out_data){

	ataes_transaction_status_t transaction_status;
	aes_read_status(&transaction_status);
	//uint8_t buffer_size = 32;
	uint8_t eCount = (uint8_t) (count & 0xff); // get lower byte for encryption count
	uint8_t buffer[48];
	memset(&buffer, 0, sizeof buffer);;
	memcpy(&buffer[0], in_mac, 16);
	memcpy(&buffer[16], in_data, 32);
	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = DECRYPT;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = key_id;
	aes_command_pk.param2 = count; //
	aes_command_pk.data_buffer = buffer;
	aes_command_pk.size_of_data_buffer = eCount <= 16 ? 32 : 48;
	aes_command_pk.rx_buffer_length = eCount + 2;
	aes_write_command(&aes_command_pk, &transaction_status);
	
	if(transaction_status.aes_transaction_status != SUCCESS){
		aes_print_status(&transaction_status); // for debugging
		return ATCA_GEN_FAIL;
	}

	memcpy(out_data, &transaction_status.rx_buffer[2], eCount);

	return SUCCESS;
}


/** \brief The EncWrite command decrypts the cipher text input data, verifies the input MAC, and then writes 1 to 32
 *         bytes to a User Zone or 16 bytes to Key Memory.
 *
 * \param[in] mode     The AES_ENC_MODE_FLAG_USAGE_COUNTER, AES_ENC_MODE_FLAG_SERIAL_NUM, AES_ENC_MODE_FLAG_SMALL_ZONE
 *                     flags can be combined for the mode.
 * \param[in] address  Address to write to.
 * \param[in] count    Number of bytes to write. 1 to 32 for user zone writes. 16 for key writes.
 * \param[in] in_mac   Input MAC to be verified.
 * \param[in] in_data  Encrypted Data (cipher text). Must be 16 bytes (count <= 16) or 32 bytes (count > 16).
 *
 * \return ATCA_SUCCESS
 */
int aes_encrypt_write(uint8_t mode, uint16_t address, uint16_t count, const uint8_t in_mac[16], const uint8_t* in_data){
	
	uint8_t temp_buffer[50]; // todo fix size - should be 32
	ataes_transaction_status_t encrypt_status;
	aes_read_status(&encrypt_status);

	memcpy(temp_buffer, in_mac, 16);
	memcpy(&temp_buffer[16], in_data, 32);
	
	uint8_t in_data_size = 0; 
	if (count <= 16){
		in_data_size = 32; 
	}else {
		in_data_size = 48; 
	}
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = ENCWRITE;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = address;
	aes_command_pk.param2 = count;
	aes_command_pk.data_buffer = temp_buffer;
	aes_command_pk.size_of_data_buffer = in_data_size;
	aes_command_pk.rx_buffer_length = 4;
	aes_write_command(&aes_command_pk, &encrypt_status);
	
	if(encrypt_status.aes_transaction_status != SUCCESS){
		aes_print_status(&encrypt_status); // for debugging
		return ATCA_GEN_FAIL;
	} 
	return SUCCESS;
}

/** \brief EncRead reads 1 to 32 bytes of encrypted data from User Memory, along with an integrity MAC. The EncRead
 *         command only performs encrypted reads.
 *
 * \param[in]  mode      The AES_ENC_MODE_FLAG_USAGE_COUNTER, AES_ENC_MODE_FLAG_SERIAL_NUM, AES_ENC_MODE_FLAG_SMALL_ZONE
 *                       flags can be combined for the mode.
 * \param[in]  address   Address to read from.
 * \param[in]  count     Number of bytes to read (1 to 32).
 * \param[out] out_mac   Integrity MAC for the output data will be returned here.
 * \param[out] out_data  Encrypted output data (ciphertext) will be returned here.
 *                       Must be 16 bytes (count <= 16) or 32 bytes (count > 16).
 *
 * \return ATCA_SUCCESS
 */
int aes_encrypt_read(uint8_t mode, uint16_t address, uint16_t count, uint8_t out_mac[16], uint8_t* out_data){

	ataes_transaction_status_t encrypt_status;
	aes_read_status(&encrypt_status);
	uint8_t buffer[1] = {'\0'};
	
	uint8_t out_data_size = 0; 
	if (count <= 16){
		out_data_size = 16; 
	}else {
		out_data_size = 32; 
	}

	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = ENCREAD;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = address;
	aes_command_pk.param2 = count;
	aes_command_pk.data_buffer = buffer;
	aes_command_pk.size_of_data_buffer = 1;
	aes_command_pk.rx_buffer_length = 50 ;// ? not sure about this
	aes_write_command(&aes_command_pk, &encrypt_status);
	
	if(encrypt_status.aes_transaction_status != SUCCESS){
		aes_print_status(&encrypt_status); // for debugging
		return ATCA_GEN_FAIL;
	}
	memcpy(&out_mac[0], &encrypt_status.rx_buffer[2], 16);
	memcpy(&out_data[0], &encrypt_status.rx_buffer[18], out_data_size);

	return SUCCESS;
}

/** \brief Locks either the Small Zone, Configuration Memory or Key Memory
 *  Warning do not run unless you have configured the device 
 * \param[in]  mode - See below valid modes.  
 *
 * Lock the SmallZone Register: mode = 0x00
 * Lock the Key Memory: mode = 0x01
 * Lock the Configuration Memory, excluding SmallZone: mode = 0x02
 * Set the ZoneConfig[Zone].ReadOnly byte to ReadOnly: mode = 0x03
 * Need to lock config first e.g. aes_lock_config_zone(0x02); 
 *
 * aes_lock_zone(0x02);
 * aes_lock_zone(0x01);
 * \returns 0 on success
 */
int aes_lock_zone(uint8_t mode){

	ataes_transaction_status_t block_read_status;
	aes_read_status(&block_read_status);
	memset(&block_read_status.rx_buffer, 0, sizeof block_read_status.rx_buffer);
	uint8_t dummy[4] = {0x00,0x00,0x00,0x00};
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = LOCK;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = 0x0000;
	aes_command_pk.param2 = 0x0000;
	aes_command_pk.data_buffer = dummy;
	aes_command_pk.size_of_data_buffer = 0;
	aes_command_pk.rx_buffer_length = 4;
	aes_write_command(&aes_command_pk, &block_read_status);
	
	if(block_read_status.aes_transaction_status != SUCCESS){

		aes_print_status(&block_read_status); // for debugging
		for(uint8_t a = 2; a < aes_command_pk.rx_buffer_length + 2; a++){
			printf("0x%02X ", block_read_status.rx_buffer[a]);
		}
		printf("\n\r");
		return ATCA_GEN_FAIL;
	} 

	printf("** Zone Locked Successful **\n\r");
	return SUCCESS;
}

// not working yet
void aes_reset(void){
	
	// 7.22 Reset Command
	ataes_transaction_status_t transaction_status;
	aes_read_status(&transaction_status);
	
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = RESET;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = 0x0000;
	aes_command_pk.param2 = 0x0000; //
	aes_command_pk.data_buffer = 0;
	aes_command_pk.size_of_data_buffer = 0;
	aes_command_pk.rx_buffer_length = 0;
	aes_write_command(&aes_command_pk, &transaction_status);
	
}
int aes_info(uint16_t selector){
	// 7.12 INFO Command
	ataes_transaction_status_t transaction_status;
	aes_read_status(&transaction_status);
			
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = INFO;
	aes_command_pk.mode =	0x00;
	aes_command_pk.param1 = selector;
	aes_command_pk.param2 = 0x00; //
	aes_command_pk.data_buffer = 0;
	aes_command_pk.size_of_data_buffer = 0;
	aes_command_pk.rx_buffer_length = 6;
	aes_write_command(&aes_command_pk, &transaction_status);
		
	if(transaction_status.aes_transaction_status != SUCCESS){
		aes_print_status(&transaction_status); // for debugging
		return ATCA_GEN_FAIL;
	}
	for(uint8_t a = 2; a < aes_command_pk.rx_buffer_length - 2; a++){
		printf("0x%02X ", transaction_status.rx_buffer[a]);
	}
	printf("\n\r");

	return SUCCESS;
}
int aes_auth(uint16_t key, uint8_t mode, uint16_t usage, uint8_t *in_mac, uint8_t *out_mac){
	
	ataes_transaction_status_t transaction_status;
	aes_read_status(&transaction_status);
	
	uint8_t len = 0;
	if(mode == 0x02 || mode == 0x03){
		len = 16;		
	}
	ataes_tranaction_t aes_command_pk;
	aes_command_pk.length = 0;
	aes_command_pk.write_command = AESWRITE;
	aes_command_pk.opCode = AUTH;
	aes_command_pk.mode =	mode;
	aes_command_pk.param1 = key;
	aes_command_pk.param2 = usage; // Auth usage - keyuse, writeok & readok
	if(mode == 0x01){
		aes_command_pk.data_buffer = in_mac;
		aes_command_pk.size_of_data_buffer = 16; 
	} else {
		mode = 0x00;
		aes_command_pk.size_of_data_buffer = 0; 
	}	
	
	aes_command_pk.rx_buffer_length = len + 2; // need to test for mutual auth
	aes_write_command(&aes_command_pk, &transaction_status);
	
	if(transaction_status.aes_transaction_status != SUCCESS){
		aes_print_status(&transaction_status); // for debugging

		return transaction_status.rx_buffer[1]; // TODO ADD THIS on all events
	}
	
	if(in_mac == NULL){
		memcpy(&out_mac[0], &transaction_status.rx_buffer[2], len);
	} else {
		// if successful, check auth status
		printf("Auth Status:\t");
		int ret = aes_info(INFO_AUTHSTATUS);
		if(ret != 0){
			printf("aes_info() failed with code %d ", ret);
			aes_print_rc(ret);
		}
	}
	

	return SUCCESS;
}
int aes_config_write(uint16_t keyAddress, uint8_t * settings){
	int ret = SUCCESS;
	ataes_transaction_status_t mystatus;
	aes_read_status(&mystatus);
	delay_ms(10);
	
	uint16_t addr = keyAddress;
	uint8_t datalen = 4; 
	ret = aes_memory_write(addr, settings, datalen, &mystatus);
	if(ret != 0){
		return ret;
	}
	return ret;
}

int aes_counter_config_write(uint16_t keyAddress, uint8_t * settings){
	int ret = SUCCESS;
	ataes_transaction_status_t mystatus;
	aes_read_status(&mystatus);		
	uint16_t addr = keyAddress;
	uint8_t datalen = 4;
	ret = aes_memory_write(addr, settings, datalen, &mystatus);
	if(ret != 0){
		return ret;
	}
	return ret;
}

int aes_key_write(uint16_t keyAddress, uint8_t * settings){
	int ret = SUCCESS;
	ataes_transaction_status_t mystatus;
	aes_read_status(&mystatus);
	
	uint16_t addr = keyAddress;
	uint8_t datalen = 16; 
	ret = aes_key_mem__write(addr, settings, datalen, &mystatus);
	if(ret != 0){
		return ret;
	}

	printf("Key was 0x%X written: ", keyAddress);
	for(uint8_t i = 0; i < 16; i++){
		printf("%02X ", settings[i]);
	}
	printf("\n\r");
	return ret;
}

 /* Helpers */ 

 /** \brief Reads Key config, User zone config, counter config  */
 void aes_read_configuration(void){
	 printf("** Key Config Read **\n\r");
	 // READ CONFIG
	 printf("Key Config 00: ");
	 aes_config_read(KEYCONFIG00);
	 delay_ms(5);
	 printf("Key Config 01: ");
	 aes_config_read(KEYCONFIG01);
	 delay_ms(5);
	 printf("Key Config 02: ");
	 aes_config_read(KEYCONFIG02);
	 delay_ms(5);
	 printf("Key Config 03: ");
	 aes_config_read(KEYCONFIG03);
	 delay_ms(5);
	 printf("Key Config 04: ");
	 aes_config_read(KEYCONFIG04);
	 delay_ms(5);
	 printf("Key Config 05: ");
	 aes_config_read(KEYCONFIG05);
	 delay_ms(5);
	 printf("Key Config 06: ");
	 aes_config_read(KEYCONFIG06);
	 delay_ms(5);
	 printf("Key Config 07: ");
	 aes_config_read(KEYCONFIG07);
	 delay_ms(5);
	 printf("Key Config 08: ");
	 aes_config_read(KEYCONFIG08);
	 delay_ms(40);
	 printf("Key Config 09: ");
	 aes_config_read(KEYCONFIG09);
	 delay_ms(5);
	 printf("Key Config 10: ");
	 aes_config_read(KEYCONFIG10);
	 delay_ms(5);
	 printf("Key Config 11: ");
	 aes_config_read(KEYCONFIG11);
	 delay_ms(5);
	 printf("Key Config 12: ");
	 aes_config_read(KEYCONFIG12);
	 delay_ms(5);
	 printf("Key Config 13: ");
	 aes_config_read(KEYCONFIG13);
	 delay_ms(5);
	 printf("Key Config 14: ");
	 aes_config_read(KEYCONFIG14);
	 delay_ms(5);
	 printf("Key Config 15: ");
	 aes_config_read(KEYCONFIG15);
	 delay_ms(5);
	 printf("** User Zone Read **\n\r");
	 printf("User Zone 00:\t");
	 aes_config_read(CONFIGUSERZONE00);
	 delay_ms(1);
	 printf("User Zone 01:\t");
	 aes_config_read(CONFIGUSERZONE01);
	 delay_ms(1);
	 printf("User Zone 02:\t");
	 aes_config_read(CONFIGUSERZONE02);
	 delay_ms(1);
	 printf("User Zone 03:\t");
	 aes_config_read(CONFIGUSERZONE03);
	 delay_ms(1);
	 printf("User Zone 04:\t");
	 aes_config_read(CONFIGUSERZONE04);
	 delay_ms(1);
	 printf("User Zone 05:\t");
	 aes_config_read(CONFIGUSERZONE05);
	 delay_ms(1);
	 printf("User Zone 06:\t");
	 aes_config_read(CONFIGUSERZONE06);
	 delay_ms(1);
	 printf("User Zone 07:\t");
	 aes_config_read(CONFIGUSERZONE07);
	 delay_ms(1);
	 printf("User Zone 08:\t");
	 aes_config_read(CONFIGUSERZONE08);
	 delay_ms(1);
	 printf("User Zone 09:\t");
	 aes_config_read(CONFIGUSERZONE09);
	 delay_ms(1);
	 printf("User Zone 10:\t");
	 aes_config_read(CONFIGUSERZONE10);
	 delay_ms(1);
	 printf("User Zone 11:\t");
	 aes_config_read(CONFIGUSERZONE11);
	 delay_ms(1);
	 printf("User Zone 12:\t");
	 aes_config_read(CONFIGUSERZONE12);
	 delay_ms(1);
	 printf("User Zone 13:\t");
	 aes_config_read(CONFIGUSERZONE13);
	 delay_ms(1);
	 printf("User Zone 14:\t");
	 aes_config_read(CONFIGUSERZONE14);
	 delay_ms(1);
	 printf("User Zone 15:\t");
	 aes_config_read(CONFIGUSERZONE15);
	 delay_ms(1);
	 printf("**** Counter Config ****\n\r");
	 aes_config_read(COUNTERCONF00);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF02);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF04);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF06);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF08);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF10);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF12);
	 delay_ms(5);
	 aes_config_read(COUNTERCONF14);
	 delay_ms(5);
 }

 /** \brief Prints a buffer to stdio
 *
 * \param[in]  buff - buffer to print
 * \param[in]  size - size of buffer
 *
 */
 void aes_print_buffer(uint8_t *buff, uint8_t size){
	 for(uint8_t i = 0; i < size;i++){
		 printf("0x%02X ", buff[i]);
	 }
	 printf("\n\r");
 }
 /** \brief Prints ACES friendly buffer to stdio - Use this to test your data in ACES
 *
 * \param[in]  buff - buffer to print
 * \param[in]  size - size of buffer
 *
 */
 void aes_print_buffer_for_aces(uint8_t *buff, uint8_t size){	 
	 for(uint8_t i = 0; i < size;i++){
		 printf("%02X ", buff[i]);
	 }
	 printf("\n\r");
 }
 /** \brief Converts the value returned from AES functions to Friendly error and prints to stdio 
 *
 * \param[in]  ret_value  -  value returned from AES functions 
 *
 */
 void aes_print_rc(int ret_value) {
	 
	 switch (ret_value) {
		 case 0:
		 printf("SUCCESS");
		 break;
		 case 0x02:
		 printf("BOUNDRY_ERROR");
		 break;
		 case 0x04:
		 printf("RW_CONFING");
		 break;
		 case 0x08:
		 printf("BAD_ADDRESS");
		 break;
		 case 0x10:
		 printf("COUNT_ERROR");
		 break;
		 case 0x20:
		 printf("NONCE_ERROR");
		 break;
		 case 0x40:
		 printf("MAC_ERROR");
		 break;
		 case 0x50:
		 printf("PARSE_ERROR");
		 break;
		 case 0x60:
		 printf("DARA_MATCH");
		 break;
		 case 0x70:
		 printf("LOCK_ERROR");
		 break;
		 case 0x80:
		 printf("KEY_ERROR");
		 break;
		 default:
		 printf("unknown error %d \n\r", ret_value);
		 break;

	 }
	 printf("\n\r");
 }
 /** \brief Scans the i2c Bus for devices and prints to stdio  */
 void aes_scan_i2c(void){
	uint8_t error = 0;	
	for(uint8_t add = 1; add < 127; add++ ) {		
		scan_packet.address  = add;
		scan_packet.ten_bit_address = false;
		scan_buffer[0] = 0x00;
		scan_packet.data_length = 1;
		scan_packet.data = scan_buffer;
		error = i2c_master_write_packet_wait(&aes_i2c_master_instance, &scan_packet);		
		if (error == 0)		{
			printf("I2C device found at decimal address 0X%x\n\r", add);
		}		
	}
 }