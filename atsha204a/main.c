/*
	The SAMD21_ATSHA204a_advanced demo
	This demo provides a bunch of examples to preform Host/Client Validation
	
	Open a Terminal:
	
	Speed: 9600
	Data: 8 bit
	Parity: none
	Stop bits: 1 bit
	Flow control: none
	
	Type (00) to display the menu
	For a new unlocked ATSHA204a, follow steps 1-8. This steps will configure and provision the ATSHA240a
	for this demo.
	
	*******
	WARNING - Once you lock the ATSHA240a, it can not be unlocked!
	*******

*/
#include <asf.h>

#include "sercom_usart_config.h"
#include "cryptoauthlib.h"
#include "atsha204a_config.h"
#include "atsha204_impl.h"
#include "print_helpers.h"
#include "utility.h"
#include "main.h"

/* CryptoAuthLib Naming Prefixes from Atmel-8984-CryptoAuth-CryptoAuthLib-ApplicationNote.pdf

Table 3-1. CryptoAuthLib Naming Conventions
-------------------------------------------
Methods				Prefix
------------------------------
Core Datasheet		API atca_
Basic				API atcab_
Helpers
------------------------------
Crypto				atcac_
TLS					atcatls_
Cert				atcacert_
Host				atcah_

*/

// the root key is the same for all tutorials and examples to keep things simple
uint8_t root[32] = {0x4D, 0x79, 0x53, 0x65, 0x63, 0x72, 0x65, 0x74, 0x6B, 0x65, 0x79, 0x32, 0x30, 0x31, 0x38, 0x40, 0x31, 0x32, 0x34, 0x36, 0x38, 0x41, 0x64, 0x76, 0x68, 0x6B, 0x6D, 0x6D, 0x6C, 0x6F, 0x79, 0x33};
int key_04_roll_count = 0;
int main (void)
{
	system_init();
	delay_init();
	configure_console(9600);
	printf("ATSHA204a Advanced Examples\n\r");
	
	status = atcab_init(&cfg_atsha204a_i2c_default);	
	
	if(status != ATCA_SUCCESS){
		 printf("Init Error: 0x%02X\n\r", status);
		 return 0;
	}
	
	sha204_print_menu();	
	
	while (1) {
		if (scanf("%c",&serial_data)>0){
			if (parse_command(serial_data)){
				sha204_app_command_dispatch();
			}
		};
	}
}

void sha204_app_command_dispatch(void)
{
	
	printf("You entered command %02X\n\r",g_packet_block[0] );
	switch (g_packet_block[0]) {	
		case 0x00:
			sha204_print_menu();
		break;
		case 0x01:
			
			status = sha204_get_details(&myDetails); // we grab the details and save them
			RET_ERR(status, "Unable to get details");
					
			
			printf("- SerialNumber:\t\t");
			print_buffer(myDetails.serial_number, 9);
			printf("- RevNumber:\t\t");
			print_buffer(myDetails.rev_num, 4);
			printf("- LockValue:\t\t0x%02X [%s]\n\r", myDetails.lock_value, myDetails.lock_value != 0x55 ? "Locked" : "unlocked");
			printf("- LockConfig:\t\t0x%02X [%s]\n\r", myDetails.lock_config, myDetails.lock_config != 0x55 ? "Locked": "unlocked");
			
			printf("- I2c Address:\t\t0x%02X\n\r", myDetails.i2c_address);
			printf("- CheckMacConfig:\t0x%02X\n\r", myDetails.check_mac_config);
			printf("- OTP Mode:\t\t0x%02X\n\r", myDetails.otp_mode);
			printf("- Selector Mode:\t0x%02X\n\r", myDetails.select_mode);				
		break;
		case 0x02:
			sha204_read_config();
		break;
		case 0x03:
			status = sha204_personalize();
			if(status != ATCA_SUCCESS){
				printf("- Error: 0x%02X\n\r", status);
			} else {
				printf("- Personalization Complete\n\r");
			}
		break;
		case 0x04:
		// lock config
		status = atcab_read_zone(ATCA_ZONE_CONFIG,0,2,5,read_data, 4);
		RET_ERR(status, "Unable to read Lock Status");
		
		print_buffer(read_data, 4);
		
		if(read_data[3] == 0x55){
			printf("Are you sure you want to lock the Configuration Zone? [Y/n]");
			scanf("%c",&input);
			if(input == 'Y'){				
				printf("\n\rLocking Configuration Zone...\n\r");
				read_data[3] = 0x00; // LockConfig;
				status= atcab_lock_config_zone();
				RET_ERR(status, "Unable to Lock ConfigZone");
				
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,2,5,read_data, 4);
				RET_ERR(status, "Unable to read Lock Status");
				
				printf("LockConfig 0x%02X\n\r", read_data[3]);
				print_buffer(read_data, 4);
			} else {
				printf("\n\rAction Canceled\n\r");
			}
		} else {
			printf("LockConfig is already locked");
		}
		break;
		case 0x05:
			// read the lock status first (write/read operations)
			status = sha204_get_details(&myDetails);
			RET_ERR(status, "Unable to read device");
			
			if(myDetails.lock_config == 0x55){
				printf("- Lock Config Must be Locked to perform function\n\r");
				return;
			}
			if(myDetails.lock_value != 0x55){
				printf("- OPT Zone cannot be written to if OTP is Locked already\n\r");
				return;
			}
			memset(&write_data[0], 0, sizeof(write_data));
			memcpy(&write_data[0], &otp_data_1[0], strlen(otp_data_1));
			status = sha204_write_otp_zone(write_data, sizeof(write_data), 0);
			
			RET_ERR(status, "Failed To Write to OTP");
			
			memset(&write_data[0], 0, sizeof(write_data));
			memcpy(&write_data[0], &otp_data_2[0], strlen(otp_data_2));
		
			status = sha204_write_otp_zone(write_data, sizeof(write_data), 1);
			RET_ERR(status, "Failed To Write to OTP");
			
			printf("- Write to OTP Zone Complete\n\r");
		
		break;
		case 0x06:
			status = sha204_write_keys(root);
			if (status != ATCA_SUCCESS)
			{
				printf("- Error Writing Keys: ");
				sha204_parser_rc(status);
			} else {
				printf("- Writing to Key Slots Complete\n\r");
			}
		break;
		case 0x07:
				// LockConfig Must be locked first
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,2,5,read_data, 4);
				RET_ERR(status, "Unable to read Lock Status");				
				
				print_buffer(read_data, 4);
				if(read_data[3] == 0x55){
					printf("- LockConfig must be locked first\n\r");
					return;
				}
				if(read_data[2] != 0x55){
					printf("- OTP and Data Zones are already locked!\n\r");
					return;
				}
				printf("Are you sure you want to lock the OTP and Data Zones? MAKE SURE YOU WRITE THE KEYS FIRST! [Y/n]");
				scanf("%c",&input);
				if(input == 'Y'){

					printf("\n\rLocking the OTP and Data Zone...\n\r");
					status = atcab_lock_data_zone();
					RET_ERR(status, "Unable to Lock Data Zone");		
					
					printf("- OTP and Data Zone Locked\n\r");
				} else {
					printf("\n\r- Action Canceled\n\r");
				}
		break;
		case 0x08:
			
			// need to check the locks before reading
			status = sha204_get_details(&myDetails); // we grab the details and save them
			RET_ERR(status, "Unable to get details");	
			
			if(myDetails.lock_value == 0x55 || myDetails.lock_config == 0x55){
				printf("- Error! The Config and OTP zones must be locked to read the OTP Zone!");
				return;
			}
		
			status = sha204_read_otp_zone(32, 0, read_data);
			RET_ERR(status, "Unable to read OTP");
			
			printf("- OTP Zone Block 0: ");
			print_buffer(read_data, sizeof(read_data));
			
			status = sha204_read_otp_zone(32, 1, read_data);
			RET_ERR(status, "Unable to read OTP");
			
			printf("- OTP Zone Block 1: ");
			print_buffer(read_data, sizeof(read_data));
			
		break;
		
		case 0x09:
			// create a diversified key 
			memset(&read_data, 0, sizeof(read_data));

			for(uint8_t i = 0; i < 16;i++){
				// create a key for each slot
				status= sha204_create_diverse_key(root, i, read_data, 0x77);
				if (status != ATCA_SUCCESS)
				{
					printf("- Unable to calculate key: ");
					sha204_parser_rc(status);
					
				} else {
					printf("- Key %d: ", i);
					print_buffer(read_data, sizeof read_data);
				}
			}		
		break;
		case 0xA0:
			//SlotConfig<4> aka CheckMac must be 0
			slot = 12;
			printf("- HMAC on slot %d\n\r", slot);
			// key 12 is diversified, so we calculate it first
			status= sha204_create_diverse_key(root, 12, read_data, 0x77);
			RET_ERR(status, "Unable to calculate key");
			
			status = sha204_hmac_challenge(0x000C, read_data);
			if(status != 0){
				printf("- Failed Match\n\r");
			} else {
				printf("- Slot %d Success!\n\r",slot);
			}		
			
			slot = 3;
			printf("- HMAC on slot %d\n\r", slot);
			memset(&read_data, 0, sizeof(read_data));
			status= sha204_create_diverse_key(root, slot, read_data, 0x77);
			RET_ERR(status, "Unable to calculate key");
					
			status = sha204_hmac_challenge(slot, read_data);
			if(status != 0){
				printf("- Failed Match\n\r");
			} else {
				printf("- Slot %d Success!\n\r",slot);
			}	
					
		break;
		case 0xA1:
			status = sha204_gendig_example(0x09, 0x00);
			RET_ERR(status, "Unable to run gendig/checkmac example");			
			printf("- Challenge Response: %s\n\r", status == 0 ? "Success" : "Mismatch" );
					
			status = sha204_gendig_example(0x07, 0x00);
			RET_ERR(status, "Unable to run gendig/checkmac example");			
			printf("- Challenge Response: %s\n\r", status == 0 ? "Success" : "Mismatch" );	
			
		break;
		case 0xA2:
		
		
			printf("- Enc Read Key 4: ");
			// calculate key 0x09 (parent key) for the encryption
			status= sha204_create_diverse_key(root, 0x09, parent_key, 0x77);
			RET_ERR(status, "Unable to calculate key");
			
			status = atcab_read_enc(0x0004, 0, read_data, parent_key,0x09);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot 4: ");
				sha204_parser_rc(status);
			} else {
				printf("- Key: ");
				print_buffer(read_data, sizeof(read_data));
			}		
			
			printf("- Enc Read Key 7: ");
			status = atcab_read_enc(0x0007, 0, read_data, root,0x0A);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot 7: ");
				sha204_parser_rc(status);
				
			} else {
				printf("- Key: ");
				print_buffer(read_data, sizeof(read_data));
			}	
			
			// slot config must be IsSecret = 1 and EncryptRead = 1
			// enc_key_id must match the SlotConfig<Key_id>.ReadKey
			printf("- Enc Read Key 8: ");
			status = atcab_read_enc(0x0008, 0, read_data, root,0x0A);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot: ");
				sha204_parser_rc(status);
				
			} else {
				printf("- Key: ");
				print_buffer(read_data, sizeof(read_data));
			}
			
			printf("- Enc Read Key 12: ");
			status = atcab_read_enc(0x000C, 0, read_data, root,0x0A);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot 12: ");
				sha204_parser_rc(status);
				
			} else {
				printf("- Key: ");
				print_buffer(read_data, sizeof(read_data));
			}
			
			printf("- Enc Read Key 13: ");
			status = atcab_read_enc(0x000D, 0, read_data, root,0x0A);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot 13: ");
				sha204_parser_rc(status);				
			} else {
				printf(" Key: ");
				print_buffer(read_data, sizeof(read_data));
			}		
			
			printf("- Enc Read Key 15: ");
			// calculate key 0x0D (parent key) for the encryption
			status= sha204_create_diverse_key(root, 0x0D, parent_key, 0x77);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to calculate key: ");
				sha204_parser_rc(status);				
			}				
			
			status = atcab_read_enc(0x000F, 0, read_data, parent_key,0x0D);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to read slot 15: ");
				sha204_parser_rc(status);
			} else {
				printf("- Key: ");
				print_buffer(read_data, sizeof(read_data));
			}
			
			
		break;
		case 0xA3:
		
			status= sha204_create_diverse_key(root, 0x00, read_data, padding);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to calculate key: ");
				sha204_parser_rc(status);
				
			} else {
				printf("- Diverse Key 0: ");
				print_buffer(read_data, sizeof read_data);
			}
		
			status= sha204_create_diverse_key(root, 0x07, read_data, padding);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to calculate key: ");
				sha204_parser_rc(status);
				
			} else {
				printf("- Diverse Key 7: ");
				print_buffer(read_data, sizeof read_data);
			}
			
			
		break;
		case 0xA4:
			slot = 0x00;
			// create the derived key in sw and uses it to challenge the SHA204a
			status = sha204_create_diverse_key(root, slot, write_data, padding);
			RET_ERR(status, "Unable to create diverse key");
					
			printf("- Key: ");
			print_buffer(write_data, 32);
			status = sha204_random_challenge(slot, write_data);
					
			printf("- Challenge Response: %s\n\r", status == 0 ? "Success" : "Mismatch" );	
				
		break;
		case 0xA5:
			slot = 0x01;
			mode = MAC_MODE_INCLUDE_SN | MAC_MODE_INCLUDE_OTP_88;
			status = atcab_read_serial_number(serialNumber);
			RET_ERR(status, "Unable to read serial number");
			
			// create the key - needed for the SW digest
			status = sha204_create_diverse_key(root, slot, read_data, padding);
			RET_ERR(status, "Unable to create diverse key");
			
			// get the otp data
			status = sha204_read_otp_zone(sizeof(otp_data), 0, otp_data); //32 length read
			RET_ERR(status, "Unable to read OTP");
						
			// request a random number for the challenge			
			status = atcab_random(random_data);
			RET_ERR(status, "Unable to create random number");			
			
			// create sw digest			
			memset(request, 0, sizeof request); // set all with 0
			memcpy(request, read_data, 32); // copy over the diverse key
			memcpy(&request[32], &random_data, 32); // copy over the challenge
			
			request[64] = ATCA_MAC; //opcode
			request[65] = mode; //param1 mac mode
			request[66] = slot; //param2 upper byte Key Slot ID
			request[67] = 0x00; //param2 Lower byte always 00
			
			memcpy(&request[68], &otp_data[0], 8); // copy over OTP data
			memcpy(&request[76], &otp_data[8], 3);
			
			request[79] = serialNumber[8]; //0xEE; // SN<8>
			memcpy(&request[80], &serialNumber[4], 4);
			request[84] = serialNumber[0]; //0x01; // SN<0:1>
			request[85] = serialNumber[1]; //0x23; // SN<0:1>
			request[86] = serialNumber[2];
			request[87] = serialNumber[3];
			
			// create the sw digest 
			status = atcac_sw_sha2_256(request, 88, sw_digest);
			RET_ERR(status, "nable to create digest");			
						
			printf("- Challenge: ");
			print_buffer(random_data, 32);
			// preform hw challenge with MAC opcode
			status = sha204_random_mac_challenge(slot, mode, random_data, sw_digest);
				
			printf("- SW Digest: ");
			print_buffer(sw_digest, sizeof(sw_digest));
			
			printf("- Challenge Response: %s\n\r", status == 0 ? "Success" : "Mismatch" );
			
		break;
		case 0xA6:
			// simulates host/client where the key in host slot x, is the same as the key in client slot x
			// in this case we use the slots 6 (client) & 7 (host) because the same. In this case the root key. 
			
			status = sha204_checkmac_example(0x07, 0x06);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to run sha204_checkmac_example: ");				
			}else {				
				printf("- Success!\n\r");
			}
			
		break;
		case 0xA7:
						
			/*
			Create the diverse key on the atsha204a	Target Slot 0x07
			Parent Key (e.g. Write Key of Target Key) is 0x0B - see config
			
			The device combines the current value of a key with the nonce stored in TempKey using SHA-256 and
			places the result into the target key slot. SlotConfig<TargetKey>.Bit13 must be set or DeriveKey returns
			an error.
			
			SlotConfig<TargetKey>.Bit12 is one for this example, the source key is the parent key of the target key, 
			which is found in SlotConfig<TargetKey>.WriteKey (Create Key operation).
			*/
			
			slot = 0x07;
			status=sha204_diverse_key_example(slot, NULL, padding);
			if (status != ATCA_SUCCESS)
			{
				printf("- Unable to run diverse key: ");
				sha204_parser_rc(status);
			
			} else {
				// if this where it fail, the The mac key (0x06), must be the same as the diverse key in slot 7. 
				// this is based on the App Note
				printf("- Success - now running CheckMac\n\r");
				status = sha204_checkmac_example(slot, 0x06); // the mac key must have checkonly=0 to run mac
				if (status != ATCA_SUCCESS)
				{
					printf("Unable to run CheckMac: ");	
					sha204_parser_rc(status);				
				} else {
					
					printf("- Success!\n\r");
				}				
			}
			// check how often this has been run
			status = atcab_read_zone(ATCA_ZONE_CONFIG,0,2,0,read_data, 4);
			if(status != ATCA_SUCCESS){
				sha204_parser_rc(status);
				return;
			}
			printf("- UpdateCount on slot %d is %d \n\r", slot, read_data[3]);		
					
			
			break;
			case 0xA8:
			
				//mode = MAC_MODE_INCLUDE_SN | MAC_MODE_INCLUDE_OTP_88;
				mode = MAC_MODE_INCLUDE_SN;
				memset(&read_data, 0, sizeof(read_data)); // set all to zeros
				memset(&serialNumber, 0, sizeof(serialNumber));
				memset(&random_data, 0, sizeof(random_data));
				
				slot = 1; // slot config must have CheckOnly = 0
				// We use diversified Key 01, so we need to calculate it
				status= sha204_create_diverse_key(root, slot, read_data, 0x77);
				if (status != ATCA_SUCCESS)	{printf("- Unable to calculate key: ");sha204_parser_rc(status);}
				
				// get the SN
				status = atcab_read_serial_number(serialNumber); // get the serial number
				if (status != ATCA_SUCCESS){printf("- Unable to calculate key: ");sha204_parser_rc(status);}
				
				//Calculate a challenge to test against
				status = atcab_random(random_data);
				if(status != ATCA_SUCCESS){printf("- Unable to calculate random challenge:");	sha204_parser_rc(status);}
				// generate the mac via ATSHA204a
				status = atcab_mac(mode, 0x00 + slot, random_data, digest);
				if (status != ATCA_SUCCESS)
				{
					printf("- Unable to create mac:");
					sha204_parser_rc(status);
				}
				// generate the mac on the software side
				memset(&request, 0, sizeof request); // set all with 0
				memcpy(&request, &read_data, 32); // diverse key
				memcpy(&request[32], &random_data, 32); // challenge
				
				request[64] = ATCA_MAC;
				request[65] = mode; // mode
				request[66] = slot;
				request[67] = 0x00;
				request[79] = serialNumber[8]; //0xEE; // SN<8>
				
				request[80] = serialNumber[4];
				request[81] = serialNumber[5];
				request[82] = serialNumber[6];
				request[83] = serialNumber[7];
				
				request[84] = serialNumber[0]; //0x01; // SN<0:1>
				request[85] = serialNumber[1]; //0x23; // SN<0:1>
				request[86] = serialNumber[2];
				request[87] = serialNumber[3];
				
				status = atcac_sw_sha2_256(request, sizeof(request), sw_digest);
				if (status != ATCA_SUCCESS)	{sha204_parser_rc(status);printf("- Unable to create digest!!\n\r");}
				
				printf("- SW HASH: ");
				print_buffer(sw_digest, sizeof sw_digest);
				
				printf("- HW HASH: ");
				print_buffer(digest, sizeof digest);
				sha204_compare_digests(digest, sw_digest, sizeof(sw_digest));
			break;			
			case 0xA9:
			
				// this example preforms a challenge and uses the value in calculated in temp key, full sn and 11 bytes of the otp data
				
				memset(&calculated_key, 0, sizeof(calculated_key)); // set all to zeros				
				memset(&serialNumber, 0, sizeof(serialNumber));
				memset(&random_data, 0, sizeof(random_data));
				memset(&num_in, 0, sizeof(num_in));
				memset(&otp_data, 0, sizeof(otp_data));
				memset(&temp_key_in, 0, sizeof(temp_key_in));
				memset(&temp_key_out, 0, sizeof(temp_key_out));
				memset(&nonce_params, 0, sizeof(nonce_params));
						
				slot = 8;	
				printf("- Running Challenge against slot %d\n\r", slot);
				//The second 32 bytes are filled with the value in TempKey. Use the full SN
				mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN | MAC_MODE_INCLUDE_OTP_88;
			
				printf("- Mode: 0x%02X (MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_INCLUDE_SN | MAC_MODE_INCLUDE_OTP_88)\n\r", mode);
										
				// get the SN
				status = atcab_read_serial_number(serialNumber); // get the serial number
				if (status != ATCA_SUCCESS)	{printf("- Unable to calculate key: ");sha204_parser_rc(status);}			
			
				// get the otp data
				status = sha204_read_otp_zone(sizeof(otp_data), 0, otp_data); // 88 length
				if (status != ATCA_SUCCESS){printf("- Unable to read OTP: ");	sha204_parser_rc(status);}						
			
				//Calculate a challenge to test with
				status = atcab_random(random_data);
				if(status != ATCA_SUCCESS){printf("- Unable to calculate random challenge:");	sha204_parser_rc(status);}					
		
				memcpy(&num_in[0], &random_data, sizeof(num_in));
				// We use diversified Key 08, so we need to calculate it
				status = sha204_create_diverse_key(root, slot, calculated_key, padding);
				if (status != ATCA_SUCCESS){printf("- Unable to calculate key: ");sha204_parser_rc(status);return;}
		
				memset(&nonce_params, 0, sizeof(nonce_params));
				nonce_params.mode = NONCE_MODE_SEED_UPDATE;
				nonce_params.zero = 0;
				nonce_params.num_in = num_in;
				nonce_params.temp_key = &temp_key;
				nonce_params.rand_out = rand_out;
			
				status = atcab_nonce_rand(nonce_params.num_in, rand_out);
				if (status != ATCA_SUCCESS){printf("- Unable to create nonce:");sha204_parser_rc(status);return;}
			
				status = atcah_nonce(&nonce_params);
				if (status != ATCA_SUCCESS)	{printf("- Unable to create nonce: \n\r");	sha204_parser_rc(status);return;}
			
				// generate the mac via ATSHA204a
				status = atcab_mac(mode, 0x00 + slot, random_data, digest);
				if (status != ATCA_SUCCESS)	{printf("- Unable to create mac:");sha204_parser_rc(status);return;}			
			
				memset(&temp_key_out,0, sizeof(temp_key_out));
				memset(&temp_key_in,0, sizeof(temp_key_in));			
				memcpy(&temp_key_in[0], &rand_out[0], sizeof(rand_out));
				memcpy(&temp_key_in[32], &num_in[0], sizeof(num_in)); // 20
			
				temp_key_in[52] = ATCA_NONCE;
				temp_key_in[53] = NONCE_MODE_SEED_UPDATE;
				temp_key_in[54] = 0x00;
				// need to calculate the key out value 
				status = atcac_sw_sha2_256(temp_key_in, sizeof(temp_key_in), temp_key_out);
				if (status != ATCA_SUCCESS)	{sha204_parser_rc(status);printf("- Unable to create digest!!\n\r");return;}						
			
				// generate the mac on the software side
				memset(&request, 0, sizeof request); // set all with 0
				memcpy(&request, &calculated_key, 32); // diverse key			
				memcpy(&request[32], &temp_key_out, 32); // challenge
		
				request[64] = ATCA_MAC;
				request[65] = mode; // mode
				request[66] = slot;
				request[67] = 0x00;			
				memcpy(&request[68], &otp_data[0], 8);
				memcpy(&request[76], &otp_data[8], 3);
				request[79] = serialNumber[8]; //0xEE; // SN<8>		
				request[80] = serialNumber[4];
				request[81] = serialNumber[5];
				request[82] = serialNumber[6];
				request[83] = serialNumber[7];		
				request[84] = serialNumber[0]; //0x01; // SN<0:1>
				request[85] = serialNumber[1]; //0x23; // SN<0:1>
				request[86] = serialNumber[2];
				request[87] = serialNumber[3];
		
				status = atcac_sw_sha2_256(request, sizeof(request), sw_digest);
				if (status != ATCA_SUCCESS)	{sha204_parser_rc(status);printf("- Unable to create digest!!\n\r");return;}
		
				printf("- SW HASH: ");
				print_buffer(sw_digest, sizeof sw_digest);
		
				printf("- HW HASH: ");
				print_buffer(digest, sizeof digest);
				sha204_compare_digests(digest, sw_digest, sizeof(sw_digest));
		
			break;
			
			case 0xAA:	
			
				status = sha204_validate_keys(root);				
				RET_ERR(status, "sha204_validate_keys failed");
			break;
			case 0xAB:
				
				slot = 5;
				//SlotConfig<15> must be set to 1
				printf("- Running DeriveKey with a MAC on slot %d\n\r", slot);				
				//calculate the key for the mac - Parent key required for MAC which is Slot 2 (0x02)
				status = sha204_create_diverse_key(root, 0x02, read_data, padding);
				RET_ERR(status, "Unable to calculate key");	
								
				// get the SN
				status = atcab_read_serial_number(serialNumber); // get the serial number
				RET_ERR(status, "Unable to get SN");	
				
				// mac is required to run dervied key on this slot
				memset(&derive_key_mac, 0, sizeof (derive_key_mac));
				memcpy(&derive_key_mac[0], &read_data[0], sizeof(derive_key_mac));				
				derive_key_mac[32] = ATCA_DERIVE_KEY;	
				derive_key_mac[33] = DERIVE_KEY_MODE;
				derive_key_mac[34] = slot;
				derive_key_mac[35] = 0x00;				
				derive_key_mac[36] = serialNumber[8];	
				derive_key_mac[37] = serialNumber[0];
				derive_key_mac[38] = serialNumber[1];				 
				status = atcac_sw_sha2_256(derive_key_mac, sizeof(derive_key_mac), sw_digest);				
				
				RET_ERR(status, "Unable to create digest");	
				status=sha204_diverse_key_example(slot, sw_digest, padding);
				
				if (status != ATCA_SUCCESS)	{
					printf("- Unable to run diverse key: ");
					sha204_parser_rc(status);
					return;
				}else{
					printf("- Success. Key created.\n\r");	
					status = sha204_validate_derived_key(root, slot, 0x77, read_data);
					if (status != ATCA_SUCCESS)	 { printf("- GenDig/CheckMac Failed:"); sha204_parser_rc(status); return;}	
					printf("- Success. Key checked with GenDig/CheckMac.\n\r");					
				}		
				
			break;
			case 0xAC:
			
				slot = 4;
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,1,7,read_data, 4);
				RET_ERR(status, "Unable to read zone");
				
				printf("- UpdateCount on slot %d (Block 1 - Offset 7) = %d\n\r", slot, read_data[1]);
				
				slot = 5;			
				// read config UpdateCount for 5 
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,1,7,read_data, 4);
				RET_ERR(status, "Unable to read zone");
				
				printf("- UpdateCount on slot %d (Block 1 - Offset 7) = %d\n\r", slot, read_data[3]);
				slot = 7;	
				status = atcab_read_zone(ATCA_ZONE_CONFIG,0,2,0,read_data, 4);
				RET_ERR(status, "Unable to read zone");
				printf("- UpdateCount on slot %d (Block 2 - Offset 0) = %d\n\r", slot, read_data[3]);				
					
				break;
			case 0xAD:
				slot = 4;
				//SLotConfig<15> must be set to 1 and SLotConfig<12> = 0
				printf("- Running DeriveKey with a MAC on slot %d\n\r", slot);
				//calculate the key for the mac - Parent key required for MAC which is Slot 2 (0x02)
				status = sha204_create_diverse_key(root, 0x02, read_data, padding);				
				RET_ERR(status, "Unable to calculate key");
				// get the SN
				status = atcab_read_serial_number(serialNumber); // get the serial number	
				RET_ERR(status, "Unable to read sn");
				
				memset(&derive_key_mac, 0, sizeof (derive_key_mac));
				memcpy(&derive_key_mac[0], &read_data[0], sizeof(derive_key_mac));
				derive_key_mac[32] = ATCA_DERIVE_KEY;
				derive_key_mac[33] = DERIVE_KEY_MODE;
				derive_key_mac[34] = slot;
				derive_key_mac[35] = 0x00;				
				derive_key_mac[36] = serialNumber[8];
				derive_key_mac[37] = serialNumber[0];
				derive_key_mac[38] = serialNumber[1];
				
				status = atcac_sw_sha2_256(derive_key_mac, sizeof(derive_key_mac), sw_digest);
				RET_ERR(status, "Unable to create digest");			
					
				status=sha204_diverse_key_example(slot, sw_digest, padding);
				
				if (status != ATCA_SUCCESS)
				{
					printf("- Unable to run diverse key: ");
					sha204_parser_rc(status);
					
				}else{		
					key_04_roll_count++;
					printf("- Success. Key 4 has been rolled %d times!\n\r", key_04_roll_count);
				}					
				break;	
				
			case 0xAE:
				status = sha204_key_roll_example_validation(root);	
				
				if (status != ATCA_SUCCESS)
				{
					printf("- Unable to run sha204_validate_key_roll: ");
					sha204_parser_rc(status);
					
				}else{
					printf("- Success...Key 4 was reset and roll count is now 1\n\r");
					key_04_roll_count=1;
				}
			break;
			case 0xAF:
				
				status = ATCA_SUCCESS;
				
				// calculate the parent key 0x02
				status = sha204_create_diverse_key(root, 0x02, parent_key, padding);
				RET_ERR(status, "Unable to calculate key");					
				
				// calculate the known original key 4
				status = sha204_create_diverse_key(root, 0x04, read_data, padding);
				RET_ERR(status, "Unable to calculate key");			
				
				printf("- Encrypt Write\n\r");
				// Initialize the slot with a known key. key 2 is used for encryption
				status = atcab_write_enc(0x04, 0, read_data, parent_key, 0x02);
				
				RET_ERR(status, "Unable to encrypt write");
				
				key_04_roll_count=0;
				printf("- Key 4 was set back to its original key. Roll out is 0.\n\r");
				
				
			break;				
			case 0xB0:
			    memset(&random_data, 0, sizeof(random_data));
			    memset(&read_data, 0, sizeof(read_data));
				status = atcab_random(random_data);
				
				RET_ERR(status, "Unable to get random number");
				
				printf("- Random Generated Number:\n\r");
				print_buffer(random_data, sizeof(random_data));							
	
				status = atcab_nonce_rand(random_data, read_data);
				RET_ERR(status, "Unable to get random number");
				
				printf("- Random Generated Nonce:\n\r");
				print_buffer(read_data, sizeof(read_data));				
			
			break;
			case 0xB1:
				memset(&random_data, 0, sizeof(random_data));
				
				status = atcab_random(random_data);
				RET_ERR(status, "Unable to get random number");				
				printf("- Random Generated Number:\n\r");
				print_buffer(random_data, sizeof(random_data));
				
			break;
			case 0xB2:
					slot = 0x0E;
					// plain text read slot 14
					status = atcab_read_zone(ATCA_ZONE_DATA,slot,0,0,read_data, 32);
					RET_ERR(status, "Unable to read slot");					
					printf("- Slot %d: ", slot);
					print_buffer(read_data, sizeof(read_data));
					
			break;		
			case 0xB3:

					for(block = 0; block < 3; block++){						
						for(offset = 0; offset< 8;offset++){							
							status = atcab_get_addr(ATCA_ZONE_CONFIG, 0, block, offset, &address);
							RET_ERR(status, "Unable to read config");								
							printf("-Zone: %d (ATCA_ZONE_CONFIG) - The address for block %d, offset %d is 0x%02X\n\r", ATCA_ZONE_CONFIG,  block, offset, address);
						}
					}
			break;	
			
			case 0xB4:
				slot = 4;
				status = sha204_calculated_rolled_key(root, slot, key_04_roll_count, read_data);
				RET_ERR(status, "Run sha204_calculated_rolled_key failed");			
				printf("- Success!\n\r");
			break;
			
			case 0xB5:
				
				slot = 7;
				printf("- Slot %d\n\r", slot);
				status = sha204_validate_key_gendig(root, slot, slot,  padding);
				RET_ERR(status, "GenDig/CheckMac Failed");
				printf("- Success. Key checked with GenDig/CheckMac.\n\r");	
				
				slot = 12;
				printf("-Slot %d\n\r", slot);
				status = sha204_validate_key_gendig(root, slot, slot,  padding);
				RET_ERR(status, "GenDig/CheckMac Failed");
				printf("-Success. Key checked with GenDig/CheckMac.\n\r");	
				
				host = 6;
				client = 7;
								
				printf("- Slot %d\n\r", slot);
				status = sha204_validate_key_gendig(root, client, host, padding);
				RET_ERR(status, "GenDig/CheckMac Failed");
				printf("- Success. Key checked with GenDig/CheckMac.\n\r");	
			break;
			case 0xB6:
				status = ATCA_SUCCESS;		
				
				status = sha204_get_slot_config(config_data);
				RET_ERR(status, "Unable to read config");
				
				// get the parent key
				slot = 15;
				idx = slot * 2;
				printf("-Key %d config: 0x%02X %02X\n\r", slot, config_data[idx], config_data[idx+1]);
				parent_key_id =  (config_data[idx+1] & 0x0F);
				printf("Parent Key: %d",parent_key_id);
				// calculate the parent key 0x0D
				status = sha204_create_diverse_key(root, parent_key_id, parent_key, padding);
				RET_ERR(status, "Unable to calculate key");
				
				// calculate the known original key
				status = sha204_create_diverse_key(root, slot, read_data, padding);
				RET_ERR(status, "Unable to calculate key");
				
				printf("- Encrypt Write\n\r");				
				status = atcab_write_enc(slot, 0, read_data, parent_key, 0x0D);
				
				RET_ERR(status, "Unable to encrypt write");							
				printf("- Key %d was written with: ", slot);
				print_buffer(read_data, sizeof(read_data));
			break;
			case 0xB7:
				printf("Get slot config details for a specific slot. Enter 'c' to cancel. \n\r" );
				printf("Enter the slot number: " );						
				scanf("%s",cmd);										
				sscanf(cmd, "%d", &rc);		
						
				if(rc > -1 && rc< 16){
					printf("Slot: %s\n\r", cmd);
					status = sha204_get_slot_config(config_data);	
					RET_ERR(status, "Unable to read config");
					
					idx = rc * 2;
					printf("- Key %d config: 0x%02X %02X\n\r", rc, config_data[idx], config_data[idx+1]);	
										
					write_config_8 = ((config_data[idx] >> 0)  & 0x01);
					write_config_9 = ((config_data[idx] >> 1)  & 0x01);
					write_config_10 = ((config_data[idx] >> 2)  & 0x01);
					write_config_11 = ((config_data[idx] >> 3)  & 0x01);
					
					write_config_12 = ((config_data[idx] >> 4)  & 0x01);
					write_config_13 = ((config_data[idx] >> 5)  & 0x01);
					write_config_14 = ((config_data[idx] >> 6)  & 0x01);
					write_config_15 = ((config_data[idx] >> 7)  & 0x01);
					
					write_config_7 = ((config_data[idx+1] >> 7)  & 0x01);
					write_config_6 = ((config_data[idx+1] >> 6)  & 0x01);
					write_config_5 = ((config_data[idx+1] >> 5)  & 0x01);
					write_config_4 = ((config_data[idx+1] >> 4)  & 0x01);
					
					write_config_3 = ((config_data[idx+1] >> 3)  & 0x01);
					write_config_2 = ((config_data[idx+1] >> 2)  & 0x01);
					write_config_1 = ((config_data[idx+1] >> 1)  & 0x01);
					write_config_0 = ((config_data[idx+1] >> 0)  & 0x01);					
					
					printf("  SlotConfig<%d>.15 = %d\n\r", rc,write_config_15);
					printf("  SlotConfig<%d>.14 = %d\n\r", rc,write_config_14);
					printf("  SlotConfig<%d>.13 = %d\n\r", rc,write_config_13);
					printf("  SlotConfig<%d>.12 = %d\n\r", rc, write_config_12);
					printf("  SlotConfig<%d>.11 = %d\n\r", rc,write_config_11);
					printf("  SlotConfig<%d>.10 = %d\n\r", rc,write_config_10);
					printf("  SlotConfig<%d>.9 = %d\n\r", rc,write_config_9);
					printf("  SlotConfig<%d>.8 = %d\n\r", rc, write_config_8);					
					printf("  SlotConfig<%d>.7 = %d\n\r", rc,write_config_7);
					printf("  SlotConfig<%d>.6 = %d\n\r", rc,write_config_6);
					printf("  SlotConfig<%d>.5 = %d\n\r", rc,write_config_5);
					printf("  SlotConfig<%d>.4 = %d\n\r", rc, write_config_4);
					printf("  SlotConfig<%d>.3 = %d\n\r", rc,write_config_3);
					printf("  SlotConfig<%d>.2 = %d\n\r", rc,write_config_2);
					printf("  SlotConfig<%d>.1 = %d\n\r", rc,write_config_1);
					printf("  SlotConfig<%d>.0 = %d\n\r", rc, write_config_0);
					
					parent_key_id =  (config_data[idx+1] & 0x0F);
					write_key_id = 	(config_data[idx] & 0x0F);	
					printf("- Read Key (parent): %d\n\r", parent_key_id);
					printf("- Write Key: %d\n\r", write_key_id);
					if(write_config_13 == 1 && write_config_15 == 0){
						printf("- DeriveKey command can be run without authorizing MAC (Roll).\n\r");						
					} 
					if(write_config_13 == 1 && write_config_15 == 1){
						printf("- Authorizing MAC required for DeriveKey command (Roll).\n\r");
					}
					if(write_config_12 == 1 && write_config_13 == 1  && write_config_15 == 0){
						printf("- DeriveKey command can be run without authorizing MAC(Create).\n\r");
					}
					
					if(write_config_12 == 1 && write_config_13 == 1  && write_config_15 == 1){
						printf("- Authorizing MAC required for DeriveKey command (Create).\n\r");
					}
					
					if(write_config_13 == 0){
						printf("- Slots with this value in the WriteConfig field may not be used as the	target of the DeriveKey command.\n\r");
					}					
					if(write_config_14 == 1){
						printf("- Encrypt Writes are allowed.\n\r");
					}
					if(write_config_7 == 1){
						printf("- Slot is Secret.\n\r");
					}
					if(write_config_7 == 0){
						printf("- The slot is not secret and allows clear read, clear write, no MAC check and no Derivekey Command..\n\r");
					}
					if(write_config_6 == 1 && write_config_7 == 1){
						printf("- Encrypted read access allowed.\n\r");
					}
					if(write_config_5 == 1 ){
						printf("- Limit on the number of time the key can be used based on the UseFlag (or	LastKeyUse) for the slot..\n\r");
					}
					if(write_config_4 == 1 ){
						printf("- This slot can only be used for CheckMac and GenDig followed by CheckMac Commands.\n\r");
					}
					if(write_config_4 == 0 ){
						printf("- This slot can be used for all crypto commands.\n\r");
					}
				} else {					
					printf("Invalid number. Valid numbers are from 0-15\n\r");
				}
				//printf("Getting details for slot %c\n\r", input);
			break;
			default:
				printf("** Invalid Command ** \n\r");
			break;
	}			
}

 void sha204_print_menu(void){
	 printf("%s", megenta);
	 printf("\n\r");
	 printf("       _______ _____ _    _          ___   ___  _  _\n\r");
	 printf("    /\\|__   __/ ____| |  | |   /\\   |__ \\ / _ \\| || |\n\r");
	 printf("   /  \\  | | | (___ | |__| |  /  \\     ) | | | | || |_ __ _\n\r");
	 printf("  / /\\ \\ | |  \\___ \\|  __  | / /\\ \\   / /| | | |__   _/ _` |\n\r");
	 printf(" / ____ \\| |  ____) | |  | |/ ____\\ \\/ /_| |_| |  | || (_| |\n\r");
	 printf("/_/    \\_\\_| |_____/|_|  |_/_/    \\_\\____|\\___/   |_| \\__,_|\n\r");
	 printf("\n\r	");
	 printf("%s", reset);
	 printf("\n\r************************************************\n\r");
	 printf("Step 1 - Run Personalization (Configure Device) - Command (03)\n\r");
	 printf("Step 2 - Lock The Configuration Zone - Command (04)\n\r");
	 printf("Step 3 - Write to the OTP zone - Command (05)\n\r");
	 printf("Step 4 - Write the keys (slots) - Command (06)\n\r");		 
	 printf("Step 5 - Lock OTP and Data Zones - Command (07)\n\r");
	 printf("Step 6 - Read OTP Zones - Command (08)\n\r");
	 printf("Step 7 - Run DeriveKey for Slot 5 - Command (AB)\n\r");
	 printf("Step 8 - Run DeriveKey (Key Roll) for Slot 4 - Command (AE)\n\r");
	 printf("Step 9 - Validate Keys - Command (AA)\n\r");
	 printf("\n\r************************************************\n\r");
	 printf("\n\r************************************************\n\r");
	 printf("*******       ATSHA204a Advanced Demo      *****\n\r");
	 printf("************************************************\n\r");
	 printf("(00) Print this menu\n\r");
	 printf("(01) Print Device Details\n\r");
	 printf("(02) Read Configuration\n\r");
	 printf("(03) Personalize Device (e.g. Configure) \n\r");
	 printf("(04) Lock Configuration Zone (LockConfig) \n\r");
	 printf("(05) Write to OTP - LockConfig must be locked \n\r");
	 printf("(06) Write Keys - LockConfig must be locked\n\r");
	 printf("(07) Lock OTP and Data Zones (LockValue)\n\r");	
	 printf("(08) Read OTP Zone - Config and OTP Zone Must be locked \n\r");
	 printf("(09) Create 16 diversified keys (Software) \n\r");
	 printf("(A0) HMAC challenge with all modes (Slot 12 & 4) \n\r");
	 printf("(A1) Validate Diverse Key - DeriveKey/GenDig/CheckMac  \n\r");
	 printf("(A2) Read Encrypted Slot 8 thru 13  \n\r");
	 printf("(A3) Create diversified key slot 07 (software) \n\r");
	 printf("(A4) Calculate Diverse key for slot 00 and challenge  \n\r");
	 printf("(A5) Host/Client MAC Challenge w/Diverse key for slot 01 - Full SN & 88 Bits of OTP  \n\r");
	 printf("(A6) Host/Client CheckMac Example  \n\r");
	 printf("(A7) Derive Key Examples - Runs CheckMac and UpdateCount After   \n\r");
	 printf("(A8) Random Challenge with Slot 9 with Full SN\n\r");
	 printf("(A9) Random Challenge with Slot 8 And TempSlot, Full SN and OTP\n\r");
	 printf("(AA) Validate Keys with CheckMac \n\r");
	 printf("(AB) DeriveKey w/MAC Example with slot 5. Validation with Gendig/CheckMac \n\r");
	 printf("(AC) UpdateCount of Derived Keys - Slots 4, 5 & 7 \n\r");
	 printf("(AD) Roll Key on Slot 4 (DeriveKey) \n\r");
	 printf("(AE) Validate Key 4 (Roll Key w/CheckMac) \n\r");
	 printf("(AF) Reset Key 4 \n\r");
	 printf("(B0) Nonce - Random mode (hardware) Generate a random nonce combining a host nonce (num_in) and random number. \n\r");
	 printf("(B1) Random Number - Generate a random number (hardware). \n\r");
	 printf("(B2) Read unprotected slot 14 \n\r");
	 printf("(B3) Print Config Zone Addresses \n\r");
	 printf("(B4) Rolling Key Validation - Run Command (AF) then Command (AD) many times first before running \n\r");
	 printf("(B5) Validation Slot 12 & Slot 7 Gendig/CheckMac \n\r");
	 printf("(B6) Encrypted Write Slot 15 \n\r");
	 printf("(B7) Get slot config details for a specific slot \n\r");
	 
	 
	 printf("$");
	 
 }