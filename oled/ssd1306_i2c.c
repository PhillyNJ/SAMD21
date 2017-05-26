/*
 * ssd1306_i2c.c
 *
 * Created: 5/9/2017 6:25:16 PM
 *  Author: pvallone
 */ 
 #include <asf.h>
 #include "ssd1306_i2c.h"

 void configure_i2c(void)
 {

     #if BOARD == SAMD21_XPLAINED_PRO
		 /* Initialize config structure and software module */
		 struct i2c_master_config config_i2c_master;
		 i2c_master_get_config_defaults(&config_i2c_master);

		 /* Change buffer timeout to something longer */
		 config_i2c_master.buffer_timeout = 65535;
		 config_i2c_master.transfer_speed = I2C_MASTER_SPEED_HIGH_SPEED;
		 /* Initialize and enable device with config */
		 while(i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE, &config_i2c_master)  != STATUS_OK);
		 i2c_master_enable(&i2c_master_instance);

     #elif  BOARD == SAM4S_XPLAINED_PRO

		gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
		gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
		
		uint32_t status;
		pmc_enable_periph_clk(ID_TWI0);
		//set options
		twi_master_options_t opt;

		opt.speed = TWI_SPEED;
		opt.chip = (ADDRESS >> 1);
		opt.master_clk = sysclk_get_cpu_hz();
		// Init TWI Master
		status = twi_master_setup(TWI_CHANNEL, &opt);		
		if (status != TWI_SUCCESS)
		{
			while(1); // wait to finish
		}
		
		twi_master_enable(TWI_CHANNEL);
     
     #endif
	
	 
 }
 void write_command(uint8_t cmd){	 

	 uint8_t buff[2];
	 buff[0] = I2C_WRITE;
	 buff[1] = cmd;
	 
	 #if BOARD == SAMD21_XPLAINED_PRO
		 wr_packet.address     = ADDRESS;
		 wr_packet.data_length = 2;
		 wr_packet.data        = buff;
		 i2c_master_write_packet_wait(&i2c_master_instance, &wr_packet);
	 #elif  BOARD == SAM4S_XPLAINED_PRO	
		packet.buffer = &buff;
		/* Data length */
		packet.length = 2;
		/* Slave chip address */
		packet.chip = (ADDRESS >> 1);
		
		twi_master_write(TWI_CHANNEL, &packet);
	 #endif
	
	 
 }
