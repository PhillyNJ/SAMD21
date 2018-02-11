/** -----------------------------------------------------------------------------
* Multi-Radio with ACK payload *

RF24		SAMD21 xplained Pro
CE			PB11
CSN			PB17
SCK			PB23
MOSI		PB22
MISO		PB16

Set node to 

	PRX	 - receiver
	PTX1 - transmitter
	PTX2 - transmitter
	PTX3 - transmitter 
	PTX4 - transmitter
	PTX5 - transmitter

Requires the following ASF modules:

	* Delay Routine
	* Generic Board Support
	* PORT - GPIO Pin Control
	* SERCOM SPI - Serial Peripheral Interface (Callback APIs)
	* SERVOM USART - Serial Communications (Callback APIs)
	* Standard Serial I/O (stdio)
	* System - Core System Driver
*/

#include "main.h"
uint16_t fakeval= 0;
uint16_t ackval= 0;

enum Node nd = PTX2; 

int main (void)
{
	system_init();
	configure_console(115200);
	delay_init();
	nrf24_configure_spi_master();
	nrf24_configure_spi_master_callbacks();

	printf("RF24 SAMD21 Project\n");	
		
	nrf24_init(nd);
	nrf24_printNodeName(nd);
	nrf24_config(71,sizeof(data_array));	// try 25 - was 75
	delay_s(2); // let the RF settle
	
	nrf24_printDetails();	
	nrf24_flushTx();
	nrf24_powerUpRx();	

	while (1) {	
	     if(nd == PRX){
			receive_data();					
		 } else {			
			send_data();	
			printf("Packet Loss: %d\n\r", nrf24_getPacketLossCount());
			delay_s(3);		
		 }		
	}
}

void send_data(){

	uint8_t ack_data_status; 

	if(fakeval > 120){
		fakeval = 0; // reset
	}	
	pck.id = 100; // testing 
	pck.data = fakeval++;

	printf("Sending Payload size: %d\n\r", sizeof(pck));	
	
	nrf24_sendPayLoad(&pck, sizeof(pck), &ack_data_status);	
	
	if(ack_data_status == 1){
		
		for(uint8_t i = 0; i < BUF_LENGTH ;i++){
			printf("%d ", ack_payload[i]);
		}
		printf("\n\r");
	}
}

void receive_data(){
	
	if(ackval > 120){
		ackval = 0; // reset
	}

	if(nrf24_dataReady()) 
	{	
		nrf24_flushTx();
		// preload ack data			
		for(uint8_t z = 0; z < BUF_LENGTH; z++){
			ack_payload[z] = ackval++; // fill with dummy data
		}
		nrf24_setAckPayLoad(&ack_payload, PTX1, BUF_LENGTH);
		// get the payload size
		uint8_t payLoadsize = nrf24_getPayloadLength();
		
		if(payLoadsize > 32){
			printf("Invalid payload length received: %d\n\r", payLoadsize); // should implement a WDT to reset if this happens			
			return;
		}
		printf("Payload Size: %d\n\r", payLoadsize);
	
		uint8_t pipeNo = nrf24_getPayLoad(&pck, payLoadsize);				
	
		printf("Packet ID: %d\n\r", pck.id);
		printf("Packet Val: %d\n\r", pck.data);

		printf("\n\r<<< Data Complete From Radio Pipe %d>>>\n\r", pipeNo);	
		
	}	
}







