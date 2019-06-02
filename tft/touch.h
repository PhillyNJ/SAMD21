/*
 * touch.h
 *
 * Created: 12/31/2017 9:46:12 AM
 *  Author: pvallone
 */ 


#ifndef TOUCH_H_
#define TOUCH_H_

#define Z1LOW	275
#define Z1HIGH 3852
#define Z2LOW	329
#define Z2HIGH	3803
#define ZTHRESHOLD 3800 //Controls sensitivity. lower this value if you get random input values

#define BUF_LENGTH 3
#define BUF_LENGTH_READ 1
#define SLAVE_SELECT_PIN PIN_PA17 // PA17
int touch_x,touch_y,ty,tx;

// SPI Touch
struct spi_module spi_master_instance;
struct spi_slave_inst slave;

void configure_spi_master_callbacks(void);
void callback_spi_master( struct spi_module *const module);
void configure_spi_master(void);
void send_spi(uint8_t command, uint8_t data1, uint8_t data2);

uint8_t read_xy(void);
void formatFloat(float f, char * buffer);

static uint8_t read_buffer[3];
static uint8_t data_buffer[3];
#endif /* TOUCH_H_ */