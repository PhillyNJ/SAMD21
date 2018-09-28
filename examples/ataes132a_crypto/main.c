#include "main.h"

volatile uint32_t g_ul_ms_ticks = 0;
volatile bool g_b_systick_event = false;

uint8_t crunch1[16] = {0xCC,0x46,0xC3,0x26,0x4F,0x69,0x11,0x82,0x64,0x87,0x8B,0x50,0xA8,0xC9,0x58,0x8B};
uint8_t crunch2[16] = {0xAA,0x4C,0xD4,0x0B,0xF7,0x41,0xF6,0x52,0xFF,0x7B,0x19,0x01,0x6C,0xC2,0x77,0xA0};
#define BLINK_PERIOD 1000
int main (void)
{
	system_init();
	delay_init();
	SysTick_Config(system_gclk_chan_get_hz(GCLK_GENERATOR_0)/10000);	
	configure_console(9600);
	printf("SAMD21-ATAES132a Tutorial 1\n\r");
	aes132p_enable_interface();
	print_commands();
	aes132_read_device_details(&mydetails);

	while (1) {
		if (scanf("%c",&serial_data)>0){
			if (parse_command(serial_data)){
				aes132app_command_dispatch();
			}	
		};	
	}
}

void aes132app_command_dispatch(void)
{
	uint8_t random[16];
	uint8_t sn[8];
	uint8_t sz[4];
	uint8_t crunch[16];
	uint8_t auth_status[2] = {'\0'};	
	uint16_t usage; 
	uint32_t start = 0;
	switch (g_packet_block[0]) {	
		case 0x00:
			print_commands();
			break;
		case 0x01:
			aes132_read_device_details(&mydetails);		
			break;		
		case 0x02:
			aes132_print_zone_addresses();
			break;
		case 0x03:	
			aes132_personalize();
			break;
		case 0x04:
			aes132_read_configuration();
			break;
		case 0x05:
			printf("** Test Auth (key 13) ** \n\r");
			usage =  (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK | AES132_AUTH_USAGE_KEY_USE);
			aes132_nonce(); // create nonce first
			aes132_inbound_auth_key(13, key13, usage);
		break;	
		case 0x06:
			printf("** Inbound Auth (key 4) ** \n\r");
			usage =  (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK);
			aes132_nonce(); // create nonce first
			aes132_inbound_auth_key(4, key04, usage);
		break;		
		case 0x07:
			printf("Outbound Auth (Key 4) \n\r");
			aes132_nonce(); // create nonce first
			aes132_outbound_auth(0x04, key04, 0xE2, 0x00);
		break;	
		case 0x08:
			aes132_read_info(AES132_INFO_AUTH_STATUS, auth_status); // auth status
			aes_print_buffer(auth_status, 2);
			auth_status[1] != 0xFF ? printf("Auth Key: %d\n\r", auth_status[1]) : printf("No Auth Key Enabled\n\r");
		break;
		case 0x09:
			printf("Creates a new for KeyMemory 6 - pre-auth with Key 13 (05)\n\r");
			uint8_t myNewKey[16] = {'\0'};
			int ret = aes132_key_create(myNewKey);
			if(ret == AES132_FUNCTION_RETCODE_SUCCESS){
				printf("Key06 now has this key: ");
				aes_print_buffer(myNewKey, sizeof myNewKey);
			}
		break;
		case 0x0A:
			printf("Mutual Auth - Key 04\n\r");
			aes132_nonce();
			aes132_mutual_auth(0x0004, key04, 0x03, (AES132_AUTH_USAGE_READ_OK | AES132_AUTH_USAGE_WRITE_OK));
			break;
		case 0x0B:
			printf("Resetting Auth Register\n\r");
			aes132_reset_auth();
			break;	
		case 0x0C:
			printf("Encryption with key 06 - Requires pre-auth with Key 13 - (05)\n\r");
			aes132_encrypt_decrypt_key(0x006, key06);
			break;
		case 0x0D:
			printf("Hardware Encryption and Decryption = - Encrypt Key 1 - Decrypt Key 2\n\r");
			aes132_encrypt_decrypt();
			break;
		case 0x0E:
			printf("Software Encryption & Encrypt Write to AES132 Memory - Uses Key 01\n\r");
			aes132_encrypt_encwrite();
		break;
		case 0x0F:
			printf("Encrypt Read from AES132 Memory & Software Decrypt - Uses Key 00\n\r");
			aes132_encread_decrypt();
			break;
        case 0x10:
			printf("Write to user zone 0, 1 & 2. For zones 2 & 3, pre-auth required - run command (06) first \n\r");
			// Check the auth register
			aes132_read_info(AES132_INFO_AUTH_STATUS, auth_status); // auth status
			auth_status[1] != 0xFF ? printf("Auth Key: %d\n\r", auth_status[1]) : printf("No Auth Key Enabled\n\r");

			if(auth_status[1] != 4){
				printf("** Pre Auth Required - Run command (06) First** \n\r");
			} else {
				aes132_write_user_zone();
			}
			break;
		case 0x11:
			printf("Read from User Zone 0, 1 & 2 - Pre-Auth Required to read from zone 2 - Run Command (92)\n\r");
			aes132_read_user_zone();
		break;
		case 0x12:
			printf("Reloading Key 6 - Requires pre-auth with Key 13 - (05)\n\r");
			aes132_keyload_to_key_memory(0x0006, key05, key06);
		break;
		case 0x13:
			printf("Load VolatileKey \n\r");
			aes132_volatile_keyload();
			break;
		case 0x14:
			aes132_volatile_key_encrypt_decrypt();
			break;
		case 0x15:
			aes132_nonce();
			break;
		case 0x16:			
			aes132_create_random_number(random);
			aes_print_buffer(random, sizeof random);
			break;
		case 0x17:
			aes132_write_user_zone_sn();
		break;	
		case 0x18:
			aes132_encread_decrypt_sn();
		break;
		case 0x19:			
			aes132_read_serial(sn);
			aes_print_buffer(sn, sizeof sn);
			break;		
		case 0x20:
			aes132_read_small_zone(sz, 4);	
			aes_print_buffer(sz, sizeof sz);
		break;
		case 0x21:
			aes132_write_user_zone_04();
		break;
		case 0x22:
			aes132_decrypt_read();
		break;
		case 0x23:
			aes132_read_info(AES132_INFO_MACCOUNT, auth_status); 
			aes_print_buffer(auth_status, 2);
		break;
		case 0x24:			
		
			start = g_ul_ms_ticks;
			printf("Start Time: %ld\n\r", start);
			aes132_crunch(0x00FF, crunch);
			aes_print_buffer(crunch, 16);
			uint32_t finish = g_ul_ms_ticks;
			printf("Finish Time: %ld\n\r", finish);
			memcmp(&crunch, &crunch2, 16) == 0 ? printf("Valid ATAES132a\n\r"): printf("Failed Crunch Test\n\r");
			printf("Time: %ld\n\r", finish-start);

		break;
		default:
		//aes132app_single_execute();
		//printf("aes132app_single_execute\n\r");

		break;
	}
}

void print_commands(void){
	printf("%s", megenta);
	printf("\n\r");
	printf("        _     ___   ___   _   ____  ___        \n\r");
	printf("       /_\\   | __| / __| / | |__ / |_  )  __ _ \n\r");
	printf("      / _ \\  | _|  \\__ \\ | |  |_ \\  / /  / _` |\n\r");
	printf("     /_/ \\_\\ |___| |___/ |_| |___/ /___| \\__,_|\n\r");
	printf("                                               \n\r");
	printf("%s", reset);
	printf("\n\r************************************************\n\r");
	printf("*****         ATAES132a Main Menu          *****\n\r");
	printf("************************************************\n\r");
	printf("(00) - Show This Menu\n\r");
	printf("(01) - Read Device Details\n\r");
	printf("(02) - Print Zone Addresses\n\r");
	printf("(03) - Personalize Device\n\r");
	printf("(04) - Read Configuration\n\r");
	printf("(05) - Inbound Auth for Key 13\n\r");
	printf("(06) - Inbound Auth for Key 4\n\r");
	printf("(07) - Outbound Auth for Key 4\n\r");
	printf("(08) - Read Auth Register\n\r");
	printf("(09) - Create Key - KeyMemory 6 \n\r");
	printf("(0A) - Mutual Auth for Key 4\n\r");
	printf("(0B) - Reset Auth Register\n\r");
	printf("(0C) - Hardware Encryption/Decryption with key 06 - Requires pre-auth with Key 13 - (05)\n\r");
	printf("(0D) - Hardware Encryption and Decryption = - Encrypt Key 1 - Decrypt Key 2\n\r");
	printf("(0E) - Software Encryption & Encrypt Write (EncWrite) to AES132a Memory - Uses Key 01\n\r");
	printf("(0F) - Encrypt Read (EncRead) from AES132a Memory & Software Decrypt - Uses Key 00\n\r");
	printf("(10) - Write UserZone - Write to user zone 0, 1 & 2. For zones 2 & 3, pre-auth required - run command (06) first\n\r");
	printf("(11) - Read UserZone - from User Zone 0, 1 & 2 - Pre-Auth Required to read from zone 2 - Run Command (06)\n\r");
	printf("(12) - Keyload (KeyMemory) - Loads Key 6 - Requires pre-auth with Key 13 - (05)\n\r");
	printf("(13) - Keyload (VolatileKey) \n\r");
	printf("(14) - Test VolatileKey Encryption and Decryption \n\r");
	printf("(15) - Create Nonce\n\r");
	printf("(16) - Create Random Number\n\r");
	printf("(17) - EncWrite to User Zone 3 - Use Serial Number & Pre-Auth\n\r");
	printf("(18) - EncRead to User Zone 3 - Use Serial Number & Pre-Auth\n\r");	
	printf("(19) - Read Serial Number\n\r");
	printf("(20) - Read Small Zone [0:3]\n\r");
	printf("(23) - Read MAC Count \n\r");
	printf("(24) - Crunch \n\r");
	printf("\n\r\n\r");
	

}

void SysTick_Handler (void)
{
	g_ul_ms_ticks++;
	g_b_systick_event = true;
	
}