/*
 * touch.c
 *
 * Created: 12/31/2017 9:46:24 AM
 *  Author: pvallone
 */ 
 #include <asf.h>
 #include "touch.h"

 volatile bool transrev_complete_spi_master = false;
 void configure_spi_master_callbacks(void)
 {
	 spi_register_callback(&spi_master_instance, callback_spi_master, SPI_CALLBACK_BUFFER_TRANSCEIVED);
	 spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
 }

 void configure_spi_master(void)
 {
	 struct spi_config config_spi_master;
	 struct spi_slave_inst_config slave_dev_config;
	 spi_slave_inst_get_config_defaults(&slave_dev_config);
	 slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	 
	 spi_attach_slave(&slave, &slave_dev_config);
	 spi_get_config_defaults(&config_spi_master);
	 config_spi_master.data_order =  SPI_DATA_ORDER_MSB; // not sure if this is correct
	 config_spi_master.mux_setting = EXT2_SPI_SERCOM_MUX_SETTING;
	 config_spi_master.pinmux_pad0 = EXT2_SPI_SERCOM_PINMUX_PAD0;
	 config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	 config_spi_master.pinmux_pad2 = EXT2_SPI_SERCOM_PINMUX_PAD2;
	 config_spi_master.pinmux_pad3 = EXT2_SPI_SERCOM_PINMUX_PAD3;
	 spi_init(&spi_master_instance, EXT2_SPI_MODULE, &config_spi_master);
	 spi_enable(&spi_master_instance);

 }
 void callback_spi_master( struct spi_module *const module)
 {
	 transrev_complete_spi_master = true;
 }
 
 void send_spi(uint8_t command, uint8_t data1, uint8_t data2){
	 
	 spi_select_slave(&spi_master_instance, &slave, true);
	 data_buffer[0] = command;
	 data_buffer[1] = data1;
	 data_buffer[2] = data2;
	 
	 spi_transceive_buffer_job(&spi_master_instance, data_buffer,read_buffer,BUF_LENGTH);
	 while (!transrev_complete_spi_master) {
		 /////* Wait for write and read complete */
	 }
	 transrev_complete_spi_master = false;
	 spi_select_slave(&spi_master_instance, &slave, false);
	 
 }
 uint8_t read_xy(void){

	 int z1, z2, tmpH, tmpL;
	 float tempX1 = 0.132706656f;
	 float tempY1 = 0.208116544f;
	 int z1Range = 3617;
	 int z2Range = 3844;
	 int z1_low = 282;
	 int z2_low = 128;
	 int z_threshold = 3800;

	 send_spi(0xB1, 0,0);
	 tmpH = (read_buffer[1] << 5);
	 tmpL = (read_buffer[2] >> 3);

	 z1 = tmpH | tmpL;
	 send_spi(0xC1, 0,0);
	 tmpH = (read_buffer[1] << 5);
	 tmpL = (read_buffer[2] >> 3);
	 z2 = tmpH | tmpL;
 
	 if((z2 - z1) < z_threshold){
	 
		 send_spi(0xD1, 0,0);
		 tmpH = (read_buffer[1] << 5);
		 tmpL = (read_buffer[2] >> 3);
		 tx =  tmpH | tmpL;
	 
		 send_spi(0x91, 0,0);
		 tmpH = (read_buffer[1] << 5);
		 tmpL = (read_buffer[2] >> 3);
		 ty =  tmpH | tmpL;		 		
	 
		 touch_x = abs(tempX1 * (float)(tx - z1_low));
		 touch_y = abs(tempY1 * (float)(ty - z2_low));

		 return 1;
	 
	 }
	return 0;

 }
