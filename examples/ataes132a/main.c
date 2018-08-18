/**
 * Notes: the libtomcrypt lib was copied from project
 * C:\Users\pvallone\Documents\Atmel Studio\AtmelStudio\Atmel-42688-Atmel-Smart-Plug-Firmware-User-Guide_AT15735_ApplicationNote
 * All includes added to asf.h
 *
 * \brief Empty user application template
 *
 */
 #include "main.h"

int ret = 0;



int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);

	aes_configure_i2c();
	aes_configure_i2c_callbacks();
	delay_ms(20);
	struct aes132a_details mydetails;
	show_menu();
	uint8_t sn[8];
	char menuOneCommand;
	char input[32];
	char address_input[5];
	char count_input[2];

	while (1) {

		scanf("%c",&menuOneCommand);
		if(menuOneCommand == '0' || menuOneCommand == 'm'){
			show_menu();
		}else if (menuOneCommand == '1')
		{
			printf("** Scan i2c bus ** \n\r"); // require /r/n to trigger
			aes_scan_i2c();
			
		} else if(menuOneCommand == '2'){
			
			ret = aes_read_serial_number(sn);
			if(ret != 0){
				printf("aes_read_serial_number() failed with code %d ", ret);
				aes_print_rc(ret);
			} else {
				printf("Serial Number: ");
				aes_print_buffer(sn, 8);
			}
		} else if(menuOneCommand == '3'){
			ret = aes_get_aes132a_details(&mydetails);
			if(ret != 0){
				printf("aes_get_aes132a_details() failed with code %d ", ret);
				aes_print_rc(ret);				
			}
			printf("Serial Number:\t\t");
			for(uint8_t i = 0; i < 8;i++){
				printf("0x%02X ", mydetails.serial_number[i]);
			}
			printf("\n\rLockKeys:\t\t0x%02X %s\n\r", mydetails.lock_keys, mydetails.lock_keys != 0x55 ? "[Locked]" : "[Unlocked]");
			printf("LockSmall:\t\t0x%02X %s\n\r", mydetails.lock_small, mydetails.lock_small != 0x55 ? "[Locked]" : "[Unlocked]");
			printf("LockConfig:\t\t0x%02X %s\n\r", mydetails.lock_config, mydetails.lock_config != 0x55 ? "[Locked]" : "[Unlocked]");
			printf("Manufacturing Id:\t0x%02X%02X\n\r", mydetails.manufacturing_id[0], mydetails.manufacturing_id[1]);
		}else if(menuOneCommand == '4'){
			aes_read_configuration();
		}else if(menuOneCommand == '5'){
			ret = run_hardware_encryption_test();
			if(ret != 0){
				printf("run_hardware_encryption_test() failed with code %d ", ret);
				aes_print_rc(ret);
			}
		}else if(menuOneCommand == '6'){
			ret = run_hardware_decryption_test();
			if(ret != 0){
				printf("run_hardware_decryption_test() failed with code %d ", ret);
				aes_print_rc(ret);
			}
		}else if(menuOneCommand == '7'){
			ret =run_hardware_encryp_decrypt_test_random();
			if(ret != 0){
				printf("run_hardware_decryption_test_random() failed with code %d ", ret);
				aes_print_rc(ret);
				
			}
		}else if(menuOneCommand == '9'){
			ret = aes_get_aes132a_details(&mydetails);
			if(ret != 0){
				printf("aes_get_aes132a_details() failed with code %d ", ret);
				aes_print_rc(ret);
				
			}
			if(mydetails.lock_keys == 0x55 || mydetails.lock_config == 0x55){
				printf("Device is not locked. Can not run tests!\n\r");
				} else {
				ret = run_tests();
				printf("Completed Tests\n\r");
				if(ret != 0){
					printf("run_tests() failed with code %d ", ret);
					aes_print_rc(ret);
				}
			}
		}else if(menuOneCommand == 'A'){
			printf("Please enter a 32 character phrase: ");
			scanf("%s",input);
			uint8_t len = strlen(input);
			if(len <= 32){
				printf("\n\rYou entered %s for encryption/decryption\n\r", input);
				printf("Input Data Length: %d\n\r", len);
				uint8_t my_encrypted_data[32];
				uint8_t my_decrypted_text[len];
				ret = run_hardware_encryp_decrypt_test_input(input, my_encrypted_data, my_decrypted_text);
				if(ret != 0){
					printf("run_hardware_encryp_decrypt_test_input() failed with code %d ", ret);
					aes_print_rc(ret);
				}

			}else {
				printf("Length must be 16 characters or less!\n\r");
			}
			
		}else if(menuOneCommand == 'B'){
			uint8_t test[] = "Hello Test FreeZone Write";
			uint8_t out[sizeof test];
			ret = aes_write_to_userzone(0xF180, test, (sizeof test) + 2);
			if(ret != 0){
				printf("aes_write_to_userzone() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			delay_ms(10);
			printf("Writing '%s'\n\r", (char *)test);
			ret = aes_block_read_memory(0xF180, (sizeof test) + 2, out);
			if(ret != 0){
				printf("aes_block_read() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			printf("Read '%s'\n\r", (char *)out);
			
		}else if(menuOneCommand == 'C'){
			printf("Please enter a 32 character phrase: ");
			scanf("%s",input);
			uint8_t len = strlen(input);
			if(len <= 32){
				printf("\n\rYou entered %s for encryption write\n\r", input);
				printf("Input Data Length: %d\n\rEnter the Zone Address (0000-0FF8):", len);
				scanf("%s",address_input);
				int address = (int)strtol(address_input, NULL, 16);
				printf("\n\rYou Entered Address %d (0x%02X)\n\r", address,address);
				if(address + len > 0x1000){
					printf("User Zone Out OF Bounds Error\n\r");
					} else {
					ret = run_encrypted_write_test(input, address);
					if(ret != 0){
						printf("run_encrypted_write_test() failed with code %d ", ret);
						aes_print_rc(ret);
					}
				}				
			}
		}else if(menuOneCommand == 'D'){
			printf("\n\rEnter the Zone Address To Read (0000-0FF8):");
			scanf("%s",address_input);
			int address = (int)strtol(address_input, NULL, 16);
			printf("\n\rEnter Number of Bytes to read (1-32):");
			scanf("%s",count_input);
			int num = (int)strtol(count_input, NULL, 10);
			printf("\n\r");
			if(num < 1 || num > 32){
				printf("\n\rNumber of Bytes must be between 1-32!");
			} else {
				if(address + num > 0x1000){ // todo check this logic against zone and size
					printf("User Zone Out OF Bounds Error\n\r");
				}else {
					printf( "\n\rReading %d bytes from address 0x%04x\n\r", num, address);
					char mytext[32];
					ret = run_encrypted_read_test(mytext, address, num);
					if(ret != 0){
						printf("run_encrypted_read_test() failed with code %d ", ret);
						aes_print_rc(ret);
					}
					printf( "Encrypted read '%s' from address 0x%02X\n\r", mytext, address);

				}
				
			}
		}else if(menuOneCommand == 'E'){
			uint8_t test[] = "Sercret1235467";
			uint8_t out[sizeof test];
			uint16_t sm_address = SMALL_ZONE;			
			ret = aes_write_to_userzone(sm_address, test, (sizeof test) + 2);
			if(ret != 0){
				printf("aes_write_to_userzone() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			delay_ms(10);
			printf("Writing '%s'\n\r", (char *)test);
			ret = aes_block_read_memory(sm_address, (sizeof test) + 2, out);
			if(ret != 0){
				printf("aes_block_read() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			printf("Read '%s'\n\r", (char *)out);
		}else if(menuOneCommand == 'F'){	

			uint8_t test[] = "Hello Test Free Space Write";
			uint8_t out[sizeof test];
			uint16_t free_address =0xF180;
			ret = aes_write_to_userzone(free_address, test, (sizeof test) + 2);
			if(ret != 0){
				printf("aes_write_to_userzone() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			delay_ms(10);
			printf("Writing '%s'\n\r", (char *)test);
			ret = aes_block_read_memory(free_address, (sizeof test) + 2, out);
			if(ret != 0){
				printf("aes_block_read() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			printf("Read '%s'\n\r", (char *)out);
		}else if(menuOneCommand == 'G'){	
				
			uint8_t out[32];
			uint16_t sm_address = SMALL_ZONE;			
			ret = aes_block_read_memory(sm_address,32 , out);
			if(ret != 0){
				printf("aes_block_read() failed with code %d ", ret);
				aes_print_rc(ret);
			}
			printf("Read Small Zone: '%s'\n\r", (char *)out);

		}else if(menuOneCommand == '\n' || menuOneCommand == '\r' ){
			// do nothing
		}
		delay_ms(10);
		port_pin_toggle_output_level(LED0_PIN);
	}
}

int run_hardware_encryption_test(void){

	ret = register_cipher(&aes_desc);
	
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
	}

	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}

	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	uint8_t nonce[12];
	uint8_t random[16];
	uint8_t mac[16];
	uint8_t cipher_text[32];
	uint8_t plain_text[] = "Hello World!";
	uint8_t dec_plain_text[sizeof(plain_text)];

	struct atcah_aes132_decrypt_params_t dec_params;
	printf("\n\rCreate Random Nonce\n\r");

	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Calculate the value of the AES132A nonce register after a nonce command..\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// encrypt
	//uint8_t sm_zone[4] = "Serc"; // only use if small zone is loaded
	ret = aes_encrypt(AES_ENC_MODE_FLAG_SERIAL_NUM, 1, sizeof(plain_text), plain_text, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	dec_params.op_code = ENCYRPT;
	dec_params.mode = AES_ENC_MODE_FLAG_SERIAL_NUM;
	dec_params.param1 = 1; // KeyId
	dec_params.count = sizeof(plain_text);
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key01;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	
	//memcpy(dec_params.small_zone,sm_zone, 4);
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);
	
	if (ret != 0){
		printf("atcah_aes132_decrypt_from() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}

	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.\n\r");
	}
	printf("** Plain Text '%s'\n\r", plain_text);
	printf("** Software Decryption: '%s'\n\r", (char *) dec_plain_text);
	return SUCCESS;
}

/* 
	Run a Software encryption and hardware decryption 
	Uses Key 5
*/
int run_hardware_decryption_test(void){
	uint8_t enc_dec_key = 13;
	const uint8_t *key_for_encryp_decryp = key13;
	// key 13 for encryption/decrypt
	
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
	}
	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
		
	uint8_t nonce[12] = { 0x10, 0x01, 0x02, 0x03, 0xA4, 0x05, 0x06, 0x07, 0x08, 0x09, 0xAA, 0x0B };	
	uint8_t mac[16];
	uint8_t cipher_text[32];
	uint8_t plain_text[] = "Hello World!!";	
	uint8_t dec_plain_text[sizeof(plain_text)];

	printf("Software encrypt of '%s' data and calculate MAC\n\r", (char *) plain_text);
	
	struct atcah_aes132_encrypt_params_t enc_params;
	enc_params.op_code = DECRYPT;
	enc_params.mode = 0;
	enc_params.param1 = enc_dec_key; // dKeyId
	enc_params.count = sizeof(plain_text);
	enc_params.plain_text = plain_text;
	enc_params.key = key_for_encryp_decryp;
	enc_params.nonce = nonce;
	enc_params.is_nonce_rand = FALSE;
	enc_params.mfg_id = mfg_id;
	
	ret = atcah_aes132_encrypt_for(&enc_params, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_encrypt_for() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	ret = aes_create_pass_through_nonce(nonce);
	if (ret != 0){
		printf("aes_create_pass_through_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	ret = aes_decrypt(enc_params.mode, enc_params.param1, enc_params.count, mac, cipher_text, dec_plain_text);
	if (ret != 0){
		printf("aes_decrypt() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){

		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.");
	}
	printf("Decrypted Text: '%s' Done\n\r", (char *)dec_plain_text);
	
	return SUCCESS;

}

/* 
	Performs a hardware encryption and hardware & software decryption
	Uses Key 4 to encrypt and Key 5 for decrypt
*/
int run_hardware_encryp_decrypt_test_random(void){
	
	// key 4 for encryption/ 5 for decrypt
	
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
	}

	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	
	uint8_t nonce[12];
	uint8_t mac[16];
	uint8_t random[16];
	uint8_t cipher_text[32];
	uint8_t plain_text[] = "Hello World!!";
	uint8_t dec_plain_text[sizeof(plain_text)];
	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	
	printf("\n\r*** Hardware Encryption ***\n\r");
	printf("\n\rCreate Random Nonce\n\r");

	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	printf("Calculate the value of the AES132A nonce register after a nonce command..\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	// encrypt
	ret = aes_encrypt(0, 4, sizeof(plain_text), plain_text, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	struct atcah_aes132_decrypt_params_t dec_params;
	dec_params.op_code = ENCYRPT;
	dec_params.mode = 0;
	dec_params.param1 = 4; // KeyId
	dec_params.count = sizeof(plain_text);
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key05;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);	
	if (ret != 0){

		printf("atcah_aes132_decrypt_from() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){
		
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.\n\r");
	}
	printf("** Plain Text %s\n\r", plain_text);
	printf("Software Decryption: '%s'\n\r", (char *) dec_plain_text);

	// now try to decrypt in hardware	
	ret = aes_create_pass_through_nonce(nonce);
	if (ret != 0){
		printf("aes_create_pass_through_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	printf("Decrypting data with ATAES132A (client)...\n\r");
	uint16_t dec_key_id = ((4 << 8) | 0x05); // enkey = 4, dkey =5
	uint16_t dec_count = (1 << 8) | sizeof(plain_text); // MSB is the macCount
	
	ret = aes_decrypt(0, dec_key_id, dec_count, mac, cipher_text, dec_plain_text);
	if (ret != 0){
		printf("aes_decrypt() failed with code %d ", ret);
		aes_print_rc(ret);
		return 0;
	}
	printf("Decrypted From ATAES132a: '%s'\n\r", (char *)dec_plain_text);	
	return SUCCESS;
}

int run_hardware_encryp_decrypt_test_input(char *input, uint8_t *cipher_text, uint8_t *dec_plain_text){
	
	printf("\n\r*** Hardware Encryption / Decryption / Random Nonce / Include Serial Number ***\n\r");
	printf("\n\r*** Encryption with Key 4 - Decryption with Key 5 ***\n\r");
	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };	
	uint8_t nonce[12];
	uint8_t mac[16];
	uint8_t random[16];	
	uint8_t plain_text[strlen(input)];	
	uint8_t len = strlen(input);

	memcpy(plain_text, input, sizeof(plain_text));
	// key 4 for encryption/ 5 for decrypt	
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
	}
	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	//Calculate the value of the AES132A nonce register after a nonce command
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}	
	// encrypt it
	ret = aes_encrypt(0, 4, len, plain_text, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	struct atcah_aes132_decrypt_params_t dec_params;
	dec_params.op_code = ENCYRPT;
	dec_params.mode = 0;
	dec_params.param1 = 4; // KeyId
	dec_params.count = len;
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key05;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);
	if (ret != 0){

		printf("atcah_aes132_decrypt_from() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	if (memcmp(dec_plain_text, plain_text, len) != 0){		
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.\n\r");
	}
	//printf("** Plain Text %s\n\r", plain_text);
	printf("Decrypting data with Software: '%s'\n\r", (char *) dec_plain_text);

	// now try to decrypt in hardware
	ret = aes_create_pass_through_nonce(nonce);
	if (ret != 0){
		printf("aes_create_pass_through_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	uint16_t dec_key_id = ((4 << 8) | 0x05); // enkey = 4, dkey =5
	uint16_t dec_count = (1 << 8) | len; // MSB is the macCount
	
	ret = aes_decrypt(0, dec_key_id, dec_count, mac, cipher_text, dec_plain_text);
	if (ret != 0){
		printf("aes_decrypt() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Decrypted From ATAES132a: '%s'\n\r", (char *)dec_plain_text);	
	return SUCCESS;
}

int run_encrypted_read_test(char *outout, uint16_t address, uint8_t length){
	if(address + length > 0x1000){
		printf("User Zone Out OF Bounds Error\n\r");
		return ATCA_GEN_FAIL;
	}
	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	uint8_t random[16];
	uint8_t nonce[12];
	uint8_t mac[16];
	uint8_t cipher_text[32];
	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}	
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Creating random nonce for encrypted read...\n\r");	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);	
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("UnWrapping Nonce...\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code:: %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Nonce was unwrapped...\n\r");

	memset(&cipher_text, 0, sizeof cipher_text); //clear for sanity
	ret = aes_encrypt_read(AES_ENC_MODE_FLAG_SERIAL_NUM, address, length, mac, cipher_text);
	if (ret != 0){
		
		printf("Failed encrypt_read_test%d (0x%02X)\n\r", ret, ret);
		aes_print_rc(ret);
		return ret;
	}
	
	// decrypt it
	struct atcah_aes132_decrypt_params_t dec_params;
	dec_params.op_code = ENCREAD;
	dec_params.mode = AES_ENC_MODE_FLAG_SERIAL_NUM;
	dec_params.param1 = address;
	dec_params.count = length;
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key00;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	uint8_t dec_plain_text[length];
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);
	if (ret != 0){
		printf("Failed atcah_aes132_decrypt_from %d (0x%02X)\n\r", ret, ret);
		aes_print_rc(ret);
		return ret;
	}	
	memcpy(outout, &dec_plain_text, length);
	
	return SUCCESS;
}

int run_encrypted_write_test(char *input, uint16_t address){
	
	uint8_t len = strlen(input);

	if(address + len > 0x1000){
		printf("User Zone Out OF Bounds Error\n\r");
		return ATCA_GEN_FAIL;
	}

	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	uint8_t random[16];
	uint8_t nonce[12];
	uint8_t mac[16];
	uint8_t cipher_text[32];
	uint8_t plain_text[len];

	memcpy(plain_text,input,len);

	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// encrypt write example
	printf("Creating random nonce...\n\r");
	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND_RESEED, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	printf("Calculate the value of the AES132A nonce register after a nonce command..\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND_RESEED, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// Write data
	
	printf("Performing encrypted write of '%s'...\n\r", (char *) plain_text);

	// Calculate cipher text and mac for encrypted write
	struct atcah_aes132_encrypt_params_t enc_params;
	enc_params.op_code = ENCWRITE;
	enc_params.mode = AES_ENC_MODE_FLAG_SERIAL_NUM;
	enc_params.param1 = address;
	enc_params.count = sizeof(plain_text);
	enc_params.plain_text = plain_text;
	enc_params.key = key00;
	enc_params.nonce =  nonce;
	enc_params.is_nonce_rand = TRUE;
	enc_params.mfg_id = mfg_id;
	memcpy(enc_params.serial_num, mydetails.serial_number, sizeof(enc_params.serial_num));

	ret = atcah_aes132_encrypt_for(&enc_params, mac, cipher_text);
	
	if (ret != 0){
		printf("atcah_aes132_encrypt_for() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	ret = aes_encrypt_write(AES_ENC_MODE_FLAG_SERIAL_NUM, address, sizeof(plain_text), mac, cipher_text);
	if(ret != 0){
		printf("aes_encrypt_write() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	return SUCCESS;
}

int run_tests(void){
	/* Definitions */
	uint8_t in_seed[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	uint8_t random[16];
	uint8_t nonce[12];
	uint8_t mac[16];
	uint8_t cipher_text[32];
	uint8_t plain_text[] = "PhillyNJ2018";
	uint16_t dec_key_id;
	uint16_t dec_count;
	struct aes132a_details mydetails;
	ret = aes_get_aes132a_details(&mydetails);
	if (ret != 0){
		printf("aes_get_aes132a_details() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Serial Number:\t\t");
	for(uint8_t i = 0; i < 8;i++){
		printf("0x%02X ", mydetails.serial_number[i]);
	}
	printf("\n\rLockKeys:\t\t0x%02X %s\n\r", mydetails.lock_keys, mydetails.lock_keys != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("LockSmall:\t\t0x%02X %s\n\r", mydetails.lock_small, mydetails.lock_small != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("LockConfig:\t\t0x%02X %s\n\r", mydetails.lock_config, mydetails.lock_config != 0x55 ? "[Locked]" : "[Unlocked]");
	printf("Manufacturing Id:\t0x%02X%02X\n\r", mydetails.manufacturing_id[0], mydetails.manufacturing_id[1]);
	
	ret = register_cipher(&aes_desc);
	if (ret != 0){
		printf("register_cipher() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// encrypt write example
	printf("Creating random nonce...\n\r");
	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND_RESEED, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	uint16_t mfg_id = mydetails.manufacturing_id[0] << 8 | mydetails.manufacturing_id[1];
	printf("Calculate the value of the AES132A nonce register after a nonce command..\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND_RESEED, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// Write data
	
	printf("Performing encrypted write of '%s'...\n\r", (char *) plain_text);

	// Calculate cipher text and mac for encrypted write
	struct atcah_aes132_encrypt_params_t enc_params;
	enc_params.op_code = ENCWRITE;
	enc_params.mode = AES_ENC_MODE_FLAG_SERIAL_NUM;
	enc_params.param1 = 0x0000; // Address
	enc_params.count = sizeof(plain_text);
	enc_params.plain_text = plain_text;
	enc_params.key = key00;
	enc_params.nonce =  nonce;
	enc_params.is_nonce_rand = TRUE;
	enc_params.mfg_id = mfg_id;
	memcpy(enc_params.serial_num, mydetails.serial_number, sizeof(enc_params.serial_num));

	ret = atcah_aes132_encrypt_for(&enc_params, mac, cipher_text);
	
	if (ret != 0){
		printf("atcah_aes132_encrypt_for() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	ret = aes_encrypt_write(AES_ENC_MODE_FLAG_SERIAL_NUM, 0x0000, sizeof(plain_text), mac, cipher_text);
	if(ret != 0){
		printf("aes_encrypt_write() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	printf("Creating random nonce for encrypted read...\n\r");
	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);
	
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("UnWrapping Nonce...\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code:: %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Nonce was unwrapped...\n\r");

	memset(&cipher_text, 0, sizeof cipher_text); //clear for sanity
	ret = aes_encrypt_read(AES_ENC_MODE_FLAG_SERIAL_NUM, 0x0000, sizeof(plain_text), mac, cipher_text);
	if (ret != 0){
		
		printf("Failed encrypt_read_test%d (0x%02X)\n\r", ret, ret);
		aes_print_rc(ret);
		return ret;
	}
	
	// decrypt it
	struct atcah_aes132_decrypt_params_t dec_params;
	dec_params.op_code = ENCREAD;
	dec_params.mode = AES_ENC_MODE_FLAG_SERIAL_NUM;
	dec_params.param1 = 0x0000;
	dec_params.count = sizeof(plain_text);
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key00;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	uint8_t dec_plain_text[sizeof(plain_text)];
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);
	if (ret != 0){
		printf("Failed atcah_aes132_decrypt_from %d (0x%02X)\n\r", ret, ret);
		aes_print_rc(ret);
		return ret;
	}
	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.");
	}

	printf( "read '%s'\n\r", (char *)dec_plain_text);
	printf("\n\r*** Hardware Encryption ***\n\r");
	printf("\n\rCreate Random Nonce\n\r");
	
	ret = aes_create_nonce(AES_NONCE_MODE_RAND, random, in_seed);
	if (ret != 0){
		printf("aes_create_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Calculate the value of the AES132A nonce register after a nonce command..\n\r");
	ret = atcah_aes132_nonce(AES_NONCE_MODE_RAND, in_seed, mfg_id, random, nonce);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	// encrypt
	ret = aes_encrypt(0, 1, sizeof(plain_text), plain_text, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}

	dec_params.op_code = ENCYRPT;
	dec_params.mode = 0;
	dec_params.param1 = 1; // KeyId
	dec_params.count = sizeof(plain_text);
	dec_params.out_mac = mac;
	dec_params.out_data = cipher_text;
	dec_params.key = key01;
	dec_params.nonce = nonce;
	dec_params.is_nonce_rand = TRUE;
	dec_params.mfg_id = mfg_id;
	memcpy(dec_params.serial_num, mydetails.serial_number, sizeof(dec_params.serial_num));
	ret = atcah_aes132_decrypt_from(&dec_params, dec_plain_text);
	
	if (ret != 0){

		printf("atcah_aes132_decrypt_from() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	
	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.\n\r");
	}
	printf("** Plain Text %s\n\r", (char *)plain_text);
	printf("Software Decryption: '%s'\n\r", (char *) dec_plain_text);

	// now try to decrypt in hardware
	printf("Loading nonce for ATAES132A decrypt...\n\r");
	
	ret = aes_create_pass_through_nonce(nonce);
	if (ret != 0){
		printf("aes_create_pass_through_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	printf("Decrypting data with ATAES132A (client)...\n\r");
	dec_key_id = ((1 << 8) | 0x02); // enkey = 1, dkey =2
	dec_count = (1 << 8) | sizeof(plain_text); // MSB is the macCount
	printf("** Plain Text Before: %s \n\r", (char *)plain_text);
	
	ret = aes_decrypt(0, dec_key_id, dec_count, mac, cipher_text, dec_plain_text);
	if (ret != 0){
		printf("aes_decrypt() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}

	printf("** Plain Text After: %s \n\r", (char *)plain_text);
	printf("Decrypted Text: '%s'\n\r", (char *)dec_plain_text);
	printf("Software encrypt data and calculate MAC\n\r");
	// Software encrypt data and calculate MAC
	memset(nonce, 0, sizeof(nonce)); // All zero nonce :)
	enc_params.op_code = DECRYPT;
	enc_params.mode = 0;
	enc_params.param1 = 2; // dKeyId
	enc_params.count = sizeof(plain_text);
	enc_params.plain_text = plain_text;
	enc_params.key = key02;
	enc_params.nonce = nonce;
	enc_params.is_nonce_rand = FALSE;
	enc_params.mfg_id = mfg_id;
	ret = atcah_aes132_encrypt_for(&enc_params, mac, cipher_text);
	if (ret != 0){
		printf("atcah_aes132_encrypt_for() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	ret = aes_create_pass_through_nonce(nonce);
	if (ret != 0){
		printf("aes_create_pass_through_nonce() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	ret = aes_decrypt(enc_params.mode, enc_params.param1, enc_params.count, mac, cipher_text, dec_plain_text);
	if (ret != 0){
		printf("aes_decrypt() failed with code %d ", ret);
		aes_print_rc(ret);
		return ret;
	}
	if (memcmp(dec_plain_text, plain_text, sizeof(plain_text)) != 0){
		printf("Decrypt failed. Input plain-text doesn't match decrypted plain-text.");
	}
	printf("Decrypted Text: '%s'. Done\n\r", (char *)dec_plain_text);
	
	return SUCCESS;
}


void show_menu(void){

	printf("\n\r");
	printf("        _     ___   ___   _   ____  ___        \n\r");
	printf("       /_\\   | __| / __| / | |__ / |_  )  __ _ \n\r");
	printf("      / _ \\  | _|  \\__ \\ | |  |_ \\  / /  / _` |\n\r");
	printf("     /_/ \\_\\ |___| |___/ |_| |___/ /___| \\__,_|\n\r");
	printf("                                               \n\r");
	printf("\n\r********************************************\n\r");
	printf("***         ATAES132 Main Menu           ***\n\r");
	printf("********************************************\n\r");
	printf("* (L) = Device must be locked for running  *\n\r");
	printf("* (UL) = Device must be locked for running *\n\r");
	printf("* To show menu select '0', 'M' or 'm'     *\n\r");
	printf("* All EEPROM addresses are in hex. e.g. FE*\n\r");
	printf("********************************************\n\r");
	printf("0. Show Menu (Also press 'M' or 'm')\n\r");
	printf("1. Scan i2c Bus\n\r");
	printf("2. Read Serial Number\n\r");
	printf("3. Read Details\n\r");
	printf("4. Read Configurations\n\r");
	printf("5. Run Hardware Encryption Test (L)\n\r");
	printf("6. Run Hardware Decryption Test - Pass Through Nonce (L)\n\r");
	printf("7. Run Hardware Encryption/Decryption Test - Random Nonce (L)\n\r");
	printf("9. Run Encryption Tests (L)\n\r");
	printf("A. Encrypt/Decrypt Text Input (L)\n\r");
	printf("B. Write to Free Space (UL)\n\r");
	printf("C. Encrypted Write Text Input (L)\n\r");
	printf("D. Encrypted Read (L)\n\r");
	printf("E. Write/Read to Small Zone (UL)\n\r");
	printf("F. Write/Read to Free Space (UL)\n\r");
	printf("G. Read to Small Zone\n\r");

}