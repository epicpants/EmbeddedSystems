/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: I2C.c
 * Brief: Prototypes for I2C functions for device communication
 */
#ifndef _I2C_H
#define _I2C_H

#include "AT89C51RC2.h"
#include "main.h"
#include "PORT.H"

#define STOP (0) //These two values are used to control the I2C_clock_delay function.
#define CONTINUE (1)


#define BUS_BUSY (57)
#define SLAVE_NACK (58)
#define ACK (0)
#define NACK (1)
#define NO_ERRORS (0)
#define I2C_FREQ (90000UL)


uint8 I2C_Write(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name);
//uint8 I2C_Read(uint8 device_addr, uint8 number_of_bytes, uint8 * array_name);
uint8 I2C_Check_ACK(void);



#endif
