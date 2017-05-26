/*
 * ssd1306_i2c.h
 *
 * Created: 5/9/2017 6:25:04 PM
 *  Author: pvallone
 */ 


#ifndef SSD1306_I2C_H_
#define SSD1306_I2C_H_


#define I2C_READ    1
#define I2C_WRITE	0

#if BOARD == SAMD21_XPLAINED_PRO
	#define CONF_I2C_MASTER_MODULE    SERCOM2
	#define ADDRESS 0x3C
	struct i2c_master_packet scan_packet;
	struct i2c_master_packet rd_packet;
	struct i2c_master_module i2c_master_instance;
	struct i2c_master_packet wr_packet;
#elif  BOARD == SAM4S_XPLAINED_PRO
	#define ADDRESS					0x78 // 8 but address
	#define TWI_CHANNEL				TWI0
	#define TWI_SPEED				400000 //Fast mode 400 khz
	twi_packet_t packet;
#endif

void configure_i2c(void);
void write_command(uint8_t cmd);


#endif /* SSD1306_I2C_H_ */