#include <asf.h>
#include "conf_usart.h"
#include "conf_at25dfx.h"

#define ADDRESS 0x10000

typedef struct  {	
	uint8_t id;
	char message[120];
}
Packet;

Packet write_pck;
Packet read_pck;

uint8_t write_buffer[sizeof(write_pck)];
uint8_t read_buffer[sizeof(read_pck)];

int main (void)
{
	system_init();
	delay_init();
	configure_console(115200);	
	at25dfx_init();
	delay_init();

	printf("SAMD21 Serial Flash Tutorial AT25DFx\n");
	
	at25dfx_chip_wake(&at25dfx_chip);	
	if (at25dfx_chip_check_presence(&at25dfx_chip) != STATUS_OK) {
		// Handle missing or non-responsive device
		printf("Not chip found!\n");
	}	
	
	// write
	const char *message = "Hello World";
	write_pck.id = 125;

	strncpy(write_pck.message, message, sizeof(write_pck.message));	
	memcpy(write_buffer, &write_pck, sizeof(write_pck));// copy struct to uint8_t array	
	at25dfx_chip_set_sector_protect(&at25dfx_chip, ADDRESS, false);	
	at25dfx_chip_erase_block(&at25dfx_chip, ADDRESS, AT25DFX_BLOCK_SIZE_4KB);	
	at25dfx_chip_write_buffer(&at25dfx_chip, ADDRESS, write_buffer, sizeof(write_pck));	
	at25dfx_chip_set_global_sector_protect(&at25dfx_chip, true);
	
	// read
	at25dfx_chip_read_buffer(&at25dfx_chip, ADDRESS, read_buffer, sizeof(read_pck));
	memcpy(&read_pck,read_buffer,sizeof(read_pck)); // copy data to our struct
	
	at25dfx_chip_sleep(&at25dfx_chip);	
	printf("Reading Packet from AT25DFx\n\r");
	printf("Message: %s with id of %d\n", read_pck.message, read_pck.id);

	while (1) {
		
	}
}
