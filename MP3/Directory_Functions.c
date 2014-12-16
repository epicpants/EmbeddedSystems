#include "AT89C51RC2.h"
#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "UART.h"
#include "SPI.h"
#include "SD.h"
#include "Directory_Functions.h"
#include "print.h"



uint32 idata FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
uint16 idata BytesPerSec_g;
uint8 idata SDtype_g, SecPerClus_g, FATtype_g, BytesPerSecShift_g, FATshift_g;

/***********************************************************************
DESC: Prints all short file name entries for a given directory 
INPUT: Starting Sector of the directory and the pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: Uint16 number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/
uint16  Print_Directory(uint32 Sector_num, uint8 xdata * array_in)
{ 
   uint32 Sector, max_sectors;
   uint16 i, entries;
   uint8 temp8, j, attr, out_val, error_flag;
   uint8 * values;

   AUXR = 0x0C; // Allows use of 1 KB of RAM
   values=array_in;
   entries=0;
   i=0;
   if (Sector_num<FirstDataSec_g)  // included for FAT16 compatibility
   { 
      max_sectors=RootDirSecs_g;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=SecPerClus_g;
   }
   Sector=Sector_num;
   nCS0=0;
   error_flag=send_command(17,(Sector<<SDtype_g));
   if(error_flag==NO_ERRORS) error_flag=read_block(512, values);
   nCS0=1;
   if(error_flag==NO_ERRORS)
   {
     do
     {
 
	    temp8=read8(0+i,values);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read8(0x0b+i,values);
		   if((attr&0x0E)==0)   // if hidden, system or Vol_ID bit is set do not print
		   {
		      entries++;
			  printf("%5d. ",entries);  // print entry number with a fixed width specifier
		      for(j=0;j<8;j++)
			  {
			     out_val=read8(i+j,values);   // print the 8 byte name
			     putchar(out_val);
			  }
              if((attr&0x10)==0x10)  // indicates directory
			  {
			     for(j=8;j<11;j++)
			     {
			        out_val=read8(i+j,values);
			        putchar(out_val);
			     }
			     printf("[DIR]\n");
			  }
			  else       // print a period and the three byte extension for a file
			  {
			     putchar(0x2E);       
			     for(j=8;j<11;j++)
			     {
			        out_val=read8(i+j,values);
			        putchar(out_val);
			     }
			     putchar(0x0d);
                 putchar(0x0a);
			  }
		    }

		}
		i=i+32;  // next entry

		if(i>510)
		{
		  Sector++;
          if((Sector-Sector_num)<max_sectors)
		  {
              nCS0=0;
              error_flag=send_command(17,(Sector<<SDtype_g));
              if(error_flag==NO_ERRORS) error_flag=read_block(512, values);
			  if(error_flag!=NO_ERRORS)
			    {
			      entries=0;   // no entries found indicates disk read error
				  temp8=0;     // forces a function exit
			    }
                nCS0=1;
			    i=0;
		  }
		  else
		  {
			  entries=entries|more_entries;  // set msb to indicate more entries in another cluster
			  temp8=0;                       // forces a function exit
		  }
		}
       
	  }while(temp8!=0);
	}
	else
	{
		entries=0;    // no entries found indicates disk read error
	}
    return entries;
 }


/***********************************************************************
DESC: Uses the same method as Print_Directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32 with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/

uint32 Read_Dir_Entry(uint32 Sector_num, uint16 Entry, uint8 xdata * array_in)
{ 
   uint32 Sector, max_sectors, return_clus;
   uint16 i, entries;
   uint8 temp8, attr, error_flag;
   uint8 * values;

   AUXR = 0x0C; // Allows use of 1 KB of RAM
   values=array_in;
   entries=0;
   i=0;
   return_clus=0;
   if (Sector_num<FirstDataSec_g)  // included for FAT16 compatibility
   { 
      max_sectors=RootDirSecs_g;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=SecPerClus_g;
   }
   Sector=Sector_num;
   nCS0=0;
   error_flag=send_command(17,(Sector<<SDtype_g));
   if(error_flag==NO_ERRORS)  error_flag=read_block(512, values);
   nCS0=1;
   if(error_flag==NO_ERRORS)
   {
     do
     {
        temp8=read8(0+i,values);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read8(0x0b+i,values);
		   if((attr&0x0E)==0)    // if hidden do not print
		   {
		      entries++;
              if(entries==Entry)
              {
			    if(FATtype_g==FAT32)
                {
                   return_clus=read8(21+i,values);
				   return_clus&=0x0F;            // makes sure upper four bits are clear
				   return_clus=return_clus<<8;
                   return_clus|=read8(20+i,values);
                   return_clus=return_clus<<8;
                }
                return_clus|=read8(27+i,values);
			    return_clus=return_clus<<8;
                return_clus|=read8(26+i,values);
			    attr=read8(0x0b+i,values);
			    if(attr&0x10) return_clus|=directory_bit;
                temp8=0;    // forces a function exit
              }
              
		   }
        }
		i=i+32;  // next entry
		if(i>510)
		{
		   Sector++;
		   if((Sector-Sector_num)<max_sectors)
		   {
              nCS0=0;
              error_flag=send_command(17,(Sector<<SDtype_g));
              if(error_flag==NO_ERRORS)  error_flag=read_block(512, values);
              nCS0=1;
			  if(error_flag!=NO_ERRORS)
			  {
			     return_clus=no_entry_found;
                 temp8=0; 
			  }
			  i=0;
		   }
		   else
		   {
			  temp8=0;                       // forces a function exit
		   }
		}
        
	 }while(temp8!=0);
   }
   else
   {
	 return_clus=no_entry_found;
   }
   if(return_clus==0) return_clus=no_entry_found;
   return return_clus;
}

//This function will return the first sector in a cluster.
//If cluster_num is 0 it will return FirstRootDirSec_g.
uint32 First_Sector (uint32 Cluster_num)
{
	uint32 returnVal;

	if(Cluster_num == 0)
	{
		returnVal = FirstRootDirSec_g;
	}
	else
	{
		returnVal = ((Cluster_num - 2) * SecPerClus_g) + FirstDataSec_g;
	}

	return returnVal;
}

//This function will find the next cluster # of a file.
//The first sector of that cluster is loaded into array_name.
uint32 Find_Next_Clus(uint32 Cluster_num, uint8 xdata * array_name)
{
	uint32 returnVal;
	uint32 thisFatSecNum;
	uint32 thisFatEntOffset;
	uint32 FAToffset;
	uint8 errorVal;
	
	errorVal = 0;
	returnVal = 0;

	thisFatSecNum = StartofFAT_g + ((Cluster_num * FATtype_g) / BytesPerSec_g); //FATtype_g should be: 4 for FAT32, 2 for FAT16.
	
	//Reading in first sector of next cluster:
	nCS0 = 0;
	errorVal = send_command(CMD17, thisFatSecNum);
	if(errorVal != NO_ERRORS)
	{
		printf("Error sending CMD17 in Find_Next_Clus: %2.2BX.\n", errorVal);
		nCS0 = 1;
		return 0;
	}

	errorVal = read_block(BytesPerSec_g, array_name);
	nCS0 = 1;
	if(errorVal != NO_ERRORS)
	{
		printf("Error reading block in Find_Next_Clus.\nSector Number %i.\nErrorVal: %2.2BX.\n", thisFatSecNum, errorVal);
		return 0;
	}
	
	FAToffset = Cluster_num * FATtype_g;  //FATtype_g should be 4 for FAT32.
	thisFatEntOffset = FAToffset % BytesPerSec_g;

	if(FATtype_g == FAT32)
	{
		returnVal = read32(thisFatEntOffset, array_name);
		returnVal = (returnVal & 0x0fffffff); // Top 4 bits must be masked because only lower 28 bits are used.
	}
	else if(FATtype_g == FAT16)
	{
		returnVal = (uint32)read16(thisFatEntOffset, array_name);
	}

	return returnVal;
}

//This function will return the 8 bit value at offset in array_name.
uint8 read8(uint16 offset, uint8 xdata * array_name)
{
	uint8 return_val = array_name[offset];
	return return_val;
}

//This function will return the 16 bit value (Little Endian) at offset in array_name.
uint16 read16(uint16 offset, uint8 xdata * array_name)
{
	uint16 return_val = 0;
	uint8 temp, index;
	for(index = 0; index < 2; index++)
	{
		return_val = return_val << 8;
		temp = array_name[offset + (1-index)];
		return_val = (return_val | ((uint16) temp));
	}
	return return_val;
}

//This function will return the 32 bit value (Little Endian) at offset in array_name.
uint32 read32(uint16 offset, uint8 xdata * array_name)
{
	uint32 idata return_val = 0;
	uint8 temp, index;
	for(index = 0; index < 4; index++)
	{
		return_val = return_val << 8;
		temp = array_name[offset + (3-index)];
		//printf("%2.2BX ", temp);
		return_val = (return_val | ((uint32) temp));
	}
	//printf("\n");
	return return_val;
}

//This function will mount an SD drive. It is only valid for FAT32. It will initialize all of the globals
//  that are declared at the top of this file.
uint8 mount_drive(uint8 xdata * array_name)
{
	uint32 idata block_num, TotSec32, RootClus, FATSz32, FATSz, DataSec, CountofClus, TotSec, RelSec;
	uint8 error_val, value8, NumFATs;
	uint16 idata offset, RsvdSecCnt, RootEntCnt, TotSec16, FATSz16;

	AUXR = 0x0C; // Allows use of 1 KB of RAM
	block_num = 0;
	RelSec = 0;
	nCS0 = 0;
	GREENLED = 0;
	error_val = send_command(CMD17, block_num); //sending command to read the sector at 0.
	if(error_val == NO_ERRORS)
	{
		
		error_val = read_block(512, array_name);//reading in data.
		GREENLED = 1;
		if(error_val == NO_ERRORS)
		{
			offset = 0;
			value8 = read8(offset, array_name);//Check the value at offset 0 to see if it is a jump instruction:
			if(value8 != JUMP_INST1 && value8 != JUMP_INST2) //If it is not a jump instruction:
			{
				offset = 0x01c6; //read in the 32 bit value at 0x01C6:
				printf("OFFSET = %u\n", offset);
				block_num = read32(offset, array_name);
				RelSec = block_num;
				printf("block_num: %lu\n", block_num);
				error_val = send_command(17, block_num); //send command to read in the sector that is specified by the 32 bit value at offset.
				if(error_val == NO_ERRORS)
				{
					error_val = read_block(512, array_name); // Read in data at sector.
					if(error_val == NO_ERRORS)
					{
						offset = 0;
						value8 = read8(offset, array_name); //Make sure there is a jump instruction at the first block:
						if(value8 != JUMP_INST1 && value8 != JUMP_INST2)
						{
							error_val = DRIVE_ERROR; // If not jump instruction return error.
						}
					}
				}
			}
			if(error_val == NO_ERRORS)
			{
				// Read global values in:
				BytesPerSec_g = read16(0x0B, array_name);
				printf("BytesPerSec_g: %u.\n", BytesPerSec_g);
				SecPerClus_g = read8(0x0D, array_name);
				printf("SecPerClus_g: %hu.\n", SecPerClus_g);
				RsvdSecCnt = read16(0x0E, array_name);
				printf("RsvdSecCnt: %u.\n", RsvdSecCnt);
				NumFATs = read8(0x10, array_name);
				printf("NumFATs: %hu.\n", NumFATs);
				RootEntCnt = read16(0x11, array_name);
				printf("RootEntCnt: %u.\n", RootEntCnt);
				TotSec16 = read16(0x13, array_name);
				printf("TotSec16: %u.\n", TotSec16);
				TotSec32 = read32(0x20, array_name);
				printf("TotSec32: %lu.\n", TotSec32);
				FATSz16 = read16(0x16, array_name);
				printf("FATSz16: %u.\n", FATSz16);
				FATSz32 = read32(0x24, array_name);
				printf("FATSz32: %lu.\n", FATSz32);
				RootClus = read32(0x2C, array_name);
				printf("RootClus: %lu.\n", RootClus);

				// Calculate RootDirSectors
				RootDirSecs_g = ((RootEntCnt * 32) + (BytesPerSec_g - 1)) / BytesPerSec_g;
				printf("RootDirSecs_g: %lu.\n", RootDirSecs_g);

				// Determine FATSz and TotSec
				FATSz = FATSz16;
				if(FATSz == 0)
				{
					FATSz = FATSz32;
				}
				TotSec = TotSec16;
				if(TotSec == 0)
				{
					TotSec = TotSec32;
				}

				// Calculate DataSec
				DataSec = TotSec - (RsvdSecCnt + (NumFATs * FATSz) + RootDirSecs_g);
				printf("DataSec: %lu.\n", DataSec);

				// Calculate CountofClus
				CountofClus = DataSec / SecPerClus_g;
				printf("CountofClus: %lu.\n", CountofClus);

				// Determine FAT type
				if(CountofClus < 65525)
				{
					FATtype_g = FAT16;
					error_val = FAT16_ERROR; // FAT16 unsupported
				}
				else
				{
					FATtype_g = FAT32;
					SDtype_g = 0;
					printf("Set SDtype\n");
				}

				StartofFAT_g = RsvdSecCnt + RelSec;
				printf("StartofFAT_g: %lu.\n", StartofFAT_g);
				FirstDataSec_g = RsvdSecCnt + (NumFATs * FATSz) + RootDirSecs_g + RelSec;
				printf("FirstDataSec_g: %lu.\n", FirstDataSec_g);
				FirstRootDirSec_g = ((RootClus - 2) * SecPerClus_g) + FirstDataSec_g;
				printf("FirstRootDirSec_g: %lu.\n", FirstRootDirSec_g);
			}
			printf("Mount drive error is: %2.2BX\n", error_val);
		}
		else
		{
			printf("Error in read block\n");
		}
		
	}
	else if(error_val == TIMEOUT_ERROR)
	{
		printf("Timeout error occurred\n");
	}
	else if(error_val == SPI_ERROR)
	{
		printf("SPI error occurred\n");
	}
	else // This should not happen
	{
		printf("SPI error occurred: %2.2bx\n", error_val);
	}
	nCS0 = 1;
	return error_val;
}

//This function will print the sectors of data starting with the first sector of Cluster. 
//It will initialize array_in.
uint8 Open_File(uint32 Cluster, uint8 xdata * array_in)
{
	uint8 error_val, SectorOffset, stop, input;
	uint32 StartingSector, cluster_num;
	
	StartingSector = First_Sector(Cluster);
	SectorOffset = 0;
	stop = 0;
	nCS0 = 0;
	while(error_val == NO_ERRORS && !stop) 
	{
		error_val = send_command(17, (StartingSector + SectorOffset)); // send command to read in block.
		if(error_val == NO_ERRORS)
		{
			error_val = read_block(BytesPerSec_g, array_in); // read in data.
			if(error_val == NO_ERRORS)
			{
				print_memory_block(BytesPerSec_g, array_in); // Print block.
				printf("Press Esc to stop, anything else to continue..."); // Prompt user for input.
				scanf("%c", &input);
				if(input == ESC) 
				{
					stop = 1;
				}
				else //If input != ESC, print the next sector.
				{
					SectorOffset++;
					if(SectorOffset == SecPerClus_g)
					{
						SectorOffset = 0;
						stop = 1;
						cluster_num = Find_Next_Clus(cluster_num, array_in);
					}
				}
			}
			else
			{
				printf("Read block error\n");
			}
		}
		else
		{
			printf("Send command error\n");
		}
	}
	
	nCS0 = 1;
	return error_val;
}
