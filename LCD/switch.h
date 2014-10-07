#ifndef _SWITCH_H
#define _SWITCH_H

#include <delay.h>

#define YES (1)
#define NO (0)
#define SWITCH_NOT_PRESSED (0)
#define SWITCH_PRESSED (1)
#define DEBOUNCE_TIME (5)
bit SWITCH1_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH2_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH3_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH4_GET_INPUT(uint8 DEBOUNCE_PERIOD);
//bit Read_Switch(uint8 SW_MASK, bit SW_State);
void CHECK_SWITCH_STATES(uint8 * array_sw_states, uint8 * array_sw_held);
#endif