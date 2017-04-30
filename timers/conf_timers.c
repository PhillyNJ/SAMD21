/*
 * conf_timers.c
 *
 * Created: 4/30/2017 9:23:11 AM
 *  Author: pvallone
 */
 #include "conf_timers.h"
 #include <asf.h> 

void configure_tc(uint16_t channel_1_compare, uint16_t channel_2_compare){
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_16_bit.compare_capture_channel[0] = channel_1_compare;
	config_tc.counter_16_bit.compare_capture_channel[1] = channel_2_compare; 
	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);
	tc_enable(&tc_instance);
}

void configure_tc_callback_channel_0(tc_callback_t const callback_func){		
	tc_register_callback(&tc_instance, callback_func,TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

void configure_tc_callback_channel_1(tc_callback_t  const callback_func){
	tc_register_callback(&tc_instance, callback_func,TC_CALLBACK_CC_CHANNEL1);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL1);
}