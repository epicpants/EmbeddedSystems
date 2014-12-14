#include "button.h"

//This function will update the status of the button.
//pin_status has to be passed in because sbit types cannot be passed,
// and they cannot be included in structs.
void update_button(button * myButton, uint8 pin_status)
{
	switch(myButton->state)
	{
		case SW_NOT_PRESSED:
		{
			if(pin_status == ACTIVE)
			{
				myButton->state = DEBOUNCE;
				myButton->num_ticks = 0;
			}
			break;
		}
		case DEBOUNCE:
		{
			myButton->num_ticks++;	
			if( (pin_status == ACTIVE) && (myButton->num_ticks >= DEBOUNCE_TICKS) )
			{
				myButton->state = SW_PRESSED;
				myButton->num_ticks = 0;
			}
			else if( pin_status == INACTIVE )
			{
				myButton->state = SW_NOT_PRESSED;
				myButton->num_ticks = 0;
			}
			break;
		}
		case SW_PRESSED:
		{
			myButton->state = SW_HELD;
			break;
		}
		case SW_HELD:
		{
			if(pin_status == INACTIVE)
			{
				myButton->state = R_DEBOUNCE;
				myButton->num_ticks = 0;
			}
			break;

		}
		case R_DEBOUNCE:
		{
			myButton->num_ticks++;
			if( (pin_status == INACTIVE) && (myButton->num_ticks >= DEBOUNCE_TICKS) )
			{
				myButton->state = SW_NOT_PRESSED;
				myButton->num_ticks = 0;
			}
			else if( pin_status == ACTIVE )
			{
				myButton->state = SW_HELD;
				myButton->num_ticks = 0;
			}
			break;
		 }
		 default:
		 {
		 	myButton->state = SW_NOT_PRESSED;
			myButton->num_ticks = 0;
			break;
		 }
	 }
	 return;
}
