/** -----------------------------------------------------------------------------

RF24		SAMD21 xplained Pro
CE			PB11
CSN			PB17
SCK			PB23
MOSI		PB22
MISO		PB16

*/

#ifndef NRF24
#define NRF24

#include "nRF24L01.h"
#include <stdint.h>
#include <asf.h>

#define LOW 0
#define HIGH 1

#define CE		PORT_PB11 // EXT3

#define NRF24_ADDR_LEN 5
#define BUF_LENGTH 32
#define READ_BUF_LENGTH 33

uint8_t payload_len;
uint8_t freq_channel;

enum Node{ // simple 1 receiver, 5 transmitters
	
	PRX	 = 0, // receiver
	PTX1 = 1, // transmitter
	PTX2 = 2, // transmitter
	PTX3 = 3,
	PTX4 = 4,
	PTX5 = 5	
};

uint8_t lookup_table[50][5];

enum Mode {
	
	TRANSMIT = 0,
	RECEIVE = 1,
};

struct spi_module spi_master_instance;
struct spi_slave_inst slave;

int8_t data_array[BUF_LENGTH];// 32 bytes max
int8_t ack_payload[BUF_LENGTH];

typedef struct { // not sure if I ma going to use this

	bool has_ack;
	bool is_sending;
	
} nrf24_status_t; 


typedef struct  {
	
	uint8_t id;
	uint16_t data;

} nrf24_packet_t;

nrf24_packet_t pck;


uint8_t read_buffer[READ_BUF_LENGTH];
uint8_t data_buffer[BUF_LENGTH];

/* SPI core functions */
void	nrf24_configure_spi_master_callbacks(void);
void	nrf24_configure_spi_master(void);

/* nRF24 misc functions */
void    nrf24_init(enum Node nd);
void	nrf24_printDetails(void);
void	nrf24_printRegister(uint8_t reg);
void	nrf24_printNodeName(enum Node nd);
void	nrf24_setRxDataPipe(uint8_t * rxAddr, uint8_t configNode);
void	nrf24_setDataPipe(uint8_t * addr);
void	nrf24_configure_datapipe(uint8_t reg, uint8_t * value, uint8_t len);
void    nrf24_config(uint8_t channel, uint8_t pay_length);

void    nrf24_configure_star_network(uint8_t channel);
void    nrf24_configure_star_node(uint8_t node_id, enum Mode md);
/* nRF24 state check functions */
uint8_t nrf24_dataReady(void);
uint8_t nrf24_isSending(uint8_t *ack_data);
uint8_t nrf24_getStatus(void);
uint8_t nrf24_rxFifoEmpty(void);
uint8_t nrf24_txIsFull(void);

/* nRF24 payload */
uint8_t	nrf24_getPayLoad(void * buf, uint8_t len);
void    nrf24_sendPayLoad(void * buf, uint8_t len, uint8_t *ack_data);
uint8_t nrf24_getPayloadLength(void);
void	nrf24_setAckPayLoad(void * buf, uint8_t pipe, uint8_t len);

/* post transmission analysis */
uint8_t nrf24_getPacketLossCount(void);
void	nrf24_write_register(uint8_t reg, uint8_t * value, uint8_t len);
void    nrf24_configRegister(uint8_t reg, uint8_t value);
uint8_t	nrf24_readRegister(uint8_t reg, uint8_t len);

/* nRF24 power management */
void    nrf24_powerUpRx(void);
void    nrf24_powerUpTx(void);
void    nrf24_powerDown(void);

/*  nRF24flush methods*/
void	nrf24_flushRx(void);
void	nrf24_flushTx(void);

/* nRF24 SPI interface ... */

void	nrf24_transfer(uint8_t data);
void	nr424_transmit_data (uint8_t * data, uint8_t len);
void	nrf24_ce_state(uint8_t state);
void	nrf24_transfer_wait(void);

#endif
