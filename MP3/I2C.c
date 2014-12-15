/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: I2C.c
 * Brief: Implemented I2C functions for device communication
 */
#include "I2C.h"
#include <stdio.h>

#define ON (0)
#define OFF (1)

#define RELOAD (65536 - ((OSC_FREQ) / (OSC_PER_INST * 2 * I2C_FREQ)))
#define I2C_RELOAD_H (RELOAD >> 8)
#define I2C_RELOAD_L (RELOAD & 0x00FF)

void I2C_Delay_Start(void);
void I2C_Clock_Delay(uint8 control);

//This function uses timer1 to generate a delay for I2C communication.
//After this function is called the calling function should block until TF1 == 0.
void I2C_Delay_Start(void)
{
	TMOD &= 0x0F; //Set timer1 to 16-bit mode.
	TMOD |= 0X10; 

	ET1 = 0; // No interrupts
	TH1 = I2C_RELOAD_H;
	TH2 = I2C_RELOAD_L;
	TF1 = 0; // clear overflow flag
	TR1 = 1; // Start the timer
	return;
}

//This function will either stop the timer or reload it to continue based on the control value that is sent to it.
void I2C_Clock_Delay(uint8 control)
{
	if(TR1 == 1) // If the timer is running:
	{
		while(TF1 == 0); // Block until timer overflows.
	}

	TR1 = 0; // Stop the timer
	if(control == CONTINUE)
	{
		TH1 = I2C_RELOAD_H;
		TH2 = I2C_RELOAD_L;
 		TF1 = 0; // clear overflow flag.
		TR1 = 1; // Start timer.
	}
	else if(control == STOP)
	{
		TF1 = 0; // clear overflow flag.
	}
	
	return;
}

// Helper function to return Slave response as error (NO_ERRORS for ACK, SLAVE_NACK for NACK)
uint8 I2C_Check_ACK(void)
{
	uint8 error_val = NO_ERRORS;
	I2C_Clock_Delay(CONTINUE);
	SCL = 0;
	SDA = 1;
	I2C_Clock_Delay(CONTINUE);
	SCL = 1;
	while(SCL != 1);
	if(SDA == NACK)
	{
		error_val = SLAVE_NACK;
	}
	return error_val;
}

// This function will write number_of_bytes to the device at device_addr
// array_name contains the bytes to be written to the device
// Returns byte corresponding to error (NO_ERRORS, BUS_BUSY, SLAVE_NACK)
uint8 I2C_Write(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name)
{
	uint8 error_val, index, write_byte;
	bit write_bit;

	YELLOWLED = ON;
	error_val = NO_ERRORS;

	/*****************
	 Idle condition
	 ****************/
	SCL = 1;
	SDA = 1;

	if(SCL != 1 || SDA != 1)
	{
		error_val = BUS_BUSY;
		YELLOWLED = OFF;
		return error_val;
	}
	else
	{
		I2C_Delay_Start();
	}

	/*****************
	 Start condition
	 ****************/

	SDA = 0;

	/*****************
	 Specify Address
	 ****************/
	
	write_byte = (device_addr << 1); // R/W set to 0
	//printf("I2C write_byte = %2.2bX\n", write_byte);
	for(index = 0; index < 8; index++)
	{
		I2C_Clock_Delay(CONTINUE);
		SCL = 0;
		write_bit = ((write_byte >> (7 - index)) & 0x01);
		SDA = write_bit;
		I2C_Clock_Delay(CONTINUE);
		SCL = 1;
		while(SCL != 1);
		if(SDA != write_bit)
		{
			error_val = BUS_BUSY;
			YELLOWLED = OFF;
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
			I2C_Clock_Delay(CONTINUE);
			SCL = 0;
			write_bit = ((write_byte >> (7 - index)) & 0x01);
			SDA = write_bit;
			I2C_Clock_Delay(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			if(SDA != write_bit)
			{
				error_val = BUS_BUSY;
				YELLOWLED = OFF;
				return error_val;
			}
		}
		index++;
	}

	/*****************
	 Stop condition
	 ****************/

	I2C_Clock_Delay(CONTINUE);
	SCL = 0;
	SDA = 0;
	I2C_Clock_Delay(STOP);
	SCL = 1;
	while(SCL != 1);
	SDA = 1;
	
	YELLOWLED = OFF;
	return error_val;
}


//This function will read number_of_bytes from the device at device_addr into array_name.
//If there is an error, the return value will be non-zero.
uint8 I2C_Read(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name)
{
	uint8 index, error, check_value, count, byte_value;

	error = 0;

	SCL = 1;
	SDA = 1;
	
	if(SCL != 1 || SDA != 1)
	{
		error = BUS_BUSY;
		return error;
	}
	else
	{
		I2C_Delay_Start();
	}


	if(error == NO_ERRORS) // Start condition:
	{
		I2C_Clock_Delay(CONTINUE);
		if(SCL != 1 || SDA != 1)
		{
			error = BUS_BUSY;
			return error;
		}
		else
			SDA = 0;           //We do not need to check to ensure it is low because pulling low will always succeed with I2C.
	}

	if(error == NO_ERRORS) // Send address, msb first then read bit:
	{
		device_addr = ((device_addr << 1) | 0x01);
		//printf("device_addr = %2.2bX\n", device_addr);
		for(index = 0; index < 8 && error == NO_ERRORS; index++)
		{
			I2C_Clock_Delay(CONTINUE);
			SCL = 0;
			check_value = ((device_addr >> (7 - index)) & 0x01);
			if(check_value == 0x01)
				SDA = 1;
			else
				SDA = 0;

			I2C_Clock_Delay(CONTINUE);

			SCL = 1;
			while(SCL != 1); // Slow devices may need more time than we allow, so they will hold SCL low until they are finished.

			if(((SDA == 0) && (check_value == 0x01)) || ((SDA == 1) && (check_value == 0x00)))
			{
				error = BUS_BUSY;
				I2C_Clock_Delay(STOP);
				return error;
			}
		}	
	}

	if(error == NO_ERRORS) //Allow slave device to send ACK:
	{
		I2C_Clock_Delay(CONTINUE);
		SCL = 0;
		SDA = 1;
		I2C_Clock_Delay(CONTINUE);
		SCL = 1;
		while(SCL != 1);
		if(SDA != 0)
		{
			I2C_Clock_Delay(STOP);
			error = SLAVE_NACK;
			printf("Got SLAVE_NACK here\n");
		}
	}

	for(count = 0; (count < number_of_bytes) && (error == NO_ERRORS); count++) // Read in the response from the slave device:
	{
		printf("Inside the read for loop\n");
		printf("Number of bytes = %2.2bX\n", number_of_bytes);
		byte_value = 0;
		SDA = 1;
		for(index = 0; index < 8; index++)
		{
			I2C_Clock_Delay(CONTINUE);
			SCL = 0;
			SDA = 1;
			I2C_Clock_Delay(CONTINUE);
			SCL = 1;
			while(SCL != 1);
			byte_value |= ((uint8)SDA << (7 - index));
		}

		array_name[count] = byte_value;

		if(count < number_of_bytes - 1) // If we have to receive another byte, send ACK.
		{
			I2C_Clock_Delay(CONTINUE);
			SCL = 0;
			SDA = 0;
			I2C_Clock_Delay(CONTINUE);
			SCL = 1;
			SDA = 1;
			while(SCL != 1);
		}
		else // If we just received the last byte, send NACK:
		{
			I2C_Clock_Delay(CONTINUE);
			SCL = 0;
			SDA = 1;
			I2C_Clock_Delay(CONTINUE);
			SCL = 1;
			while(SCL != 1);
		}
	} // End of read loop.

	
	I2C_Clock_Delay(CONTINUE);
	SCL = 0;
	SDA = 0;
	I2C_Clock_Delay(STOP);
	SCL = 1;
	while(SCL != 1);
	SDA = 1;
	
	TR1 = 0; // Stop the timer we used for the I2C delays.
	return error;
}
