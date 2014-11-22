#include "I2C.h"


uint32 idata I2C_freq;
uint8 idata I2C_reloadH;
uint8 idata I2C_reloadL;

void I2C_Set_Frequency(uint32 freq);
void I2C_Delay_Start(void);
void I2C_clock_delay(uint8 control);

//This function will initialize the I2C_freq that is required for the I2C delay function.
void I2C_Set_Frequency(uint32 freq)
{
	uint16 reload;
	I2C_freq = freq;

	reload = 65536 - ((OSC_FREQ) / (OSC_PER_INST * 2 * I2C_freq)); // Calculate the reload value for a 16 bit timer
	I2C_reloadH = (reload >> 8); // Initialize the timer reload values here.
	I2C_reloadL = (reload & 0x00FF);
	return;
}

//This function uses timer1 to generate a delay for I2C communication.
//After this function is called the calling function should block until TF1 == 0.
void I2C_Delay_Start(void)
{
	TMOD &= 0x0F; //Set timer1 to 16-bit mode.
	TMOD |= 0X10; 

	ET1 = 0; // No interrupts
	TH1 = I2C_reloadH;
	TH2 = I2C_reloadL;
	TF1 = 0; // clear overflow flag
	TR1 = 1; // Start the timer
	return;
}

//This function will either stop the timer or reload it to continue based on the control value that is sent to it.
void I2C_clock_delay(uint8 control)
{
	if(TR1 == 1) // If the timer is running:
	{
		while(TF1 == 0); // Block until timer overflows.
	}

	TR1 = 0; // Stop the timer
	if(control == CONTINUE)
	{
		TH1 = I2C_reloadH;
		TH2 = I2C_reloadL;
 		TF1 = 0; // clear overflow flag.
		TR1 = 1; // Start timer.
	}
	else if(control == STOP)
	{
		TR1 = 0; // clear overflow flag.
	}
	
	return;
}

uint8 I2C_Write(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name)
{

}


//This function will read number_of_bytes from the device at device_addr into array_name.
//If there is an error, the return value will be non-zero.
uint8 I2C_Read(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name)
{
	uint8 index, error;

	error = 0;

	SCL = 1;
	SDA = 1;
	
	

	return error;
}
