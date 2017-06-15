/*
 * conf_at25dfx.c
 *
 * Created: 6/15/2017 7:20:13 AM
 *  Author: pvallone
 */ 
 #include "conf_at25dfx.h"

 void at25dfx_init(void)
 {
	
	 struct at25dfx_chip_config at25dfx_chip_config;
	 struct spi_config at25dfx_spi_config;
	
	 at25dfx_spi_get_config_defaults(&at25dfx_spi_config);
	 at25dfx_spi_config.mode_specific.master.baudrate = AT25DFX_CLOCK_SPEED;
	 at25dfx_spi_config.mux_setting = AT25DFX_SPI_PINMUX_SETTING;
	 at25dfx_spi_config.pinmux_pad0 = AT25DFX_SPI_PINMUX_PAD0;
	 at25dfx_spi_config.pinmux_pad1 = AT25DFX_SPI_PINMUX_PAD1;
	 at25dfx_spi_config.pinmux_pad2 = AT25DFX_SPI_PINMUX_PAD2;
	 at25dfx_spi_config.pinmux_pad3 = AT25DFX_SPI_PINMUX_PAD3;

	 spi_init(&at25dfx_spi, AT25DFX_SPI, &at25dfx_spi_config);
	 spi_enable(&at25dfx_spi);
	 
	
	 at25dfx_chip_config.type = AT25DFX_MEM_TYPE;
	 at25dfx_chip_config.cs_pin = AT25DFX_CS;

	 at25dfx_chip_init(&at25dfx_chip, &at25dfx_spi, &at25dfx_chip_config);

	
 }