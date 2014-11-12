#ifndef _Directory_Func_H
#define _Directory_Func_H

#include "Main.H"

//------- Public Constants  -----------------------------------------
#define FAT32 (4)
#define FAT16 (2)
#define FAT32_shift (2)
#define FAT16_shift (1)
#define Disk_Error (0xF0)
#define No_Disk_Error (0)
#define more_entries (0x8000)   
#define no_entry_found (0x80000000)  // msb set to indicate error
#define directory_bit  (0x10000000)  // lsb of first nibble (bit28)

// ------ Public function prototypes -------------------------------
//uint8 read8(uint16 offset, uint8 * array_name);

//uint16 read16(uint16 offset, uint8 * array_name);

//uint32 read32(uint16 offset, uint8 * array_name);

//uint8 Mount_Drive(uint8 xdata * array_name);

//uint32 First_Sector (uint32 Cluster_num);

uint16 Print_Directory(uint32 Sector_num, uint8 xdata * array_in);

uint32 Read_Dir_Entry(uint32 Sector_num, uint16 Entry, uint8 xdata * array_in);

//uint32 Find_Next_Clus(uint32 Cluster_num, uint8 xdata * array_name);

//uint8 Open_File(uint32 Cluster, uint8 xdata * array_in);

#endif