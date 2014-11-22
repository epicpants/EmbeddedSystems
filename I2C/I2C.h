

#ifndef _I2C_H
#define _I2C_H

#include "main.h"

uint8 I2C_Write(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name);
uint8 I2C_Read(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name);



#endif
