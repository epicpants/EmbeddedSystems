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


	RESET = 1;
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
		if(send[0] != 0xFF)
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && error_val == NO_ERRORS);

	printf("Sent %lu bytes\n", index);

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
		if(send[0] != 0xFF);
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && error_val == NO_ERRORS);
	if(error_val != NO_ERRORS)
	{
		printf("Patch 2 write error = %2.2bX\n", error_val);
	}

	printf("Sent %lu bytes\n", index);

	//Configure STA013:
	if(error_val == NO_ERRORS)
	{
		
		
		
		send[0] = 0x54;
		send[1] = 0x07;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = 0x55;
		send[1] = 0x10;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)6;
		send[1] = (uint8)9;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)11;
		send[1] = (uint8)2;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)82;
		send[1] = (uint8)184;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)81;
		send[1] = (uint8)0;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)101;
		send[1] = (uint8)0;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)100;
		send[1] = (uint8)0;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)80;
		send[1] = (uint8)6;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)97;
		send[1] = (uint8)5;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)5;
		send[1] = (uint8)161;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS)
	{	
		send[0] = (uint8)0;
		send[1] = (uint8)0;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //Enable DATA_REQ:
	{	
		send[0] = 0x18;
		send[1] = 0x04;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //Set DATA_REQ to active high:
	{	
		send[0] = 0x0C;
		send[1] = 0x05;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //left attenuation:
	{	
		send[0] = 0x46;
		send[1] = 0x07;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //right attenuation:
	{	
		send[0] = 0x48;
		send[1] = 0x07;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //tone attenuation:
	{	
		send[0] = 0x70;
		send[1] = 0x07;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val == NO_ERRORS) //run:
	{	
		send[0] = 0x72;
		send[1] = 0x01;
		error_val = I2C_Write(STA013, 2, send);
	}

	if(error_val != NO_ERRORS)
		printf("Failed to configure the STA013.\n");
	return error_val;
}


#endif
