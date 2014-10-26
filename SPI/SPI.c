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

uint8 send_command(uint8 command, uint32 argument)
{
	uint8 SPI_send, return_val;
	uint16 SPI_return;
	return_val = NO_ERRORS;
	if(command < 64) // less than 7 bits
	{
		/********
		 * Byte 1
		 ********/
		SPI_send = command | 0x40;
		SPI_return = SPI_Transfer(SPI_send);

		/********
		 * Byte 2
		 ********/
		if((SPI_return & 0xF000) == 0) // Check errors, 0 means no errors
		{
			SPI_send = argument >> 24; // MSB of argument
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 3
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x00FFFFFF;
			SPI_send = argument >> 16;
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 4
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x0000FFFF;
			SPI_send = argument >> 8;
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 5
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x000000FF;
			SPI_send = argument; // LSB of argument
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 6
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			if(command == 0) // CMD0
			{
				SPI_send = 0x95; // CMD0 CRC
			}
			else if(command == 8) // CMD8
			{
				SPI_send = 0x87; // CMD8 CRC
			}
			else
			{
				SPI_send = 0x01; // Don't care
			}
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}
	}
	else // Command invalid
	{
		return_val = ILLEGAL_COMMAND;
	}
	return return_val;
}

uint8 receive_response(uint8 number_of_bytes, uint8 * array_name)
{
	uint8 index, dat, error_val, return_val;
	uint16 SPI_value;
	index = 0;
	return_val = NO_ERRORS;
	do
	{
		SPI_value = SPI_Transfer(0xFF);
		error_val = (SPI_value & 0xF000)>>8;
		dat = SPI_value & 0x00FF; // dat is lower byte
		index++;
	} while((dat == 0xFF) && (error_val == 0x00) && (index != 0));

	if(error_val != 0) // Some sort of SPI error
	{
		return_val = SPI_ERROR;
	}
	else if(index == 0) // Timed out
	{
		return_val = TIMEOUT_ERROR;
	}
	else // Response is valid
	{
		*array_name = dat; // R1 response
		if((dat == 0x01) || (dat == 0x00)) // R1 response reported no errors
		{
			if(number_of_bytes > 1)
			{
				for(index = 1; index < number_of_bytes; index++)
				{
					SPI_value = SPI_Transfer(0xFF);
					dat = SPI_value & 0x00FF;
					*(array_name + index) = dat;
				}
			}
		}
	}
	SPI_value = SPI_Transfer(0xFF); // Allow SPI to get ready for next communication
	return return_val;
}