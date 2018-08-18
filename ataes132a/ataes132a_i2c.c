/*
 * ataes132a_i2c.c
 *
 * Created: 6/18/2018 6:33:21 AM
 *  Author: pvallone
 */ 

#include "ataes132a_config.h"
#include "ataes132a_i2c.h"


void aes_parse_status(ataes_transaction_status_t * status)
{
	uint8_t mask = status->aes_transaction_status;

	status->device_ready = (mask & 0x01);
	status->i2c_mode = (mask & 0x02) >> 1;
	status->sleep_standby_mode = (mask & 0x04) >> 2;
	status->checksum_error = (mask & 0x10) >> 4;
	status->memory_buffer_changed = (mask & 0x40) >> 6;
	status->error = (mask & 0x80) >>7;	
}

void aes_write_command(ataes_tranaction_t * packet, ataes_transaction_status_t * status){
		
	memset(aes_read_buffer, 0, sizeof aes_read_buffer);	 // clear
	memset(aes_write_buffer, 0, sizeof aes_write_buffer);	 // clear
	
	// assemble buffer for i2c communication	
	uint8_t *p_buffer;
	uint8_t checksum[2];
	
	uint8_t ckLen = packet->size_of_data_buffer + 6;
	
	if(ckLen < 6){
		printf("Data must be at least 9 bytes. You need %d more bytes in your data buffer.\n\r ", ckLen - 9);		
		status->error = true;
	};

	p_buffer = aes_write_buffer;
	packet->length+= sizeof packet->param1 + sizeof packet->param2 +  sizeof packet->mode  + sizeof packet->opCode + packet->size_of_data_buffer + 3;
	
	*p_buffer++ = packet->write_command >> 8;
	*p_buffer++ = packet->write_command & 0xFF;

	*p_buffer++ = packet->length; // must be at least nine
	*p_buffer++ = packet->opCode;	
	*p_buffer++ = packet->mode;	

	*p_buffer++ = packet->param1 >> 8;
	*p_buffer++ = packet->param1 & 0xFF;
	*p_buffer++ = packet->param2 >> 8;
	*p_buffer++ = packet->param2 & 0xFF;
			
	if (packet->size_of_data_buffer > 0) {
		memcpy(p_buffer, packet->data_buffer, packet->size_of_data_buffer);
		p_buffer += packet->size_of_data_buffer;
	}
		
	aes_calculate_checksum(packet->length - 2, &aes_write_buffer[2],checksum);	

	// add check sum to the end of the buffer
	*p_buffer++ = checksum[0];
	*p_buffer++ = checksum[1];
	
	// send to aes132a
	aes_w_packet.address    = AES_I2C_ADDRESS; // 0x50
	aes_w_packet.data_length = packet->length + 2;

	//printf("*** Size of Packet %d (0x%02X)\n\r", aes_w_packet.data_length,aes_w_packet.data_length);

	aes_w_packet.ten_bit_address = false;

	aes_w_packet.data  = aes_write_buffer;
	
	i2c_master_write_packet_wait(&aes_i2c_master_instance, &aes_w_packet);
	delay_ms(10);

	uint8_t returnBufferLen = 9;
	
	returnBufferLen = packet->rx_buffer_length;
	
	aes_rd_packet.address = AES_I2C_ADDRESS;
	aes_rd_packet.data_length = returnBufferLen; // must be at least 9 // todo get real data length 

	aes_rd_packet.ten_bit_address = false;
	aes_rd_packet.data = aes_read_buffer;

	i2c_master_read_packet_wait(&aes_i2c_master_instance, &aes_rd_packet);	
	status->return_count = aes_read_buffer[0]; // first byte is the number of bytes returned	
	status->aes_transaction_status = aes_read_buffer[1]; // is the transaction status  	 
	
	status->rx_buffer = aes_read_buffer; // not needed but keeps things tidy		
	

};
void aes_block_read(ataes_tranaction_t * packet, ataes_transaction_status_t * status){

	uint8_t checksum[2];	

	memset(aes_read_buffer, 0, sizeof aes_read_buffer);	 // clear
	memset(aes_write_buffer, 0, sizeof aes_write_buffer);	 // clear
	delay_ms(10);

	aes_write_buffer[0] = 0xFE; // write command hi byte
	aes_write_buffer[1] = 0x00; // write command low byte
	aes_write_buffer[2] = 0x09; // nine bytes sent
	aes_write_buffer[3] = BLOCKREAD;// 0x02
	aes_write_buffer[4] = 0x00; // mode is always 0x00

	aes_write_buffer[5] = (packet->param1 >> 8); 
	aes_write_buffer[6] = (packet->param1 & 0xFF);
	
	aes_write_buffer[7] = (packet->param2 >> 8); // param 2 - always 0x00
	aes_write_buffer[8] = (packet->param2 & 0xFF); // param 2 11 bytes o read

	aes_calculate_checksum(7, &aes_write_buffer[2], checksum);
		
	aes_write_buffer[9] = checksum[0];
	aes_write_buffer[10] = checksum[1];

	aes_w_packet.address    = AES_I2C_ADDRESS; 
	aes_w_packet.data_length = 11;
	aes_w_packet.ten_bit_address = false;
	aes_w_packet.data  = aes_write_buffer;

	i2c_master_write_packet_wait(&aes_i2c_master_instance, &aes_w_packet);
	delay_ms(10);
	aes_rd_packet.address = AES_I2C_ADDRESS;
	aes_rd_packet.data_length = packet->rx_buffer_length + 4;
	aes_rd_packet.ten_bit_address = false;
	aes_rd_packet.data = aes_read_buffer;

	i2c_master_read_packet_wait(&aes_i2c_master_instance, &aes_rd_packet);
	
	status->return_count = aes_read_buffer[0];
	status->aes_transaction_status = aes_read_buffer[1];	
	status->rx_buffer = aes_read_buffer;
	delay_ms(10);


}
int aes_memory_write(uint16_t address, uint8_t *data, uint8_t length, ataes_transaction_status_t *status){
	
	memset(aes_read_buffer, 0, sizeof aes_read_buffer);	 // clear
	memset(aes_write_buffer, 0, sizeof aes_write_buffer);	 // clear
	
	uint8_t high = address >> 8;
	uint8_t low = address & 0xFF;

	aes_write_buffer[0]= high;
	aes_write_buffer[1] = low;
	
	memcpy(&aes_write_buffer[2], data, length);
		
	aes_w_packet.address     = AES_I2C_ADDRESS;
	aes_w_packet.data_length = length + 2;
	aes_w_packet.ten_bit_address = false;
	aes_w_packet.data  = aes_write_buffer;
	i2c_master_write_packet_wait(&aes_i2c_master_instance, &aes_w_packet);
			
	aes_rd_packet.address = AES_I2C_ADDRESS;
	aes_rd_packet.data_length = 1;
	aes_rd_packet.ten_bit_address = false;
	aes_rd_packet.data = aes_read_buffer;

	i2c_master_read_packet_wait(&aes_i2c_master_instance, &aes_rd_packet);
	
	status->aes_transaction_status = aes_read_buffer[0];
	
	if(status->aes_transaction_status == BAD_CALL){

		printf("Status Return Error! Status = 0x%x\n\r",status->aes_transaction_status);
		aes_parse_status(status);
		aes_print_status(status);
		return BAD_CALL;
	}else {

		bool error = (status->aes_transaction_status & 0x80) >> 7;
		
		if(error){

			printf("Status Return Error! Status = 0x%x\n\r",status->aes_transaction_status);
			aes_parse_status(status);
			aes_print_status(status);
			return ATCA_GEN_FAIL;
		}

	}
	return SUCCESS;
}

void aes_memory_read(uint16_t address, uint8_t *out_data, uint8_t length){
	
	memset(aes_write_buffer, 0, sizeof aes_write_buffer);	 // clear
	memcpy(&aes_write_buffer[0], &address, 2);
	
	aes_w_packet.address     = AES_I2C_ADDRESS;
	aes_w_packet.data_length = 2;
	aes_w_packet.ten_bit_address = false;
	aes_w_packet.data  = aes_write_buffer;
	i2c_master_write_packet_wait(&aes_i2c_master_instance, &aes_w_packet);
	
	aes_rd_packet.address = AES_I2C_ADDRESS;
	aes_rd_packet.data_length = length;
	aes_rd_packet.ten_bit_address = false;
	aes_rd_packet.data = out_data;

	i2c_master_read_packet_wait(&aes_i2c_master_instance, &aes_rd_packet);	

}


/** \This function calculates a 16-bit CRC.
* \param[in] count number of bytes in data buffer
* \param[in] data pointer to data
* \param[out] crc pointer to calculated CRC (high byte at crc[0])
*/
void aes_calculate_checksum(uint8_t length, uint8_t *data, uint8_t *crc){

	uint8_t counter;
	uint8_t crcLow = 0, crcHigh = 0, crcCarry;
	uint8_t polyLow = 0x05, polyHigh = 0x80;
	uint8_t shiftRegister;
	uint8_t dataBit, crcBit;
	for (counter = 0; counter < length; counter++) {
		for (shiftRegister = 0x80; shiftRegister > 0x00; shiftRegister >>= 1) {
			dataBit = (data[counter] & shiftRegister) ? 1 : 0;
			crcBit = crcHigh >> 7;
			// Shift CRC to the left by 1.
			crcCarry = crcLow >> 7;
			crcLow <<= 1;
			crcHigh <<= 1;
			crcHigh |= crcCarry;
			if ((dataBit ^ crcBit) != 0) {
				crcLow ^= polyLow;
				crcHigh ^= polyHigh;
			}
		}
	}
	crc[0] = crcHigh;
	crc[1] = crcLow;

}


const char * aes_get_transaction_status(ataes132a_event_status_t * status){
	
	switch(*status){
		
		case SUCCESS:
		return "SUCCESS";			
		case BOUNDRY_ERROR:
		return "BOUNDRY_ERROR";		
		case RW_CONFING:
		return "RW_CONFING";	
		case BAD_ADDRESS:
		return "BAD_ADDRESS";		
		case COUNT_ERROR:
		return "COUNT_ERROR";		
		case NONCE_ERROR:
		return "NONCE_ERROR";		
		case MAC_ERROR:
		return "MAC_ERROR";		
		case PARSE_ERROR:
		return "PARSE_ERROR";		
		case DARA_MATCH:
		return "DARA_MATCH";		
		case UNKNOWN_ERROR:
		return "UNKNOWN_ERROR";		
		case INVALID_DATA_LENGTH:
		return "INVALID_DATA_LENGTH";		
		default:
		return "unknown error";		

	}
	return "unknown error";		
}


void aes_print_status(ataes_transaction_status_t * status){

	printf("device_ready?\t\t%s\n\r", !status->device_ready ? "True": "false");
	printf("i2c_mode?\t\t%s\n\r", !status->i2c_mode ? "True": "false");
	printf("sleep_standby_mode?\t%s\n\r", status->sleep_standby_mode ? "True": "false");
	printf("checksum_error?\t\t%s\n\r", status->checksum_error ? "True": "false");
	printf("memory_buffer_changed?\t%s\n\r", status->memory_buffer_changed ? "True": "false");
	printf("error?\t\t\t%s\n\r", status->error ? "True": "false");

	printf("AES Transaction Status:\t");

	switch(status->aes_transaction_status){
		
		case SUCCESS:
		printf("SUCCESS");
		break;
		case BOUNDRY_ERROR:
		printf("BOUNDRY_ERROR");
		break;
		case RW_CONFING:
		printf("RW_CONFING");
		break;
		case BAD_ADDRESS:
		printf("BAD_ADDRESS");
		break;
		case COUNT_ERROR:
		printf("COUNT_ERROR");
		break;
		case NONCE_ERROR:
		printf("NONCE_ERROR");
		break;
		case MAC_ERROR:
		printf("MAC_ERROR");
		break;
		case PARSE_ERROR:
		printf("PARSE_ERROR");
		break;
		case DARA_MATCH:
		printf("DARA_MATCH");
		break;
		case UNKNOWN_ERROR:
		printf("UNKNOWN_ERROR");
		break;
		case INVALID_DATA_LENGTH:
		printf("INVALID_DATA_LENGTH");
		break;
		default:
		printf("unknown error %d \n\r", status->aes_transaction_status);
		break;

	}
	printf("\n\r");
}

void aes_read_status(ataes_transaction_status_t *status){

	memset(aes_read_buffer, 0, sizeof aes_read_buffer);	 // clear
	memset(aes_write_buffer, 0, sizeof aes_write_buffer);	 // clear

	aes_write_buffer[0] = (RDSR >> 8); // RDSR = 0xFFF0
	aes_write_buffer[1] = (RDSR & 0xFF);
	aes_w_packet.address     = AES_I2C_ADDRESS;
	aes_w_packet.data_length = 2;
	aes_w_packet.ten_bit_address = false;
	aes_w_packet.data  = aes_write_buffer;
	i2c_master_write_packet_wait(&aes_i2c_master_instance, &aes_w_packet);

	aes_read_buffer[0] = (RDSR >> 8); // RDSR = 0xFFF0
	aes_read_buffer[1] = (RDSR & 0xFF);
	aes_rd_packet.address = AES_I2C_ADDRESS;
	aes_rd_packet.data_length = 1; 
	aes_rd_packet.ten_bit_address = false;
	aes_rd_packet.data = aes_read_buffer;

	i2c_master_read_packet_wait(&aes_i2c_master_instance, &aes_rd_packet);
	#ifdef AESSHOWSTATUS
	printf("* Status: 0x%x\n\r", aes_read_buffer[0]);
	#endif	
	status->aes_transaction_status = aes_read_buffer[0]; // first byte is the current event status 

	bool error = (status->aes_transaction_status & 0x80) >> 7;	

	if(error){

		printf("* Status Return Error! Status = 0x%x\n\r", status->aes_transaction_status);	
		aes_parse_status(status); 
		aes_print_status(status);
	}

	delay_ms(10);

}

void aes_configure_i2c(void)
{
	/* Initialize config structure and software module */
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);

	/* Change buffer timeout to something longer */
	config_i2c_master.buffer_timeout = 65535;

	/* Initialize and enable device with config */
	while(i2c_master_init(&aes_i2c_master_instance, SERCOM2, &config_i2c_master)  != STATUS_OK);

	i2c_master_enable(&aes_i2c_master_instance);
	
}

void aes_configure_i2c_callbacks(void)
{
	/* Register callback function. */
	i2c_master_register_callback(&aes_i2c_master_instance, aes_i2c_write_complete_callback,	I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	i2c_master_enable_callback(&aes_i2c_master_instance,I2C_MASTER_CALLBACK_WRITE_COMPLETE);
}

void aes_i2c_write_complete_callback(struct i2c_master_module *const module)
{
	/* Initiate new packet read */
	i2c_master_read_packet_job(&aes_i2c_master_instance,&aes_rd_packet);
}