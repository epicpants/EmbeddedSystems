#ifndef STA013_H
#define STA013_H

#include "Main.h"
#include "I2C.h"

#define STA013 (0x43)

extern uint8 code CONFIG;
extern uint8 code CONFIG2;

uint8 init_decoder()
{
	uint8 * p_out;
	uint8 send[2];
	uint8 error_val;
	uint32 index;



	printf("Applying Patch 1...\n");
	p_out = &CONFIG;
	index = 0;
	error_val = NO_ERRORS;
	do
	{
		send[0] = *(p_out + index);
		index++;
		send[1] = *(p_out + index);
		index++;
		if(send[0] != 0xFF && send[1] != 0XFF)
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && send[1] != 0xFF && error_val == NO_ERRORS);
	if(error_val != NO_ERRORS)
	{
		printf("Patch 1 write error = %2.2bX\n", error_val);
	}

	printf("Applying patch 2...\n");
	index = 0;
	p_out = &CONFIG2;
	error_val = NO_ERRORS;
	do
	{
		send[0] = *(p_out + index);
		index++;
		send[1] = *(p_out + index);
		index++;
		if(send[0] != 0xFF && send[1] != 0xFF);
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && send[1] != 0xFF && error_val == NO_ERRORS);
	if(error_val != NO_ERRORS)
	{
		printf("Patch 2 write error = %2.2bX\n", error_val);
	}


	return error_val;
}


#endif
