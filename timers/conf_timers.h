/*
 * conf_timers.h
 *
 * Created: 4/30/2017 9:22:58 AM
 *  Author: pvallone
 */ 


#ifndef CONF_TIMERS_H_
#define CONF_TIMERS_H_

#include <asf.h>

#define CONF_TC_MODULE TC3
struct tc_module tc_instance;
void configure_tc(uint16_t channel_0_compare, uint16_t channel_1_compare);
void configure_tc_callback_channel_0(tc_callback_t const callback_func);
void configure_tc_callback_channel_1(tc_callback_t const callback_func);

#endif /* CONF_TIMERS_H_ */