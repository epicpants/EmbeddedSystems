#ifndef _SWITCH_H
#define _SWITCH_H

#include <delay.h>

#define YES (1)
#define NO (0)
#define SWITCH_NOT_PRESSED (0)
#define SWITCH_PRESSED (1)
bit SWITCH1_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH2_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH3_GET_INPUT(uint8 DEBOUNCE_PERIOD);
bit SWITCH4_GET_INPUT(uint8 DEBOUNCE_PERIOD);

#endif