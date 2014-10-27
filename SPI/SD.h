#ifndef _SD_H
#define _SD_H

#include <PORT.h>
#include <stdio.h>
//public #defines:
#define UNKNOWN_TYPE (0)
#define SD (1)
#define SDHC (2)
#define IDLE_STATE (1)
#define BAD_CRC (72)
#define VOLTAGE_NOT_SUPPORTED (73)
#define BAD_VALUE (74)
#define CMD0 (0)
#define CMD0_ARGUMENT (0x00000000)
#define CMD8 (8)
#define CMD8_ARGUMENT (0x000001AA) //CRC7 for this is: 0x87 (1 appended for stop bit on end)
#define CMD8_CRC (0x87)
#define CMD41 (41)
#define CMD41_ARGUMENT_V2 (0x40000000)
#define CMD55 (55)
#define CMD55_ARGUMENT (0x00000000)
#define CMD58 (58)
#define SIZE_OF_R1 (1)
#define SIZE_OF_R3 (5)
#define SIZE_OF_R7 (5)
#define DATA_START_TOKEN (0xFE)

//SD Card public variables:
uint8 SD_Card_type = UNKNOWN_TYPE;


//SD Card public functions:
uint8 SD_Card_init();
uint8 read_block(uint16 number_of_bytes, uint8* array);

#endif