#ifndef _LED_H
#define _LED_H

#include <delay.h>

#define LEFT (0)
#define RIGHT (1)

uint8 LED_Change_State(uint8 current_state, bit DIRECTION);
#endif