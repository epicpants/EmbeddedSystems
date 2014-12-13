#ifndef _SDCard_H
#define _SDCard_H

#include "Main.H"

// ------ Public Constant definitions -----------------------------

// error values
#define no_errors (0)
#define timeout_error (0x81)
#define illegal_cmnd (0x82)
#define response_error (0x83)
#define data_error (0x84)
#define voltage_error (0x85)
#define card_inactive (0x86)
#define SPI_error (0x87)

// SD Card types
#define unknown (0xFF)
#define Ver2 (0x02)
#define Standard_Capacity (9)  // used to shift address for block to byte address conversion
#define High_Capacity (0)      // no shift when no conversion is required

// SD Card Commands 
#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD55 55
#define CMD58 58
#define ACMD41 41


// ------ Public function prototypes -------------------------------
uint8 SEND_COMMAND(uint8 cmnd, uint32 argum);

uint8 response(uint8 num, uint8 * valout);

uint8 read_block(uint16 num, uint8 * array_out);

uint8 SD_card_init(void);

uint8 Return_SD_Card_Type(void);

void print_error(uint8 error);


#endif