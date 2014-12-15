#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

#define TICK_INTERVAL  ( 11 ) //tick interval in ms
#define ACTIVE         ( 0  ) //Buttons are active low.
#define INACTIVE       ( 1  )
#define DEBOUNCE_TIME  ( 50 ) // debounce time in ms
#define DEBOUNCE_TICKS ( DEBOUNCE_TIME / TICK_INTERVAL + 1 ) //55ms debounce time if tick interval is correct.

typedef uint8 button_state;
enum
{
	SW_NOT_PRESSED,
	DEBOUNCE,
	SW_PRESSED,
	SW_HELD,
	R_DEBOUNCE
};

typedef struct
{
	button_state state;
	uint8 num_ticks;
} button;

//This function will update the status of the button.	
void update_button(button * myButton, uint8 pin_status);

#endif
