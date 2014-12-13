#ifndef FAT_H
#define FAT_H

#include "Main.h"


#define JUMP_INST1 (0xEB)
#define JUMP_INST2 (0xE9)
#define RELSEC_OFFSET (0x01C6)
#define DRIVE_ERROR (75)
#define FAT16_ERROR (76)

uint8 read8(uint16 offset, uint8 * array_name);
uint16 read16(uint16 offset, uint8 * array_name);
uint32 read32(uint16 offset, uint8 * array_name);
uint8 mount_drive(uint8 * array_name); 
#endif