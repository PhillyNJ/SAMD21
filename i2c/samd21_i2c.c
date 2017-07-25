/*
 * samd21_i2c.c
 *
 * Created: 7/16/2017 6:47:24 AM
 *  Author: pvallone
 */ 
 #include "samd21_i2c.h"

 void samd21_configure_i2c(void)
 {
	 /* Initialize config structure and software module */
	 struct i2c_master_config config_i2c_master;
	 i2c_master_get_config_defaults(&config_i2c_master);

	 /* Change buffer timeout to something longer */
	 config_i2c_master.buffer_timeout = 65535;

	 /* Initialize and enable device with config */
	 while(i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE, &config_i2c_master)  != STATUS_OK);

	 i2c_master_enable(&i2c_master_instance);
	 
 }

 void samd21_configure_i2c_callbacks(void)
 {
	 /* Register callback function. */
	 i2c_master_register_callback(&i2c_master_instance, samd21_i2c_write_complete_callback,	I2C_MASTER_CALLBACK_WRITE_COMPLETE);
	 i2c_master_enable_callback(&i2c_master_instance,I2C_MASTER_CALLBACK_WRITE_COMPLETE);
 }

 void samd21_i2c_write_complete_callback(struct i2c_master_module *const module)
 {
	 /* Initiate new packet read */
	 i2c_master_read_packet_job(&i2c_master_instance,&read_packet);
 }

 void samd21_i2c_scan(void){
	 
	 uint8_t error = 0;
	 
	 for(uint8_t add = 1; add < 127; add++ ) {
		 
		 scan_packet.address  = add;
		 scan_packet.ten_bit_address = false;
		 read_buffer[0] = 0x00;
		 scan_packet.data_length = 1;
		 scan_packet.data = read_buffer;
		 error = i2c_master_write_packet_wait(&i2c_master_instance, &scan_packet);
		 
		 if (error == 0)
		 {
			 printf("I2C device found at decimal address Hex address is 0x%x\n", add);
			 
		 }
		 
	 }
 }
 