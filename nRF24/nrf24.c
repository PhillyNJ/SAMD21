
#include "nrf24.h"
#include <asf.h>
volatile bool transrev_complete_spi_master = false;
const uint8_t PRX_ADDRESS[6] = { RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5 };
enum Node node = PRX; // default
enum Mode current_mode = RECEIVE; // default for start network
uint8_t ptx0[5] =	{0x78, 0x78, 0x78, 0x78, 0x78};
uint8_t ptx1[5] =	{0xF1, 0xB4, 0xB5, 0xB6, 0xB3};
uint8_t ptx2[5] =	{0xCD, 0xB4, 0xB5, 0xB6, 0xB3};
uint8_t ptx3[5] =	{0xA3, 0xB4, 0xB5, 0xB6, 0xB3};
uint8_t ptx4[5] =	{0x0F, 0xB4, 0xB5, 0xB6, 0xB3};
uint8_t ptx5[5] =	{0x05, 0xB4, 0xB5, 0xB6, 0xB3};

void nrf24_init(enum Node nd) 
{
	node= nd;
    REG_PORT_DIRSET1 = CE; 	
	nrf24_ce_state(LOW);   
}

void nrf24_ce_state(uint8_t state)
{
	if(state)
	{			
		REG_PORT_OUT1 |= CE; 
	}
	else
	{	
		REG_PORT_OUT1 &= ~(CE); 
	}
}

void nrf24_transfer(uint8_t data){		
  
	data_buffer[0]=data;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
	nrf24_transfer_wait();	
}

void nr424_transmit_data(uint8_t * data, uint8_t len)
 {					
		for(int x = 0; x < len; x++){						
			data_buffer[x] = data[x];			
		}
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,len);
		nrf24_transfer_wait();		
}

void nrf24_transfer_wait(){

	while (!transrev_complete_spi_master) {
		/////* Wait for write and read complete */
	}
	transrev_complete_spi_master = false;

}

void nrf24_configRegister(uint8_t reg, uint8_t value)
{
	spi_select_slave(&spi_master_instance, &slave, true);
	data_buffer[0]= W_REGISTER | (REGISTER_MASK & reg);
	data_buffer[1]=value;	
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,2);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false); 

}

uint8_t nrf24_readRegister(uint8_t reg, uint8_t len)
{	
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(R_REGISTER | (REGISTER_MASK & reg));
	data_buffer[0]= NOP;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,len);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false);	
	return read_buffer[1];
}	

uint8_t nrf24_getPayloadLength(){
	
	spi_select_slave(&spi_master_instance, &slave, true);	
	data_buffer[0]= R_RX_PL_WID;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,2);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false);

	return read_buffer[1];
}

void nrf24_write_register(uint8_t reg, uint8_t * value, uint8_t len)
{
	spi_select_slave(&spi_master_instance, &slave, true);	
	nrf24_transfer(W_REGISTER | (REGISTER_MASK & reg));	
	nr424_transmit_data(value,len);	
	spi_select_slave(&spi_master_instance, &slave, false); 
}

void nrf24_powerDown()
{
	nrf24_ce_state(LOW);
	nrf24_configRegister(RF_CONFIG,(1<<PRIM_RX)); // need to test this
	delay_ms(2); // let settle at least 1.5 ms
}
void nrf24_powerUpTx()
{
	nrf24_configRegister(RF_CONFIG, RF24_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) );	
}

void nrf24_powerUpRx()
{
	nrf24_flushRx(); // clean out
	nrf24_ce_state(LOW);
	// reset the status register so we can start fresh
	nrf24_configRegister(RF_CONFIG, RF24_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );
	nrf24_ce_state(HIGH);	
	nrf24_configRegister(RF_STATUS,(1 << TX_DS) | (1 << MAX_RT));

}

void nrf24_flushTx(){
	
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(FLUSH_TX);
	spi_select_slave(&spi_master_instance, &slave, false);
}

void nrf24_flushRx(){
	
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(FLUSH_RX);
	spi_select_slave(&spi_master_instance, &slave, false);
}

uint8_t nrf24_txIsFull(){
	
	uint8_t status = nrf24_getStatus();

	/* Transmission went OK */
	if((status & ((1 << TX_FULL))))
	{
		return 0;
	}	
	return 1;	
}

uint8_t nrf24_getStatus()
{	
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(R_REGISTER | (REGISTER_MASK & RF_STATUS));
	data_buffer[0]= NOP;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false);	
	return read_buffer[0];
}

static void callback_spi_master( struct spi_module *const module)
{
	transrev_complete_spi_master = true;
}

void nrf24_setDataPipe(uint8_t * addr){
	
	nrf24_configure_datapipe(RX_ADDR_P0, addr,NRF24_ADDR_LEN);
	delay_us(30);
	nrf24_configure_datapipe(TX_ADDR, addr,NRF24_ADDR_LEN);
		
}

void nrf24_configure_datapipe(uint8_t reg, uint8_t * value, uint8_t len){

	data_buffer[0]=  W_REGISTER | (REGISTER_MASK & reg);

	spi_select_slave(&spi_master_instance, &slave, true);
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
	nrf24_transfer_wait();	

	for(int x = 0; x < NRF24_ADDR_LEN; x++){
		data_buffer[x] = value[x];
	}
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,5);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false);

}

void nrf24_configure_spi_master_callbacks()
{
	spi_register_callback(&spi_master_instance, callback_spi_master, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}

void nrf24_config(uint8_t channel, uint8_t pay_length)
{	   
   nrf24_flushTx(); // clear out the TX Buffer
   freq_channel = channel;
	
   
   // CRC enable, 1 byte CRC length
   nrf24_configRegister(RF_CONFIG, CONFIG_MASK); // 0x0B 0000‭ 1011‬
   
   // Auto Acknowledgment - all pipes
   nrf24_configRegister(EN_AA, 0x3F);
  
   nrf24_configRegister(FEATURE, 0x06); // Enable dynamic payload    
   
   // Enable RX addresses
   nrf24_configRegister(EN_RXADDR,0x3F); // all pipes

   // 3 byte addresses
   nrf24_configRegister(SETUP_AW, 0x03);
   
   // 5 retries
   nrf24_configRegister(SETUP_RETR, 0x25);// todo make retries global? 5 retries, 750us wait
   
   // Set RF channel
   nrf24_configRegister(RF_CH,channel);
   
   //set speed
   nrf24_configRegister(RF_SETUP, 0x0E); //0dBm 2Mbps
   
   nrf24_configRegister(DYNPD,0x3F); 	// Dynamic packet length on all pipes
   

   // not needed for dynamic payload //
   /* 
   payload_len = pay_length;
   nrf24_configRegister(RX_PW_P0, payload_len);
   nrf24_configRegister(RX_PW_P1, payload_len);
   nrf24_configRegister(RX_PW_P2, payload_len);
   nrf24_configRegister(RX_PW_P3, payload_len);
   nrf24_configRegister(RX_PW_P4, payload_len);
   nrf24_configRegister(RX_PW_P5, payload_len);   
   */
   
   switch(node){
	   
	   case PRX:
	   nrf24_setRxDataPipe(ptx0, 0);
	   nrf24_setRxDataPipe(ptx1, 1);
	   nrf24_setRxDataPipe(ptx2, 2);
	   nrf24_setRxDataPipe(ptx3, 3);
	   nrf24_setRxDataPipe(ptx4, 4);
	   nrf24_setRxDataPipe(ptx5, 5);
	   break;
	   case PTX1:
	   nrf24_setDataPipe(ptx1);
	   break;
	   case PTX2:
	   nrf24_setDataPipe(ptx2);
	   break;
	   case PTX3:
	   nrf24_setDataPipe(ptx3);
	   break;
	   case PTX4:
	   nrf24_setDataPipe(ptx4);
	   break;
	   case PTX5:
	   nrf24_setDataPipe(ptx5);
	   break;	  
	   
   }

   // Start listening
    nrf24_powerUpRx();
	nrf24_ce_state(HIGH);	
}

void nrf24_configure_star_network(uint8_t channel){

   nrf24_flushTx(); // clear out the TX Buffer
   freq_channel = channel;	
   
   // CRC enable, 1 byte CRC length
   nrf24_configRegister(RF_CONFIG, CONFIG_MASK); // 0x0B 0000‭ 1011‬
   
   // Auto Acknowledgment - all pipes
   nrf24_configRegister(EN_AA, 0x3F);
  
   nrf24_configRegister(FEATURE, 0x06); // Enable dynamic payload    
   
   // Enable RX addresses
   nrf24_configRegister(EN_RXADDR,0x3F); // all pipes

   // 3 byte addresses
   nrf24_configRegister(SETUP_AW, 0x03);
   
   // 5 retries
   nrf24_configRegister(SETUP_RETR, 0x25);// todo make retries global? 5 retries, 750us wait
   
   // Set RF channel
   nrf24_configRegister(RF_CH,channel);
   
   //set speed
   nrf24_configRegister(RF_SETUP, 0x0E); //0dBm 2Mbps
   
   nrf24_configRegister(DYNPD,0x3F); 	// Dynamic packet length on all pipes
   
   // Start listening
   nrf24_powerUpRx();
   nrf24_ce_state(HIGH);	

   // set up lookup table
   uint8_t nc = 1;
   for(uint8_t i = 0; i < 50; i++){

		for(uint8_t j = 0; j < 5; j++){		   
			lookup_table[i][j] = nc++;
			//printf("lookup_table[%d][%d] = %d\n\r", i,j, nc);
		}
   }
}

void nrf24_configure_star_node(uint8_t node_id, enum Mode md){

	current_mode = md;

	uint8_t pipeLength = 5;
	uint8_t i;	
	uint8_t offset = 2;
	
	if(md == RECEIVE){
	
		for(i = 0; i < pipeLength; i++){
			ptx0[i] = node_id + 1;
		}
		
		ptx1[0] = (node_id * 5) + offset;
		offset++;
		ptx2[0] = (node_id * 5) + offset;
		offset++;
		ptx3[0] = (node_id * 5) + offset;
		offset++;
		ptx4[0] = (node_id * 5) + offset;
		offset++;
		ptx5[0] = (node_id * 5) + offset;
		uint8_t val = node_id + 1;
		for(i = 1; i < pipeLength; i++){

			ptx1[i] = val;
			ptx2[i] = val;
			ptx3[i] = val;
			ptx4[i] = val;
			ptx5[i] = val;
		}

		nrf24_setRxDataPipe(ptx0, 0);
		nrf24_setRxDataPipe(ptx1, 1);
		nrf24_setRxDataPipe(ptx2, 2);
		nrf24_setRxDataPipe(ptx3, 3);
		nrf24_setRxDataPipe(ptx4, 4);
		nrf24_setRxDataPipe(ptx5, 5);
	} else {

		
		// find node pipe & position
		uint8_t position = 0;
		uint8_t master = 0;

		for(uint8_t x = 0; x < 50; x++){
			for(uint8_t j = 0; j < 5; j++){
				if(lookup_table[x][j] == node_id){
					position = j ;
					master = x + 1;
				};
			}
		}


		printf("Node %d reports to Node %d on Pipe %d\n\r", node_id, master, position);
		// fill the pipe arrays
		ptx1[0] = node_id;
		ptx2[0] = node_id;
		ptx3[0] = node_id;
		ptx4[0] = node_id;
		ptx5[0] = node_id;
		
		for(uint8_t z = 1; z< 5;z++){
				
				ptx1[z] = master;
				ptx2[z] = master;
				ptx3[z] = master;
				ptx4[z] = master;
				ptx5[z] = master;			
			
		}
		printf("Found Position %d\n\r", position);
		switch(position){
			case 1:
			nrf24_setDataPipe(ptx1);
			break;
			case 2:
			nrf24_setDataPipe(ptx2);
			break;
			case 3:
			nrf24_setDataPipe(ptx3);
			break;
			case 4:
			nrf24_setDataPipe(ptx4);
			break;
			case 5:
			nrf24_setDataPipe(ptx5);
			break;
		}

	}	
	
}
void nrf24_setRxDataPipe(uint8_t * rxAddr, uint8_t configNode){
	
	if(configNode < 2){
						
		data_buffer[0]=  W_REGISTER | (REGISTER_MASK & PRX_ADDRESS[configNode]);

		spi_select_slave(&spi_master_instance, &slave, true);
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
		nrf24_transfer_wait();	
		for(int x = 0; x < NRF24_ADDR_LEN; x++){						
			data_buffer[x] = rxAddr[x];			
		}	
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,5);
		nrf24_transfer_wait();	
		spi_select_slave(&spi_master_instance, &slave, false);

		
	}else{
		
		data_buffer[0]=  W_REGISTER | (REGISTER_MASK & PRX_ADDRESS[configNode]);
		data_buffer[1] = rxAddr[0];

		spi_select_slave(&spi_master_instance, &slave, true);
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,2);
		nrf24_transfer_wait();	
		spi_select_slave(&spi_master_instance, &slave, false);			
	}	
}

uint8_t nrf24_dataReady()
{	
	uint8_t status = nrf24_getStatus();		
	return status & (1<<RX_DR);
}

uint8_t nrf24_rxFifoEmpty()
{	
	uint8_t val = nrf24_readRegister(FIFO_STATUS, 2);	

	return val & 1;
}

uint8_t nrf24_getPayLoad(void *buf, uint8_t len){
	
	uint8_t pipeNo =  ((nrf24_getStatus() >> RX_P_NO) & 0x07); 	
	
	for(int i = 0; i<len;i++){
		read_buffer[i] = 0;
	}

	uint8_t *data = buf;
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(R_RX_PAYLOAD);
	data_buffer[0]= R_RX_PAYLOAD;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,len+1);
	nrf24_transfer_wait();	
	spi_select_slave(&spi_master_instance, &slave, false);	

	for(int i = 0; i<len;i++){
		data[i] = read_buffer[i];		
	}		
	nrf24_configRegister(RF_STATUS,(1<<RX_DR)); // Reset status register		
	return pipeNo;
}

void nrf24_getAckPayLoad(void * buf, uint8_t len){
	
	printf("\n\r** Ack Payload Default ** \n\r");		
	
	for(int i = 0; i<len;i++){
		read_buffer[i] = 0;
	}
	uint8_t *data = buf;
	spi_select_slave(&spi_master_instance, &slave, true);
	nrf24_transfer(R_RX_PAYLOAD);
	data_buffer[0]= R_RX_PAYLOAD;
	spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,len+1);
	nrf24_transfer_wait();
	spi_select_slave(&spi_master_instance, &slave, false);

	for(int i = 0; i<len;i++){
		data[i] = read_buffer[i];
	}
	nrf24_configRegister(RF_STATUS,(1<<RX_DR)); // Reset status register
	nrf24_flushRx();	
	
}
void nrf24_setAckPlayLoadCallback(void	(* getAckPayLoad_cb)(uint8_t)){

	getAckPayLoad_callback = getAckPayLoad_cb;
}



uint8_t nrf24_getPacketLossCount()
{	
	uint8_t val = nrf24_readRegister(OBSERVE_TX,  2);
	uint8_t pl = val >> 4;
	if(pl >= 5){ // todo get from global
		nrf24_configRegister(RF_CH,freq_channel);
	}
	return pl;
	
}

void nrf24_sendPayLoad(void * buf, uint8_t len, uint8_t *ack_data){
	
	nrf24_ce_state(LOW);	
	nrf24_powerUpTx();		
	nrf24_flushTx();
	
	spi_select_slave(&spi_master_instance, &slave, true); 
	nrf24_transfer(W_TX_PAYLOAD); 
	nr424_transmit_data(buf,len); 
	spi_select_slave(&spi_master_instance, &slave, false); 

	/* Start the transmission - Pulse High  */	
	nrf24_ce_state(HIGH); 
	delay_us(15);
	nrf24_ce_state(LOW);

	while(nrf24_isSending()){}; // wait to finish or time out

}

void nrf24_setAckPayLoad(void * buf, uint8_t pipe, uint8_t len){

   spi_select_slave(&spi_master_instance, &slave, true);
   nrf24_transfer(W_ACK_PAYLOAD | pipe);  
   nr424_transmit_data(buf,len);   
   spi_select_slave(&spi_master_instance, &slave, false); 
	
}
uint8_t nrf24_isSending(void)
{   
	/* read the current status */
	uint8_t status = nrf24_getStatus();

	if(status & (1 << MAX_RT)){
		printf("Max RT Hit\n\r"); // just for trouble shooting
	}
	/* If sending successful (TX_DS) or max retries exceeded (MAX_RT). */
	if((status & ((1 << TX_DS)  | (1 << MAX_RT))))
	{		
		nrf24_ce_state(LOW); 

		// get ack payload
		uint8_t ack = nrf24_rxFifoEmpty();		
		
		if(ack == 0){ // rx has data when 0
					
			if(getAckPayLoad_callback){
				getAckPayLoad_callback(BUF_LENGTH);				
			} else {
				// call back not defined
				nrf24_getAckPayLoad(&ack_payload, BUF_LENGTH);	
			}
			
		}

		nrf24_powerUpRx();
		return 0; /* false */
	}

	return 1; /* true */

}
void nrf24_configure_spi_master()
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = EXT3_PIN_SPI_SS_0;// PIN_PB17
	
	spi_attach_slave(&slave, &slave_dev_config);
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.data_order =  SPI_DATA_ORDER_MSB; 
	config_spi_master.mux_setting = EXT3_SPI_SERCOM_MUX_SETTING;
	config_spi_master.pinmux_pad0 = EXT3_SPI_SERCOM_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	config_spi_master.pinmux_pad2 = EXT3_SPI_SERCOM_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = EXT3_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spi_master_instance, EXT3_SPI_MODULE, &config_spi_master);
	spi_enable(&spi_master_instance);

}

void nrf24_printDetails(){
	
	printf("STATUS:\t\t0x");	
	printf("%X\n\r", nrf24_getStatus());	
	
	printf("CONFIG:\t\t0x");
	nrf24_printRegister(RF_CONFIG);
	
	printf("RF_CH:\t\t0x");
	nrf24_printRegister(RF_CH);
	
	printf("RF SETUP:\t0x");
	nrf24_printRegister(RF_SETUP);
	
	printf("EN_AA:\t\t0x");
	nrf24_printRegister(EN_AA);
	
	printf("EN_RXADDR:\t0x");
	nrf24_printRegister(EN_RXADDR);

	printf("FEATURE:\t0x");
	nrf24_printRegister(FEATURE);

	printf("DYNPD:\t\t0x");
	nrf24_printRegister(DYNPD);

	printf("OBSERVE_TX:\t0x");
	nrf24_printRegister(OBSERVE_TX);
	
	printf("FIFO_STATUS:\t0x");
	nrf24_printRegister(FIFO_STATUS);
	
	
	// end read TX
	if(current_mode == RECEIVE){
		
		for(int x=0; x < 6; x++){

			switch(x){
				case 0:
				printf("RX_ADDR_P0:\t");
				break;
				case 1:
				printf("RX_ADDR_P1:\t");
				break;
				case 2:
				printf("RX_ADDR_P2:\t");
				break;
				case 3:
				printf("RX_ADDR_P3:\t");
				break;
				case 4:
				printf("RX_ADDR_P4:\t");
				break;
				case 5:
				printf("RX_ADDR_P5:\t");
				break;
			}
			
			if(x < 2){
				
				data_buffer[0] = R_REGISTER | (REGISTER_MASK & PRX_ADDRESS[x]);
				spi_select_slave(&spi_master_instance, &slave, true);	
				spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
				nrf24_transfer_wait();	
				spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,NRF24_ADDR_LEN);
				nrf24_transfer_wait();	
				spi_select_slave(&spi_master_instance, &slave, false);
				
				for(int i = 0; i < NRF24_ADDR_LEN;i++){			
					printf("0x%X ", read_buffer[i]);						
				}
				printf("\n\r");

			}else{

				uint8_t val = nrf24_readRegister(PRX_ADDRESS[x], 2);
				printf("0x%X ", val);

				for(int z=1;z<5;z++){					
					printf("0x%X ", ptx1[z]);					
				}				
				printf("\n\r");
				
			}
			
		}
		
	}else{		
	
		printf("TX_ADDR:\t");
		
		data_buffer[0] = (R_REGISTER + TX_ADDR);

		spi_select_slave(&spi_master_instance, &slave, true);		
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
		nrf24_transfer_wait();	
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,5);
		nrf24_transfer_wait();	
		spi_select_slave(&spi_master_instance, &slave, false);
		
		for(int i = 0; i < 5;i++){
			printf("0x%X ", read_buffer[i]);
		}
		printf("\n\r");
		
		// ---------------------------------- //
		printf("RX_ADDR_P0:\t");

		data_buffer[0] = R_REGISTER + RX_ADDR_P0;
		spi_select_slave(&spi_master_instance, &slave, true);		
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,1);
		nrf24_transfer_wait();	
		spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,5);
		nrf24_transfer_wait();	
		spi_select_slave(&spi_master_instance, &slave, false);		
		for(int i = 0; i < 5;i++){
			printf("0x%X ", read_buffer[i]);
		}
		printf("\n\r");
		
	}
	
}

void nrf24_printRegister(uint8_t reg){
	
	nrf24_readRegister(reg,  1);
	printf("%X\n\r", read_buffer[0]);

}

void nrf24_printNodeName(enum Node nd){

 switch(node){	 
	 case PRX:
		printf("\n\rReceiver Mode\n\r");
	 break;
	 case PTX1:
	 case PTX2:	
	 case PTX3:	
	 case PTX4:		
	 case PTX5:		
		printf("\n\rSender Mode %d\n\r", nd);
	 break;
	 
 }
}