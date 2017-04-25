/*
 * conf_usart.c
 *
 * Created: 2/12/2017 8:07:21 AM
 *  Author: pvallone
 */ 
 #include "conf_usart.h"
 
 void configure_console(uint32_t baud)
 {
	 struct usart_config config_usart;
	 usart_get_config_defaults(&config_usart);

	 config_usart.baudrate    = baud;
	 config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	 config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	 config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	 config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	 config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

	 while (usart_init(&usart_instance,
	 EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	 }

	 stdio_serial_init(&usart_instance, CONF_STDIO_USART_MODULE, &config_usart);
	 usart_enable(&usart_instance);
 }

 void serialRead(uint16_t *const buffer){
	 
	 if (usart_read_wait(&usart_instance, buffer) == STATUS_OK) {
		 
	 }
 }
