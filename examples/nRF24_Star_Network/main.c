/** -----------------------------------------------------------------------------
* Multi-Radio with ACK payload *

RF24		SAMD21 xplained Pro
CE			PB11
CSN			PB17
SCK			PB23
MOSI		PB22
MISO		PB16

* Requirements
* Valid Nodes:
*	Node 0 - Always the base node
*	Node 1 - Not used reserved for Pipe 0 ACK
*	Node 2-49 - Can receive from child nodes and transmit to its parent
*	Nodes 50 - 251 can only transmit 	

Requires the following ASF modules:

	* Delay Routine
	* Generic Board Support
	* PORT - GPIO Pin Control
	* SERCOM SPI - Serial Peripheral Interface (Callback APIs)
	* SERCOM USART - Serial Communications (Callback APIs)
	* Standard Serial I/O (stdio)
	* System - Core System Driver
*/

#include "main.h"
uint16_t fakeval= 0;
uint16_t ackval= 0;

enum Node nd = PRX; 
enum Mode c_mode = RECEIVE;
uint8_t node_number = 2;

int main (void)
{
	system_init();
	configure_console(115200);
	delay_init();
	nrf24_configure_spi_master();
	nrf24_configure_spi_master_callbacks();

	printf("RF24 SAMD21 Star Network Project\n\r");	
	if(node_number == 1){			
		printf("Invalid Node Id.\n\r Nodes must be from 0, 2-251");
		exit(0);
	} 

	nrf24_init(nd);
	//nrf24_printNodeName(nd);

	nrf24_configure_star_network(71);
	nrf24_configure_star_node(node_number, c_mode);

	delay_s(2); // let the RF settle	
	
	nrf24_flushTx();
	nrf24_flushRx();
	nrf24_printDetails();	
	nrf24_powerUpRx();	

	while (1) {	
	 
	    if(node_number == 0){ // base always receives
			receive_data();	
		}else if(node_number < 12){
			relay_data();				
		} else {			
			send_data();	
			printf("Packet Loss: %d\n\r", nrf24_getPacketLossCount());
			delay_s(5);		
		}		
	}
}

void relay_data(){
	
	if(nrf24_dataReady())
	{		
		
		uint8_t payLoadsize = nrf24_getPayloadLength();
		
		if(payLoadsize > 32){
			printf("Invalid payload length received: %d\n\r", payLoadsize); // should implement a WDT to reset if this happens
			return;
		}
		printf("Payload Size: %d\n\r", payLoadsize);
		
		uint8_t pipeNo = nrf24_getPayLoad(&pck, payLoadsize);
		
		printf("Packet ID: %d\n\r", pck.id);
		printf("Packet Val: %d\n\r", pck.data);
		printf("\n\r<<< Data Complete From Radio # %d >>>\n\r",  pck.id);
		printf("\n\r<<< Resending to Base  >>>\n\r");
		
		c_mode = TRANSMIT;		
		nrf24_configure_star_node(node_number, c_mode);
		//nrf24_printDetails();
		uint8_t ack_data_status; 
		nrf24_sendPayLoad(&pck, sizeof(pck), &ack_data_status);	
		printf("Packet Loss: %d\n\r", nrf24_getPacketLossCount());
		
		c_mode = RECEIVE;
		nrf24_configure_star_node(node_number, c_mode);

	}


}

void send_data(){

	uint8_t ack_data_status; 

	if(fakeval > 120){
		fakeval = 0; // reset
	}	
	pck.id = node_number; // testing 
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







