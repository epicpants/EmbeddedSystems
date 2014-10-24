#include <SPI.h>

uint8 SPI_Master_Init(uint32 clock_rate)
{
	uint8 divider, return_val;
	return_val = INIT_OK;
	divider = (uint8)(((OSC_FREQ / OSC_PER_INST) * 6) / clock_rate);
	SPCON |= 0x70; // Set SPEN, SSDIS, and MSTR
	SPCON &= 0xF3; // Clear CPOL and CPHA
	if(divider > 128)
	{
		return_val = INIT_ERR;
	}
	else if(divider <= 128 && divider > 64)
	{
		SPCON |= 0x82;
		SPCON &= 0xFE;
	}
	else if(divider <=64 && divider > 32)
	{
		SPCON |= 0x81;
		SPCON &= 0xFD;
	}
	else if(divider <=32 && divider > 16)
	{
		SPCON |= 0x80;
		SPCON &= 0xFC;
	}
	else if(divider <= 16 && divider > 8)
	{
		SPCON |= 0x03;
		SPCON &= 0x7F;
	}
	else if(divider <= 8 && divider > 4)
	{
		SPCON |= 0x02;
		SPCON &= 0x7E;
	}
	else if(divider <= 4 && divider > 2)
	{
		SPCON |= 0x01;
		SPCON &= 0x7D;
	}
	else
	{
		SPCON &= 0x7C;
	}
	return return_val;
}

uint16 SPI_Transfer(uint8 send_value)
{
	uint8 test;
	uint16 data_output, timeout;
	timeout = 0;
	// Start a transfer
	SPDAT = send_value;
	// Wait for transfer to be complete
	do
	{
		test = SPSTA;
		timeout++;
	} while((test & 0x80) != 0x80 && timeout != 0);
	
	if(timeout != 0)
	{
		data_output = (SPSTA & 0x70) << 8;
		data_output |= SPDAT;
	}
	else // Timed out
	{
		data_output = TIMEOUT_ERROR;	
	}
	return data_output;
}