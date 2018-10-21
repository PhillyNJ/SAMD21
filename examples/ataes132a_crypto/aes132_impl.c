/*
 * aes132_impl.c
 *
 * Created: 9/15/2018 7:11:20 AM
 *  Author: pvallone
 */ 
 #include "aes132_impl.h"

 /*
	Reads Device details and stores in a struct for later use 
	Param1[out] - Pointer to Details Struct
	
 */
 void aes132_read_device_details(struct aes132a_details * details){

	uint8_t ret_code;
	uint8_t data_len = 32;
	uint16_t addr = 0xF000;

	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);

	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) {
		aes_print_rc(ret_code);
		return;
	};
	memcpy(details->serial_number, &g_rx_buffer[2], 8); // serial number
	memcpy(details->lot_history, &g_rx_buffer[10], 8); // lot number
	
	details->ee_page_size = g_rx_buffer[25];  // eeprom page size
	details->enc_read_size = g_rx_buffer[26];  // encrypted read size
	details->enc_write_size = g_rx_buffer[27];  // encrypted write size
	details->device_number = g_rx_buffer[28];  // device_number
	
	addr = 0xF020;
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) {
		aes_print_rc(ret_code);
		return;
	};
	
	details->lock_keys = g_rx_buffer[2];  // lock keys config status - 0x55 when unlocked
	details->lock_small = g_rx_buffer[3];  // lock keys config status - 0x55 when unlocked
	details->lock_config = g_rx_buffer[4];  // lock keys config status - 0x55 when unlocked
	details->manufacturing_id[0] = g_rx_buffer[13];
	details->manufacturing_id[1] = g_rx_buffer[14];

	addr = 0xF041; // get chip config
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, 1,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) {
		aes_print_rc(ret_code);
		return;
	};
	details->chip_config = g_rx_buffer[2];

	aes132_read_small_zone(details->small_zone, 4);	

	printf("Serial Number:\t\t");
	for(uint8_t i = 0; i < 8;i++){
		printf("0x%02X ", myDetails.serial_number[i]);
	}
	printf("\n\rLockKeys:\t\t0x%02X %s\n\r", myDetails.lock_keys, myDetails.lock_keys != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("LockSmall:\t\t0x%02X %s\n\r", myDetails.lock_small, myDetails.lock_small != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("LockConfig:\t\t0x%02X %s\n\r", myDetails.lock_config, myDetails.lock_config != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("Manufacturing Id:\t0x%02X%02X\n\r", myDetails.manufacturing_id[0], myDetails.manufacturing_id[1]);
	printf("Small Zone[0:3]:\t0x%02X 0x%02X 0x%02X 0x%02X\n\r", myDetails.small_zone[0], myDetails.small_zone[1],myDetails.small_zone[2], myDetails.small_zone[3]);
	printf("ChipConfig:\t\t0x%02X\n\r", details->chip_config);
 }

 /* 
	Prints the command block and receive data 
	Param[in] - Return code for parsing 
 */
void aes132_print_command_block(int ret_code)
{
#if defined(AES132SHOWCOMMANDBLOCK)
	aes132_debug_command_puthex(g_tx_buffer);
	printf("\r\n");
	aes132_debug_response_puthex(ret_code, g_rx_buffer);
	printf("\r\n");
	aes_print_rc(ret_code);
#endif
}

/*
	Reads n of bytes form the small zone
	Param1[out] - Pointer array to be read to
	Param2[out] - Number of bytes to be read - Max 32
*/
 void aes132_read_small_zone(uint8_t *data, uint8_t bytesToRead){
	
	 int ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, AES132_SMALL_ZONE_ADDR, bytesToRead,
	 0, NULL, 0, NULL, 0, NULL, 0, NULL,	g_tx_buffer, g_rx_buffer);	 
	 aes132_print_command_block(ret_code);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 memcpy(data, &g_rx_buffer[2], bytesToRead);
 }

 /*
	7.12 INFO Command
	Read the info register (2 bytes) - 
	Param1[in] - Selector 
	Param1[out] - Array to be read to -
 */
 void aes132_read_info(uint8_t selector, uint8_t out_data[2]){
	 
	 uint8_t ret_code;
	 ret_code = aes132m_execute(AES132_OPCODE_INFO, 0x00, selector, 0x0000,
	 2, 0, 0, NULL,
	 0, NULL, 0, NULL,
	 g_tx_buffer, g_rx_buffer);	 
	 aes132_print_command_block(ret_code);	 
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 memcpy(&out_data[0], &g_rx_buffer[2], 2);
 }
 
 /*
	Sends an Auth request - 7.1 Auth Command
	Param1[in] - Key Number for Authorization
	Param2[in] - pointer to key used for MAC for Authorization
	Param2[in] - Usage
 
 */
 void aes132_inbound_auth_key(uint8_t keyid, uint8_t *key, uint16_t usage)
 {
	 uint8_t ret_code;

	 // Scratch-pads
	 uint8_t in_mac[16];
	 
	 // Structure for calculation input/output
	 struct aes132h_in_out mac_compute_encrypt_param;
	 
	 printf("===================== Test Inbound Auth ====================\r\n");
	 uint16_t param1 = 0x00 + keyid;
	 // --- Calculate MAC for Inbound Auth in Software ---
	 // Fill in parameters and execute MAC checking function
	 mac_compute_encrypt_param.opcode  = AES132_OPCODE_AUTH;
	 mac_compute_encrypt_param.mode    = 0x01; // Perform Inbound Only authentication
	 mac_compute_encrypt_param.param1  = param1; // param 1  AuthKeyId
	 mac_compute_encrypt_param.param2  = usage;
	 mac_compute_encrypt_param.key     = key;
	 mac_compute_encrypt_param.nonce   = &g_nonce;
	 mac_compute_encrypt_param.out_mac = in_mac;             // Output from helper function is input for AES132 device
	 
	 ret_code = aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	 
	 printf("Calculated MAC (Software)\r\n");
	 printf("OutMac         : 0x"); printf_puthex_array(in_mac, 16); printf("\r\n");
	 printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	 
	 // --- Do Inbound Auth ---
	 ret_code = aes132m_execute(AES132_OPCODE_AUTH, 0x01, param1, usage,	16, in_mac, 0, NULL, 0, NULL, 0, NULL,	g_tx_buffer, g_rx_buffer);
	 printf("Auth Command\r\n");
	 aes132_print_command_block(ret_code);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
 }

 uint8_t aes132_lock_zone(uint8_t mode){
  
	uint8_t ret_code;	
	//AES132_OPCODE_LOCK
	ret_code = aes132m_execute(AES132_OPCODE_LOCK, mode, 0,0,0,NULL,0,NULL,0,NULL,0,NULL, g_tx_buffer,g_rx_buffer);
	
	aes132_print_command_block(ret_code);
	return ret_code;
	
 
 }
//Personalization if the ATAES132a device
void aes132_personalize(void)
{
	uint8_t ret_code;
	uint8_t data_len;
	uint16_t addr;
	uint8_t data[16];
	
	printf("** Personalize ** \r\n");
	// configure ChipConfig
	uint8_t chipConfig[1] = {0xC7};
	ret_code = aes132m_write_memory(1,  0xF041, chipConfig);
	printf("ChipConfig, addr: 0xF041 ");
	aes132_debug_readwrite_puthex(1, ret_code, chipConfig);

	// === Configure the key ===
	// Write Key 00
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(0), key00);
	printf("Write Key 00, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(0));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key00);
	
	// Write Key 01
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(1), key01);
	printf("Write Key 01, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(1));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key01);
	
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(2), key02);
	printf("Write Key 02, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(2));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key02);
	
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(3), key03);
	printf("Write Key 03, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(3));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key03);
	
	// Write Key 04
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(4), key04);
	printf("Write Key 04, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(4));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key04);

	//pvallone added the rest of the keys
	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(5), key05);
	printf("Write Key 05, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(5));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key05);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(6), key06);
	printf("Write Key 06, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(6));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key06);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(7), key07);
	printf("Write Key 07, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(7));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key07);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(8), key08);
	printf("Write Key 08, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(8));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key08);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(8), key09);
	printf("Write Key 09, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(9));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key09);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(10), key10);
	printf("Write Key 10, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(10));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key10);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(11), key11);
	printf("Write Key 11, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(11));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key11);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(12), key12);
	printf("Write Key 12, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(12));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key12);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(13), key13);
	printf("Write Key 13, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(13));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key13);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(14), key14);
	printf("Write Key 14, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(14));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key14);

	ret_code = aes132m_write_memory(AES132_KEY_LENGTH, AES132_KEY_ADDR(15), key15);
	printf("Write Key 15, addr: 0x");
	printf_puthex_16(AES132_KEY_ADDR(15));
	aes132_debug_readwrite_puthex(AES132_KEY_LENGTH, ret_code, key15);

	// Write KeyConfig 00
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x00);
	data[0] = 0x01;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 00, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write KeyConfig 01
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x01);
	data[0] = (AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_PARENT | AES132_KEY_CONFIG_CHILD | AES132_KEY_CONFIG_RANDOM_NONCE | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);//0xE5;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 01, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write KeyConfig 02
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(2);
	data[0] = (AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_PARENT | AES132_KEY_CONFIG_CHILD | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);// 0xE1;
	data[1] = (AES132_KEY_CONFIG_AUTH_COMPUTE | AES132_KEY_CONFIG_TRANSFER_OK | AES132_KEY_CONFIG_IMPORT_OK) ;// 0xD0;
	data[2] = 0x00;
	data[3] = 0x01;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 02, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
		
	// Write KeyConfig 03
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(3);
	data[0] = (AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);// 0xE1;
	data[1] = (AES132_KEY_CONFIG_AUTH_COMPUTE | AES132_KEY_CONFIG_TRANSFER_OK | AES132_KEY_CONFIG_IMPORT_OK) ;// 0xD0;
	data[2] = 0x00;
	data[3] = 0x01; // decryptRead ok
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 03, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write KeyConfig 04
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x04);
	data[0] = 0x00;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 04, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write KeyConfig 05
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x05);
	data[0] = (AES132_KEY_CONFIG_AUTH_KEY | AES132_KEY_CONFIG_RANDOM_NONCE | AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_PARENT | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);
	data[1] = 0x00;
	data[2] = 0x0D; // key 13 is required for pre-auth
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 05, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write KeyConfig 06
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x06);
	data[0] = (AES132_KEY_CONFIG_CHILD  | AES132_KEY_CONFIG_EXTERNAL_CRYPTO | AES132_KEY_CONFIG_RANDOM_NONCE);
	data[1] = 0x00;
	data[2] = 0x05; // // link to parent key
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 07, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(0x07);
	data[0] = (AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_PARENT | AES132_KEY_CONFIG_CHILD | AES132_KEY_CONFIG_RANDOM_NONCE | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);//0xE5;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x01;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	// Write KeyConfig 12
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(12);
	data[0] = (AES132_KEY_CONFIG_CHANGE_KEYS | AES132_KEY_CONFIG_AUTH_KEY | AES132_KEY_CONFIG_EXTERNAL_CRYPTO | AES132_KEY_CONFIG_RANDOM_NONCE);
	data[1] = AES132_KEY_CONFIG_AUTH_COMPUTE; //0x80; // AuthComput OK
	data[2] = 0x0D; // key 13 for Auth
	data[3] = 0x01; // DecReadOk0
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 12, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write KeyConfig 13
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(13);
	data[0] = (AES132_KEY_CONFIG_INBOUND_AUTH | AES132_KEY_CONFIG_EXTERNAL_CRYPTO | AES132_KEY_CONFIG_RANDOM_NONCE);//0x03;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x01; // DecReadOk0
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 13, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	

	// Write KeyConfig 14
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(14);
	data[0] = (AES132_KEY_CONFIG_RANDOM_NONCE | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);//0x05;
	data[1] = (AES132_KEY_CONFIG_AUTH_COMPUTE | AES132_KEY_CONFIG_TRANSFER_OK | AES132_KEY_CONFIG_IMPORT_OK);//0xD0;
	data[2] = 0x00;
	data[3] = 0x01;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 14, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write KeyConfig 15 - used for key 14 authorization
	data_len = AES132_KEY_CONFIG_LENGTH;
	addr = AES132_KEY_CONFIG_ADDR(15);
	data[0] = (AES132_KEY_CONFIG_AUTH_KEY | AES132_KEY_CONFIG_RANDOM_NONCE | AES132_KEY_CONFIG_EXTERNAL_CRYPTO);// 0x14;
	data[1] = AES132_KEY_CONFIG_AUTH_COMPUTE;// 0x80;
	data[2] = 0x0E; // key 14 link pointer auth
	data[3] = 0x01;
	ret_code = aes132m_write_memory(data_len, addr, data);
	
	printf("Write KeyConfig 15, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// === Write zeros to user zone ===
	data_len = 16;
	memset(data, 0x00, data_len);
	// Write User Zone #0
	addr = AES132_USER_ZONE_ADDR(0);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #0, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write User Zone #1
	addr = AES132_USER_ZONE_ADDR(1);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #1, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write User Zone #2
	addr = AES132_USER_ZONE_ADDR(2);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #2, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	
	// === Configure user zone permission ===
	// Write ZoneConfig 00 -- free read/write
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(0x00);
	data[0] = 0x00;
	data[1] = 0xF0;
	data[2] = 0x1F;
	data[3] = 0xFF;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 00, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);
	
	// Write ZoneConfig 01 -- need auth for write
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(0x01);
	data[0] = AES132_ZONE_CONFIG_AUTH_WRITE;
	data[1] = (0x04 << 4);	// AuthID is upper nibble of byte #1 & ReadID is lower nibble of byte #1
	data[2] = 0x00;			// WriteID is upper nibble of byte #2
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 01, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write ZoneConfig 02 -- need auth for read/write
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(0x02);
	data[0] = AES132_ZONE_CONFIG_AUTH_READ | AES132_ZONE_CONFIG_AUTH_WRITE;
	data[1] = (0x04 << 4);	// AuthID is upper nibble of byte #1
	data[2] = 0x00;
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 02, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write ZoneConfig 03 -- need auth for read/write
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(0x03);
	data[0] = AES132_ZONE_CONFIG_USE_SERIAL | AES132_ZONE_CONFIG_ENC_WRITE | AES132_ZONE_CONFIG_ENC_READ | AES132_ZONE_CONFIG_AUTH_READ | AES132_ZONE_CONFIG_AUTH_WRITE | AES132_ZONE_CONFIG_USE_SMALL;
	data[1] = 0xE7;	// WriteId
	data[2] = (0x07 << 4);; // key 7 write ID
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 03, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write ZoneConfig 04 -- need auth for read/write
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(4);
	data[0] = AES132_ZONE_CONFIG_ENC_WRITE | AES132_ZONE_CONFIG_ENC_READ ;
	data[1] = 0x07;	// WriteId
	data[2] = (0x07 << 4) | AES132_ZONE_CONFIG_VOLATILE_TRANSFER_OK; // key 7 write ID  
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 04, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// Write ZoneConfig 15 -- need auth for read/write with Auth Key 14
	data_len = AES132_ZONE_CONFIG_LENGTH;
	addr = AES132_ZONE_CONFIG_ADDR(0x0F);
	data[0] = AES132_ZONE_CONFIG_AUTH_READ | AES132_ZONE_CONFIG_AUTH_WRITE;
	data[1] = (0x0E << 4);	// AuthID is upper nibble of byte #1
	data[2] = 0x00;
	data[3] = 0x00;
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write ZoneConfig 15, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data);

	// write to small zone AES132_SMALL_ZONE_ADDR
	uint8_t small_data[] = "$3rc3tZoneData";
	data_len = sizeof(small_data);
	addr = AES132_SMALL_ZONE_ADDR;
	ret_code = aes132m_write_memory(data_len, addr, small_data);
	
	printf("Write small zone, addr: 0x");
	printf_puthex_16(addr);
	printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, small_data);
	printf("\r\n");
}

 /** \brief Reads Key config, User zone config, counter config  */
void aes132_read_configuration(void){
	 
	 printf("** Key Config Read **\n\r");

	 for(int i = 0; i < 16; i++){
		 printf("Key Config 0x%02X - %d: ", AES132_KEY_CONFIG_ADDR(i), i);
		 aes132_read_config(AES132_KEY_CONFIG_ADDR(i));
	 }
	 printf("** Zone Config Read **\n\r");
	 for(int i = 0; i < 16; i++){
		 printf("User Zone Config 0x%02X - %d: ", AES132_ZONE_CONFIG_ADDR(i), i);
		 aes132_read_config(AES132_ZONE_CONFIG_ADDR(i));
	 }
	 printf("**** Counter Config ****\n\r");
	 for(int i = 0; i < 16; i++){
		 printf("Counter Config 0x%02X - %d: ", AES132_COUNTER_CONFIG_ADDR(i), i);
		 aes132_read_counter_config(AES132_COUNTER_CONFIG_ADDR(i));
	 }
 }

void aes132_read_config(uint16_t addr){

	 uint8_t ret_code;
	 uint8_t data_len = 4;
	 
	 ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	 0, NULL, 0, NULL, 0, NULL, 0, NULL,
	 g_tx_buffer, g_rx_buffer);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 
	 for(uint8_t a = 2; a < 6; a++){
		 printf("0x%02X ", g_rx_buffer[a]);
	 }
	 printf("\n\r");
 }

void aes132_read_counter_config(uint16_t addr){
	 uint8_t ret_code;
	 uint8_t data_len = 2;
	 memset(&g_rx_buffer[0], 9, sizeof g_rx_buffer);
	 ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	 0, NULL, 0, NULL, 0, NULL, 0, NULL,
	 g_tx_buffer, g_rx_buffer);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 
	 for(uint8_t a = 2; a < 4; a++){
		 printf("0x%02X ", g_rx_buffer[a]);
	 }
	 printf("\n\r");
 }
 
 void aes132_nonce(void)
 {	 
	 printf("======================== Nonce ========================\r\n");
	
	 uint8_t ret_code;	 
	 uint8_t in_seed[12];
	 uint8_t i;	 
	 // Scratch-pads
	 uint8_t random[16];

	 // Structure for nonce calculation input/output
	 struct aes132h_nonce_in_out nonce_param;
	 
	 // Generate in_seed
	 for (i = 0; i < 12; i++) {
		 in_seed[i] = rand() % 0xFF;
	 }
	
	 ret_code = aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, 12, in_seed, 0, NULL,	0, NULL, 0, NULL,g_tx_buffer, g_rx_buffer);
	 printf("Nonce Command\r\n");
	 aes132_print_command_block(ret_code);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 
	 // Get random number
	 memcpy(random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);

	 // --- Nonce in Software ---
	 nonce_param.mode    = 0x01;
	 nonce_param.in_seed = in_seed;
	 nonce_param.random  = random;
	 nonce_param.nonce   = &g_nonce;
	 
	 ret_code = aes132h_nonce(&nonce_param);
	 if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	 
	 printf("Calculated Nonce (Software)\r\n");
	 printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	 printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	 printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	 printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
 }

 /** \brief Creates a new key for KeyMemory 6. 
 * \param [out] newKey"  Pointer to 16-byte new key.
 * Key06 is the child Key
 * Key05 is the parent key and is used for the cryptographic functions
 * Requires pre-auth with key 13 to run 
 */
int aes132_key_create(uint8_t *newKey){
	// Variable Initialization
	uint8_t InSeed[12]	= { 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,0x00, 0x00, 0x02, 0x00 };
	uint8_t Random[16];	
	uint8_t OutData[32];
	uint8_t InMac[16];
	uint8_t InData[16];
	uint8_t OutMac[16];
	uint8_t ret_code;
	memset(&OutData, 0, sizeof OutData);
	memset(&InMac, 0, sizeof InMac);
	memset(&OutMac, 0, sizeof OutMac);

	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;

	printf("=====================           KeyCreate Test     =============================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return ret_code;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);	

	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return ret_code;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	uint8_t mode = 0x03; // keycreate for keyload	
	uint16_t param2 = 0x00;
	uint8_t childKeyId  = 0x0006;
	
	// create in-mac
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_KEY_CREATE;
	mac_compute_encrypt_param.mode          = mode;
	mac_compute_encrypt_param.param1        = childKeyId; // child key
	mac_compute_encrypt_param.param2        = param2;
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;
	mac_compute_encrypt_param.serial_num    = NULL;
	mac_compute_encrypt_param.small_zone    = NULL;
	mac_compute_encrypt_param.key           = key05;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = NULL;
	mac_compute_encrypt_param.out_data      = NULL;
		
	ret_code	= aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);	
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return ret_code;
		
	printf("Encrypted MAC:\r\n");	
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
		
	// Copy encrypted mac to local variable
	memcpy(&InMac[0], OutMac, 16);

	ret_code	= aes132m_execute(AES132_OPCODE_KEY_CREATE, mode, childKeyId, param2, sizeof(InMac), InMac, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);

	printf("Key Create Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return ret_code;

	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 32);
	
	printf("OutMac				: 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("Encrypted key value (ciphertext): 0x"); printf_puthex_array(OutData, 16); printf("\r\n\r\n");
	
	// 3)	Send Inbound Nonce Command
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return ret_code;
	
	printf("Calculated Nonce (Software)\r\n");
	// Print Value
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	// Print MacCount
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	// Print Valid flag
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	// Print Random flag
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Copy NonceMemory to local variable
	memcpy(InSeed, g_nonce.value, 12); 
	memcpy(InMac, OutMac, 16);		
	memcpy(InData, OutData, 16);
	memset(&OutData, 0, 16);
	// decrypt the key
	struct aes132h_in_out mac_check_decrypt_param;
	printf("Software Decryption:\n\r");

	mac_check_decrypt_param.opcode        = AES132_OPCODE_KEY_CREATE;
	mac_check_decrypt_param.mode          = mode;
	mac_check_decrypt_param.param1        = childKeyId;
	mac_check_decrypt_param.param2        = param2;
	mac_check_decrypt_param.count_value   = NULL;
	mac_check_decrypt_param.usage_counter = NULL;
	mac_check_decrypt_param.serial_num    = NULL;
	mac_check_decrypt_param.small_zone    = NULL;
	mac_check_decrypt_param.key           = key05;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = InMac;
	mac_check_decrypt_param.out_mac       = NULL;
	mac_check_decrypt_param.in_data       = InData;
	mac_check_decrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	aes_print_rc(ret_code); printf("\r\n");
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return ret_code;
	
	printf("Decrypted Data (Software)\r\n");	
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	memcpy(newKey, &OutData, 16);
	return ret_code;
}


/** \brief Loads a Key into the key memory 
 * \param [in] childKeyID" -  the Target key memory
 * \param [in] parentKey" -  Pointer to 16-byte key used for the cryptographic functions
 * \param [in] newKey"  Pointer to 16-byte key to load into childKeyId.
 * Requires pre-auth may be required - Check Config 
 */
void aes132_keyload_to_key_memory(uint16_t childKeyID, uint8_t *parentKey, uint8_t *newKey)
{
	// Variable Initialization
	uint8_t InSeed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Random[16];
	uint8_t OutData[16];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	uint8_t ret_code;
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;
	
	printf("==================  keyload key memory - key %d =================\r\n", childKeyID);
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Calculate Nonce
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	// Print Value
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	// Print MacCount
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	// Print Valid flag
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	// Print Random flag
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");

	memset(&mac_compute_encrypt_param, 0, sizeof mac_compute_encrypt_param);
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_KEY_LOAD;
	mac_compute_encrypt_param.mode          = 0x01;  // copy key to Key Memory
	mac_compute_encrypt_param.param1        = childKeyID;  // ChildKeyID is key 6 to be loaded
	mac_compute_encrypt_param.param2        = 0x0000;
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;
	mac_compute_encrypt_param.serial_num    = NULL;
	mac_compute_encrypt_param.small_zone    = NULL;
	mac_compute_encrypt_param.key           = parentKey; // parent key for the encryption
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = newKey;
	mac_compute_encrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Encrypted MAC & Data (Software)\r\n");
	// Print InData
	printf("key_vol        : 0x"); printf_puthex_array(key_vol, 16); printf("\r\n");
	// Print OutMac
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	// Print OutData
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	// Print MacCount
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// 3)	Send keyload Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	
	ret_code	= aes132m_execute(AES132_OPCODE_KEY_LOAD, 0x01, childKeyID, 0x0000,
	sizeof(InMac), InMac, sizeof(OutData), OutData, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("keyload Command\r\n");
	aes132_print_command_block(ret_code);
}

void aes132_encrypt_decrypt_key(uint16_t eKeyId, uint8_t *ekey)
{
	// Variable Initialization
	uint8_t InSeed[12]	= { 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,0x00, 0x00, 0x02, 0x00 };
	uint8_t Random[16];
	uint8_t InData[] = "Hardware-Encrypt-Soft-Decrypt";
	uint8_t OutData[32];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	uint8_t ret_code;
	memset(&OutData, 0, sizeof OutData);
	memset(&InMac, 0, sizeof InMac);
	memset(&OutMac, 0, sizeof OutMac);

	struct aes132h_nonce_in_out nonce_param;
	
	printf("=============              Encrypt Test             =============\r\n");
	printf("==== Key %d for Hardware Encryption and Software Decryption  ====\r\n", eKeyId);
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Send Encrypt Command
	//uint16_t eKeyId = 0x0006;
	
	ret_code	= aes132m_execute(AES132_OPCODE_ENCRYPT, 0x00, eKeyId, sizeof(InData),
	sizeof(InData), InData, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Encrypt Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 32);
	
	// 3)	Send Inbound Nonce Command
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Copy NonceMemory to local variable
	memcpy(InSeed, g_nonce.value, 12);
	memcpy(InMac, OutMac, 16);
	memcpy(InData, OutData, 32);
	
	struct aes132h_in_out mac_check_decrypt_param;
	printf("Software Decryption:\n\r");
	mac_check_decrypt_param.opcode        = AES132_OPCODE_ENCRYPT;
	mac_check_decrypt_param.mode          = 0x00;
	mac_check_decrypt_param.param1        = eKeyId;
	mac_check_decrypt_param.param2        = sizeof(InData);
	mac_check_decrypt_param.count_value   = NULL;
	mac_check_decrypt_param.usage_counter = NULL;
	mac_check_decrypt_param.serial_num    = NULL;
	mac_check_decrypt_param.small_zone    = NULL;
	mac_check_decrypt_param.key           = ekey;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = InMac;
	mac_check_decrypt_param.out_mac       = NULL;
	mac_check_decrypt_param.in_data       = InData;
	mac_check_decrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	aes_print_rc(ret_code); printf("\r\n");
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Decrypted Data (Software)\r\n");
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	printf("Plain Text     : %s\n\r", OutData);
}

void aes132_outbound_auth(uint16_t keyid, uint8_t *key, uint8_t mode, uint16_t usage)
{
	uint8_t ret_code;

	// Scratch-pads
	uint8_t counter[4];
	uint8_t serial_num[8];
	uint8_t small_zone[4];
	uint8_t out_mac[16];

	// Structure for calculation input/output
	struct aes132h_in_out mac_check_decrypt_param;
	
	printf("==================== Test Outbound Auth ====================\r\n");
	
	// --- Do Outbound Auth ---
	//ret_code = aes132m_execute(AES132_OPCODE_AUTH, 0xE2, 0x0004, 0x0000, 0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	ret_code = aes132m_execute(AES132_OPCODE_AUTH, mode, keyid, 0x0000, 0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Auth Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Get OutMac
	memcpy(out_mac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);

	// --- Calculate and verify OutMac from Outbound Auth in Software ---
	// includes additional authenticate-only data
	// Read Counter value
	ret_code = aes132m_execute(AES132_OPCODE_COUNTER, 0x01, 0x0000, 0x0000,	0, NULL, 0, NULL, 0, NULL, 0, NULL,	g_tx_buffer, g_rx_buffer);
	printf("Read Counter 00\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;

	// Get Counter[0:3]
	memcpy(counter, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 4);

	// Read SerialNum[0:7]
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, AES132_SERIAL_NUM_ADDR, AES132_SERIAL_NUM_LENGTH,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("Read SerialNum[0:7]\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;

	// Get SerialNum[0:7]
	memcpy(serial_num, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 8);
	
	// Read SmallZone[0:3]
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, AES132_SMALL_ZONE_ADDR, 4,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("Read SmallZone[0:3]\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;

	// Get SmallZone[0:3]
	memcpy(small_zone, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 4);

	// Fill in parameters and execute MAC checking function
	mac_check_decrypt_param.opcode        = AES132_OPCODE_AUTH;
	mac_check_decrypt_param.mode          = mode;
	mac_check_decrypt_param.param1        = keyid;
	mac_check_decrypt_param.param2        = 0x0000;
	mac_check_decrypt_param.usage_counter = counter;
	mac_check_decrypt_param.serial_num    = serial_num;
	mac_check_decrypt_param.small_zone    = small_zone;
	mac_check_decrypt_param.key           = key;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = out_mac;             // Output from AES132 device is input for helper functions
	
	ret_code = aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	
	// Check if MAC matches
	if (ret_code == AES132_DEVICE_RETCODE_SUCCESS)
	{
		printf("Verify, Success\r\n");
	}
	else
	{
		printf("Verify, Fail\r\n");
		printf("ReturnCode     : 0x"); printf_puthex(ret_code); aes_print_rc(ret_code); printf("\r\n");
	}
	
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n\n");
	aes_print_rc(ret_code);
}

void aes132_mutual_auth(uint16_t keyid, uint8_t *key, uint8_t mode, uint8_t usage)
{
	uint8_t ret_code;

	// Scratch-pads
	uint8_t in_mac[16];
	uint8_t out_mac[16];
	
	// Structure for calculation input/output
	struct aes132h_in_out mac_compute_encrypt_param;
	struct aes132h_in_out mac_check_decrypt_param;
	
	printf("===================== Test Mutual Auth =====================\r\n");
	
	// --- Calculate InMac for Mutual Auth in Software ---
	// Fill in parameters and execute MAC computation function
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_AUTH;
	mac_compute_encrypt_param.mode          = mode;
	mac_compute_encrypt_param.param1        = keyid;
	mac_compute_encrypt_param.param2        = usage;
	mac_compute_encrypt_param.key           = key;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.out_mac       = in_mac;             // Output from helper functions is input for AES132 device
	
	ret_code = aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated MAC (Software)\r\n");
	printf("OutMac         : 0x"); printf_puthex_array(in_mac, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// --- Do Mutual Auth ---
	ret_code = aes132m_execute(AES132_OPCODE_AUTH, mode, keyid, usage,
	16, in_mac, 0, NULL,
	0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("Auth Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Get OutMac to ucOutMac
	memcpy(out_mac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], AES132_MAC_LENGTH);
	
	// --- Calculate and verify OutMac from Mutual Auth in Software ---
	// Fill in parameters and execute MAC checking function
	mac_check_decrypt_param.opcode        = AES132_OPCODE_AUTH;
	mac_check_decrypt_param.mode          = mode;
	mac_check_decrypt_param.param1        = keyid;
	mac_check_decrypt_param.param2        = usage;
	mac_check_decrypt_param.key           = key;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = out_mac;              // Output from AES132 device is input for helper functions
	
	ret_code = aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	
	// Check if MAC matches
	if (ret_code == AES132_DEVICE_RETCODE_SUCCESS)
	{
		printf("Verify, Success\r\n");
	}
	else
	{
		printf("Verify, Fail\r\n");
		printf("ReturnCode     : 0x"); printf_puthex(ret_code); printf("\r\n");
	}	

	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n\n");
	aes_print_rc(ret_code);
}
//Reset authentication testing
void aes132_reset_auth(void)
{
	uint8_t ret_code;
	// --- Do Reset Auth ---
	ret_code = aes132m_execute(AES132_OPCODE_AUTH, 0x00, 0x0000, 0x00,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("Auth Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	aes_print_rc(ret_code);
}
//Encrypt and Decrypt using ATAES132
void aes132_encrypt_decrypt(void)
{
	// Variable Initialization
	uint8_t InSeed[12]	= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00 };
	uint8_t Random[16];
	uint8_t InData[] = "Encrypt-Decrypt";
	uint8_t OutData[16];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	uint8_t ret_code;
	
	struct aes132h_nonce_in_out nonce_param;
	
	printf("================== Test Encrypt & Decrypt ==================\r\n");
	printf("==================  Key 1 for Encryption  ==================\r\n");
	printf("==================  Key 2 for Decryption ==================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Send Encrypt Command
	uint16_t eKeyId = 0x0001;
	uint16_t dKeyId = 0x0102; // encryption key 0x01, Decryption key 02
	ret_code	= aes132m_execute(AES132_OPCODE_ENCRYPT, 0x00, eKeyId, 0x0010,	sizeof(InData), InData, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Encrypt Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 16);
	
	// 3)	Send Inbound Nonce Command
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Copy NonceMemory to local variable
	memcpy(InSeed, g_nonce.value, 12);
	
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x00, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// 4)	Send Decrypt Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(InData, OutData, 16);
	// eKeyId = 1
	// dKeyid = 2 - These keys must be the same to work
	ret_code	= aes132m_execute(AES132_OPCODE_DECRYPT, 0X00, dKeyId, 0x0110,
	sizeof(InMac), InMac, sizeof(InData), InData, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Decrypt Command\r\n");
	aes132_print_command_block(ret_code);
	printf("Plain Text: '%s' \n\r", &g_rx_buffer[2]);
}
//Encrypt data using helper function then Encrypt Write to AES132 Memory
void aes132_encrypt_encwrite(void)
{
	// Variable Initialization
	uint8_t in_seed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00};
	uint8_t random[16];
	
	uint8_t in_data[32]	= {0x42,0x65,0x20,0x53,0x75,0x72,0x65,0x20,0x54,0x6F,0x20,0x44,0x72,0x69,0x6E,0x6B,0x20,0x59,0x6F,0x75,0x72,0x20,0x4F,0x76,0x61,0x6C,0x74,0x69,0x6E,0x65,0x00,0x00};	
	uint8_t out_data[32];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	
	uint8_t ret_code;
	
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;
	
	printf("================== Test Encrypt & EncWrite =================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(in_seed), in_seed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable(A0)
	memcpy(random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Encrypt Data
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = in_seed;
	nonce_param.random  = random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_ENC_WRITE;
	mac_compute_encrypt_param.mode          = 0x00;
	mac_compute_encrypt_param.param1        = AES132_USER_ZONE_ADDR(0);
	mac_compute_encrypt_param.param2        = sizeof in_data;
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;
	mac_compute_encrypt_param.serial_num    = NULL;
	mac_compute_encrypt_param.small_zone    = NULL;
	mac_compute_encrypt_param.key           = key01;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = in_data;
	mac_compute_encrypt_param.out_data      = out_data;
	
	ret_code	= aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Encrypted MAC & Data (Software)\r\n");
	printf("InData         : 0x"); printf_puthex_array(in_data, sizeof in_data); printf("\r\n");
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("OutData        : 0x"); printf_puthex_array(out_data, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// 3)	Send EncWrite Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(in_data, out_data, sizeof in_data);
	
	ret_code	= aes132m_execute(AES132_OPCODE_ENC_WRITE, 0x00, AES132_USER_ZONE_ADDR(0), sizeof in_data, sizeof(InMac), InMac, sizeof(in_data), in_data, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("EncWrite Command\r\n");
	aes132_print_command_block(ret_code);
	aes_print_rc(ret_code);
}
/** \brief
 *	Encrypt Read to AES132 Memory then Decrypt it using helper function
 */
void aes132_encread_decrypt(void)
{
	// Variable Initialization
	uint8_t InSeed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};
	uint8_t Random[16];	
	uint8_t InData[32];
	uint8_t OutData[32];	
	uint8_t InMac[16];
	uint8_t OutMac[16];	
	uint8_t ret_code;
	
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_check_decrypt_param;
	
	printf("================== Test EncRead & Decrypt ==================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Send EncRead Command
	ret_code	= aes132m_execute(AES132_OPCODE_ENC_READ, 0x00, AES132_USER_ZONE_ADDR(0), 0x0020,
	0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("EncRead Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 32);
	
	// 3)	Decrypt Data
	// Calculate Nonce Memory
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");	
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");	
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(InData, OutData, sizeof OutData);

	mac_check_decrypt_param.opcode        = AES132_OPCODE_ENC_READ;
	mac_check_decrypt_param.mode          = 0x00;
	mac_check_decrypt_param.param1        = AES132_USER_ZONE_ADDR(0);
	mac_check_decrypt_param.param2        = sizeof(InData);
	mac_check_decrypt_param.count_value   = NULL;
	mac_check_decrypt_param.usage_counter = NULL;
	mac_check_decrypt_param.serial_num    = NULL;
	mac_check_decrypt_param.small_zone    = NULL;
	mac_check_decrypt_param.key           = key00;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = InMac;
	mac_check_decrypt_param.out_mac       = NULL;
	mac_check_decrypt_param.in_data       = InData;
	mac_check_decrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Decrypted Data (Software)\r\n");
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	printf("Plain Text     : %s\n\r", OutData);

}

//Write user zone testing
void aes132_write_user_zone(void)
{
	uint8_t ret_code;
	uint8_t data_len = 16;
	uint16_t addr;
	
	uint8_t data[] = "Hello ATAES132a";

	printf("==================== Test Write User Zone ==================\r\n");
	
	// Write user zone #0
	addr = AES132_USER_ZONE_ADDR(0);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #0, addr: 0x"); printf_puthex_16(addr); aes_print_rc(ret_code);printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data); printf("\r\n");
	
	// Write user zone #1
	addr = AES132_USER_ZONE_ADDR(1);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #1, addr: 0x"); printf_puthex_16(addr);aes_print_rc(ret_code); printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data); printf("\r\n");
	
	// Write user zone #2
	addr = AES132_USER_ZONE_ADDR(2);
	ret_code = aes132m_write_memory(data_len, addr, data);
	printf("Write user zone #2, addr: 0x"); printf_puthex_16(addr);aes_print_rc(ret_code); printf("\r\n");
	aes132_debug_readwrite_puthex(data_len, ret_code, data); printf("\r\n");
}

void aes132_read_serial(uint8_t *sn){

	// BlockRead user zone #0
	uint16_t addr = 0xF000;
	int ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, 32,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);	
	
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	memcpy(sn, &g_rx_buffer[2], 8); // serial number

}

void aes132_write_user_zone_sn(void){

	uint8_t ret_code;
		
	aes132_nonce();
	aes132_inbound_auth_key(14, key14, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK));
	
	uint8_t in_seed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00};
	uint8_t random[16];
	
	uint8_t in_data[32] = {0x54,0x6F,0x6F,0x20,0x4D,0x61,0x6E,0x79,0x20,0x53,0x65,0x63,0x72,0x65,0x74,0x73,0x2D,0x53,0x6E,0x65,0x61,0x6B,0x65,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t out_data[32];
	uint8_t InMac[16];
	uint8_t OutMac[16];	
	uint8_t mode = 0xC0; // Use Serial Number & Small Zone
	
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(in_seed), in_seed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable(A0)
	memcpy(random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Encrypt Data
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = in_seed;
	nonce_param.random  = random;
	nonce_param.nonce   = &g_nonce;

	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	uint8_t sn[8];
	uint8_t sz[4];
	aes132_read_serial(sn);
	aes132_read_small_zone(sz,4);
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_ENC_WRITE;
	mac_compute_encrypt_param.mode          = mode;
	mac_compute_encrypt_param.param1        = AES132_USER_ZONE_ADDR(3);
	mac_compute_encrypt_param.param2        = sizeof in_data;
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;	
	mac_compute_encrypt_param.serial_num    = sn;
	mac_compute_encrypt_param.small_zone    = sz;
	mac_compute_encrypt_param.key           = key07;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = in_data;
	mac_compute_encrypt_param.out_data      = out_data;
	
	ret_code = aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Encrypted MAC & Data (Software)\r\n");
	printf("InData         : 0x"); printf_puthex_array(in_data, sizeof in_data); printf("\r\n");
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("OutData        : 0x"); printf_puthex_array(out_data, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// 3)	Send EncWrite Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(in_data, out_data, sizeof in_data);
	
	ret_code	= aes132m_execute(AES132_OPCODE_ENC_WRITE, mode, AES132_USER_ZONE_ADDR(3), sizeof in_data, sizeof(InMac), InMac, sizeof(in_data), in_data, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("EncWrite Command\r\n");
	aes132_print_command_block(ret_code);
	aes_print_rc(ret_code);

}


void aes132_encread_decrypt_sn(void)
{
	aes132_nonce();
	aes132_inbound_auth_key(14, key14, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK));
	uint8_t sn[8];
	uint8_t sz[4];
	aes132_read_serial(sn);
	aes132_read_small_zone(sz, 4);
	// Variable Initialization
	uint8_t InSeed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};
	uint8_t Random[16];	
	uint8_t InData[32];
	uint8_t OutData[32];	
	uint8_t InMac[16];
	uint8_t OutMac[16];	
	uint8_t ret_code;
	
	uint8_t mode = 0xC0; // Use Serial Number

	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_check_decrypt_param;
	
	printf("================== Test EncRead & Decrypt ==================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Send EncRead Command
	ret_code	= aes132m_execute(AES132_OPCODE_ENC_READ, mode, AES132_USER_ZONE_ADDR(3), 0x0020, 0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("EncRead Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 32);
	
	// 3)	Decrypt Data
	// Calculate Nonce Memory
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(InData, OutData, sizeof OutData);

	mac_check_decrypt_param.opcode        = AES132_OPCODE_ENC_READ;
	mac_check_decrypt_param.mode          = mode;
	mac_check_decrypt_param.param1        = AES132_USER_ZONE_ADDR(3);
	mac_check_decrypt_param.param2        = sizeof(InData);
	mac_check_decrypt_param.count_value   = NULL;
	mac_check_decrypt_param.usage_counter = NULL;
	mac_check_decrypt_param.serial_num    = sn;
	mac_check_decrypt_param.small_zone    = sz;
	mac_check_decrypt_param.key           = key07;
	mac_check_decrypt_param.nonce         = &g_nonce;
	mac_check_decrypt_param.in_mac        = InMac;
	mac_check_decrypt_param.out_mac       = NULL;
	mac_check_decrypt_param.in_data       = InData;
	mac_check_decrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_check_decrypt(&mac_check_decrypt_param);
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Decrypted Data (Software)\r\n");
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	printf("Plain Text     : %s\n\r", OutData);
	aes_print_rc(ret_code);
}

/** \brief Loads a predefined key into the VolatileKey memory to be used with the KeuLoad Command
 * 
 * Key05 is the ParentKeyID and used for the cryptographic functions * 
 * Key06 is the ChildKeyId and is the 'target" for the load
 * Requires pre-auth with key 13 to run 
 */
void aes132_volatile_keyload(void)
{
	// Variable Initialization
	uint8_t InSeed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t Random[16];		
	uint8_t OutData[16];	
	uint8_t InMac[16];
	uint8_t OutMac[16];	
	uint8_t ret_code;
	
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;
	
	printf("================== Test VolatileKey keyload =================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Calculate Nonce
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	uint8_t param1_low =  (AES132_VOL_USAGE_AUTH_OK | AES132_VOL_USAGE_ENCRYPT_OK_1 | AES132_VOL_USAGE_DECRYPT_OK | AES132_VOL_USAGE_AUTH_COMPUTE);
	uint8_t param1_high = (AES132_VOL_USAGE_WRITE_COMPUTE | AES132_VOL_USAGE_DEC_READ);

	mac_compute_encrypt_param.opcode        = AES132_OPCODE_KEY_LOAD;
	mac_compute_encrypt_param.mode          = 0x00;  // copy key to VolatileKey
	mac_compute_encrypt_param.param1        = 0x01;  // ParentKeyID
	mac_compute_encrypt_param.param2        = (param1_high << 8) | param1_low;// 0x032B - VolUsage Table 4-3.
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;
	mac_compute_encrypt_param.serial_num    = NULL;
	mac_compute_encrypt_param.small_zone    = NULL;
	mac_compute_encrypt_param.key           = key01;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = key_vol;
	mac_compute_encrypt_param.out_data      = OutData;
	
	ret_code	= aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Encrypted MAC & Data (Software)\r\n");
	printf("key_vol        : 0x"); printf_puthex_array(key_vol, 16); printf("\r\n");
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// 3)	Send keyload Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(key_vol, OutData, 16);
	
	ret_code	= aes132m_execute(AES132_OPCODE_KEY_LOAD, 0x00, 0x0001, 0x032B,
	sizeof(InMac), InMac, sizeof(key_vol), key_vol, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("keyload Command\r\n");
	aes132_print_command_block(ret_code);
	aes_print_rc(ret_code);
}


void aes132_volatile_key_encrypt_decrypt(void)
{
	// Variable Initialization
	uint8_t InSeed[12]	= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00 };
	uint8_t Random[16];
	uint8_t InData[] = "Encrypt-Decrypt";
	uint8_t OutData[16];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	uint8_t ret_code;
	
	struct aes132h_nonce_in_out nonce_param;
	
	printf("============== Test Encrypt & Decrypt with VolatileKey ===========\r\n");
	printf("==================  VolatileKey for Encryption  ==================\r\n");
	printf("==================  VolatileKey for Decryption  ==================\r\n");
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable
	memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Send Encrypt Command
	uint16_t eKeyId = 0x00FF;
	uint16_t dKeyId = 0xFFFF; // encryption key 0x01, Decryption key 02
	ret_code	= aes132m_execute(AES132_OPCODE_ENCRYPT, 0x00, eKeyId, 0x0010,
	sizeof(InData), InData, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Encrypt Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save encrypted data and encrypted mac to local variable
	memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 16);
	
	// 3)	Send Inbound Nonce Command
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = InSeed;
	nonce_param.random  = Random;
	nonce_param.nonce   = &g_nonce;
	
	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	// Print Value
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	// Print MacCount
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	// Print Valid flag
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	// Print Random flag
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	// Copy NonceMemory to local variable
	memcpy(InSeed, g_nonce.value, 12);
	
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x00, 0x0000, 0x0000,
	sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// 4)	Send Decrypt Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(InData, OutData, 16);
	ret_code	= aes132m_execute(AES132_OPCODE_DECRYPT, 0X00, dKeyId, 0x0110,
	sizeof(InMac), InMac, sizeof(InData), InData, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Decrypt Command\r\n");
	aes132_print_command_block(ret_code);

	printf("Plain Text: '%s' \n\r", &g_rx_buffer[2]);
}
// Creates a random number when device is locked
// if device is unlocked, you get 16 bytes of 0xA5
void aes132_create_random_number(uint8_t * random){

	uint8_t ret_code;
	printf("================== Test Create Random Number ==================\r\n");
	ret_code	= aes132m_execute(AES132_OPCODE_RANDOM, 0x00, 0x0000, 0x0000,
	0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);

	printf("Random Command\r\n");
	aes132_print_command_block(ret_code);	
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;

	memcpy(random, &g_rx_buffer[2], 16);
}
//Read user zone testing
void aes132_read_user_zone(void)
{
	uint8_t ret_code;
	uint8_t data_len = 16;
	uint16_t addr;
	
	printf("==================== Test Read User Zone ===================\r\n");
	
	// BlockRead user zone #0
	addr = AES132_USER_ZONE_ADDR(0);
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("BlockRead User Zone #0\r\n");
	aes132_print_command_block(ret_code);
	printf("Plain Text: %s\n\r", &g_rx_buffer[2]);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// BlockRead user zone #1
	addr = AES132_USER_ZONE_ADDR(1);
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("BlockRead User Zone #1\r\n");
	aes132_print_command_block(ret_code);
	printf("Plain Text: %s\n\r", &g_rx_buffer[2]);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;

	// BlockRead user zone #2
	addr = AES132_USER_ZONE_ADDR(2);
	ret_code = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0x00, addr, data_len,
	0, NULL, 0, NULL, 0, NULL, 0, NULL,
	g_tx_buffer, g_rx_buffer);
	printf("BlockRead User Zone #2\r\n");
	aes132_print_command_block(ret_code);
	printf("Plain Text: %s\n\r", &g_rx_buffer[2]);
}


void aes132_write_user_zone_04(void){

	uint8_t ret_code;
	
	//aes132_nonce();
	//aes132_inbound_auth_key(14, key14, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK));
	
	uint8_t in_seed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00};
	uint8_t random[16];
	
	uint8_t in_data[32] = {0x54,0x6F,0x6F,0x20,0x4D,0x61,0x6E,0x79,0x20,0x53,0x65,0x63,0x72,0x65,0x74,0x73,0x2D,0x53,0x6E,0x65,0x61,0x6B,0x65,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t out_data[32];
	uint8_t InMac[16];
	uint8_t OutMac[16];
	uint8_t mode = 0x00;// 0xC0; // Use Serial Number & Small Zone
	
	struct aes132h_nonce_in_out nonce_param;
	struct aes132h_in_out mac_compute_encrypt_param;
	
	// 1)	Send Random Nonce Command
	ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(in_seed), in_seed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("Nonce Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	
	// Save Random to local variable(A0)
	memcpy(random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
	// 2)	Encrypt Data
	// Calculate Nonce Memory  (from previous Nonce before Encrypt)
	nonce_param.mode    = 0x01;
	nonce_param.in_seed = in_seed;
	nonce_param.random  = random;
	nonce_param.nonce   = &g_nonce;

	ret_code = aes132h_nonce(&nonce_param);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Calculated Nonce (Software)\r\n");
	printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
	printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
	printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");
	
	//uint8_t sn[8];
	//uint8_t sz[4];
	//aes132_read_serial(sn);
	//aes132_read_small_zone(sz,4);
	mac_compute_encrypt_param.opcode        = AES132_OPCODE_ENC_WRITE;
	mac_compute_encrypt_param.mode          = mode;
	mac_compute_encrypt_param.param1        = AES132_USER_ZONE_ADDR(4);
	mac_compute_encrypt_param.param2        = sizeof in_data;
	mac_compute_encrypt_param.count_value   = NULL;
	mac_compute_encrypt_param.usage_counter = NULL;
	mac_compute_encrypt_param.serial_num    = NULL;
	mac_compute_encrypt_param.small_zone    = NULL;
	mac_compute_encrypt_param.key           = key07;
	mac_compute_encrypt_param.nonce         = &g_nonce;
	mac_compute_encrypt_param.in_mac        = NULL;
	mac_compute_encrypt_param.out_mac       = OutMac;
	mac_compute_encrypt_param.in_data       = in_data;
	mac_compute_encrypt_param.out_data      = out_data;
	
	ret_code = aes132h_mac_compute_encrypt(&mac_compute_encrypt_param);
	aes_print_rc(ret_code);
	if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
	
	printf("Encrypted MAC & Data (Software)\r\n");
	printf("InData         : 0x"); printf_puthex_array(in_data, sizeof in_data); printf("\r\n");
	printf("OutMac         : 0x"); printf_puthex_array(OutMac, 16); printf("\r\n");
	printf("OutData        : 0x"); printf_puthex_array(out_data, 16); printf("\r\n");
	printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");
	
	// 3)	Send EncWrite Command
	// Copy encrypted data and encrypted mac to local variable
	memcpy(InMac, OutMac, 16);
	memcpy(in_data, out_data, sizeof in_data);
	
	ret_code	= aes132m_execute(AES132_OPCODE_ENC_WRITE, mode, AES132_USER_ZONE_ADDR(4), sizeof in_data, sizeof(InMac), InMac, sizeof(in_data), in_data, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
	printf("EncWrite Command\r\n");
	aes132_print_command_block(ret_code);	
}

void aes132_decrypt_read(void){
	
		// Variable Initialization
		uint8_t InSeed[12]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};
		uint8_t Random[16];
		uint8_t InData[16];
		uint8_t OutData[16];
		uint8_t InMac[16];
		uint8_t OutMac[16];
		uint8_t ret_code;
		uint8_t mode = 0x00;
		uint16_t dKeyID = 0x0007;
		struct aes132h_nonce_in_out nonce_param;
		
		printf("================== Test DecRead  ==================\r\n");
		
		// 1)	Send Random Nonce Command
		ret_code	= aes132m_execute(AES132_OPCODE_NONCE, 0x01, 0x0000, 0x0000, sizeof(InSeed), InSeed, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
		printf("Nonce Command\r\n");
		aes132_print_command_block(ret_code);
		if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
		
		// Save Random to local variable
		memcpy(Random, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
		
		// 2)	Send EncRead Command
		ret_code	= aes132m_execute(AES132_OPCODE_ENC_READ, mode, AES132_USER_ZONE_ADDR(4), 0x0010,
		0, NULL, 0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);
		printf("EncRead Command\r\n");
		aes132_print_command_block(ret_code);
		if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
		
		// Save encrypted data and encrypted mac to local variable
		memcpy(OutMac, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
		memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA+16], 16);
		
		// 3)	Decrypt Data
		// Calculate Nonce Memory
		nonce_param.mode    = 0x01;
		nonce_param.in_seed = InSeed;
		nonce_param.random  = Random;
		nonce_param.nonce   = &g_nonce;
		
		ret_code = aes132h_nonce(&nonce_param);
		if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
		
		printf("Calculated Nonce (Software)\r\n");
		printf("Value          : 0x"); printf_puthex_array(g_nonce.value, 12); printf("\r\n");
		printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n");
		printf("Valid          : 0x"); printf_puthex(g_nonce.valid); printf("\r\n");
		printf("Random         : 0x"); printf_puthex(g_nonce.random); printf("\r\n\n");	

		memcpy(InMac, OutMac, 16);
		memcpy(InData, OutData, sizeof OutData); // ciphered text

		// try decrypting software
		/*
		struct aes132h_in_out mac_check_decrypt_param;
		mac_check_decrypt_param.opcode        = AES132_OPCODE_ENC_READ;
		mac_check_decrypt_param.mode          = 0x00;
		mac_check_decrypt_param.param1        = AES132_USER_ZONE_ADDR(4);
		mac_check_decrypt_param.param2        = sizeof(InData);
		mac_check_decrypt_param.count_value   = NULL;
		mac_check_decrypt_param.usage_counter = NULL;
		mac_check_decrypt_param.serial_num    = NULL;
		mac_check_decrypt_param.small_zone    = NULL;
		mac_check_decrypt_param.key           = key07;
		mac_check_decrypt_param.nonce         = &g_nonce;
		mac_check_decrypt_param.in_mac        = InMac;
		mac_check_decrypt_param.out_mac       = NULL;
		mac_check_decrypt_param.in_data       = InData;
		mac_check_decrypt_param.out_data      = OutData;
		
		ret_code	= aes132h_mac_check_decrypt(&mac_check_decrypt_param);
		aes_print_rc(ret_code);
		if (ret_code != AES132_FUNCTION_RETCODE_SUCCESS) return;
		
		printf("Decrypted Data (Software)\r\n");
		printf("OutData        : 0x"); printf_puthex_array(OutData, 16); printf("\r\n");
		printf("MacCount       : 0x"); printf_puthex(g_nonce.value[12]); printf("\r\n\n");		
		printf("Plain Text     : %s\n\r", OutData);
		*/
		uint8_t firstBlock[6] = {mode, 0x00, 0x07, 0x00, 0x10, 0x01};		
		uint8_t secondBlock[16];		
		memset(&secondBlock, 0,16);

		ret_code = aes132m_execute(AES132_OPCODE_DEC_READ, mode, dKeyID, sizeof(InData), sizeof(firstBlock), firstBlock, 
		sizeof(secondBlock), secondBlock, sizeof(InMac), InMac, sizeof(InData), InData, g_tx_buffer, g_rx_buffer);

		printf("DecRead Command\r\n");
		aes132_print_command_block(ret_code);
		if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
		memcpy(OutData, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], sizeof OutData);	
		aes_print_buffer(OutData,sizeof OutData);
}

void aes132_crunch(uint16_t count, uint8_t *outResult){
	
	uint8_t in_seed[16]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,0x00};
	
	uint8_t ret_code = aes132m_execute(AES132_OPCODE_CRUNCH, 0x00, count, 0x00, 16, in_seed,
	0, NULL, 0, NULL, 0, NULL, g_tx_buffer, g_rx_buffer);

	printf("Crunch Command\r\n");
	aes132_print_command_block(ret_code);
	if (ret_code != AES132_DEVICE_RETCODE_SUCCESS) return;
	memcpy(outResult, &g_rx_buffer[AES132_RESPONSE_INDEX_DATA], 16);
	
		
}