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

uint8 I2C_Check_ACK(void)
{
	uint8 error_val = NO_ERRORS;
	I2C_clock_delay(CONTINUE);
	SCL = 0;
	SDA = 1;
	I2C_clock_delay(CONTINUE);
	SCL = 1;
	while(SCL != 1);
	if(SDA == NACK)
	{
		error_val = SLAVE_NACK;
	}
	return error_val;
}

uint8 I2C_Write(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name)
{
	uint8 error_val, index, write_byte;
	bit write_bit;

	error_val = NO_ERRORS;

	/*****************
	 Idle condition
	 ****************/
	SCL = 1;
	SDA = 1;

	if(SCL != 1 || SDA != 1)
	{
		error_val = BUS_BUSY;
		return error_val;
	}

	/*****************
	 Start condition
	 ****************/

	SDA = 0;

	/*****************
	 Specify Address
	 ****************/
	
	write_byte = (device_addr << 1); // R/W set to 0
	for(index = 0; index < 8; index++)
	{
		I2C_clock_delay(CONTINUE);
		SCL = 0;
		write_bit = ((write_byte >> (7 - index)) & 0x01);
		SDA = write_bit;
		I2C_clock_delay(CONTINUE);
		SCL = 1;
		while(SCL != 1);
		if(SDA != write_bit)
		{
			error_val = BUS_BUSY;
			return error_val;
		}
	}

	/*****************
	 Write array_name
	 ****************/

	index = 0;
	while(I2C_Check_ACK() == NO_ERRORS && index < number_of_bytes)
	{
		write_byte = array_name[index];
		for(index = 0; index < 8; index++)
		{
			I2C_clock_delay(CONTINUE);
			SCL = 0;
			write_bit = ((write_byte >> (7 - index)) & 0x01);
			SDA = write_bit;
			I2C_clock_delay(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			if(SDA != write_bit)
			{
				error_val = BUS_BUSY;
				return error_val;
			}
		}
		index++;
	}

	/*****************
	 Stop condition
	 ****************/

	I2C_clock_delay(CONTINUE);
	SCL = 0;
	SDA = 0;
	I2C_clock_delay(CONTINUE);
	SCL = 1;
	while(SCL != 1);
	SDA = 1;

	return error_val;
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
