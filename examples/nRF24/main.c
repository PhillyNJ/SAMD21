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

enum Node nd = PTX1; 

int main (void)
{
	system_init();
	configure_console(9600);
	delay_init();
	nrf24_configure_spi_master();
	nrf24_configure_spi_master_callbacks();

	printf("RF24 SAMD21 Project\n");	
		
	nrf24_init(nd);
	// configure the call back for ack packload
	nrf24_setAckPlayLoadCallback(getAckPayLoad_cb);
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
			delay_s(5);		
		 }		
	}
}

void send_data(){

	uint8_t ack_data_status; 

	if(fakeval > 120){
		fakeval = 0; // reset
	}	
	packet.mode = auth; // testing 
	for(uint8_t i = 0; i > sizeof(packet.sn); i++){
		packet.sn[i] = i;		
	}	
	// fill with random data
	for(int i = 0; i<20; i++){		
		packet.data[i] = rand();
	}
	printf("Sending Payload size: %d\n\rData:\n\r", sizeof(packet));	
	for(int i = 0; i<20; i++){
		printf("0x%x ", packet.data[i]);
	}
	
	
	nrf24_sendPayLoad(&packet, sizeof(packet), &ack_data_status);	
	
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
	
		uint8_t pipeNo = nrf24_getPayLoad(&packet, payLoadsize);				
		printf("Packet SN:\n\r");
		for(uint8_t i = 0; i > sizeof(packet.sn); i++){
			printf("0x%x ", packet.sn[i]);
		}		
		printf("\n\rData: \n\r");
		for(int i = 0; i<20; i++){			
			printf("0x%x ", packet.data[i]);
		}
		

		printf("\n\r<<< Data Complete From Radio Pipe %d>>>\n\r", pipeNo);	
		
	}	
}

uint8_t	getAckPayLoad_cb(void * buf, uint8_t len){
	
	printf("\n\r** Ack Payload From Call Back** \n\r");
	
	uint8_t pipeNo =  ((nrf24_getStatus() >> RX_P_NO) & 0x07);
	
	for(int i = 0; i<len;i++){
		read_buffer[i] = 0;
	}
	uint8_t *dt = buf;
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(R_RX_PAYLOAD);
	data_buffer[0]= R_RX_PAYLOAD;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,len+1);
	nrf24_transfer_wait();
	spi_select_slave(&spi_master_instance, &slave, false);

	for(int i = 0; i<len;i++){
		dt[i] = read_buffer[i];
	}
	nrf24_configRegister(RF_STATUS,(1<<RX_DR)); // Reset status register
	nrf24_flushRx();
	return pipeNo;
	
}