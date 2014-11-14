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
uint8 idata SDtype_g, SecPerClus_g, FATtype_g, BytesPerSecShift_g,FATshift_g;


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
   AMBERLED=0;
   nCS0=0;
   error_flag=send_command(17,(Sector<<SDtype_g));
   if(error_flag==NO_ERRORS) error_flag=read_block(512, values);
   nCS0=1;
   AMBERLED=1;
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

uint8 read8(uint16 offset, uint8 * array_name)
{
	uint8 return_val = *(array_name + offset);
	return return_val;
}

uint16 read16(uint16 offset, uint8 * array_name)
{
	uint16 return_val = 0;
	uint8 temp, index;
	for(index = 0; index < 2; index++)
	{
		return_val = return_val << 8;
		temp = *(array_name + offset + (1-index));
		return_val = return_val | temp;
	}
	return return_val;
}

uint32 read32(uint16 offset, uint8 * array_name)
{
	uint32 return_val = 0;
	uint8 temp, index;
	for(index = 0; index < 4; index++)
	{
		return_val = return_val << 8;
		temp = *(array_name + offset + (3-index));
		return_val = return_val | temp;
	}
	return return_val;
}

uint8 mount_drive(uint8 * array_name)
{
	uint32 block_num, TotSec32, RootClus, FATSz32, FATSz, DataSec, CountofClus, TotSec, RelSec;
	uint8 error_val, value8, NumFATs;
	uint16 offset, RsvdSecCnt, RootEntCnt, TotSec16, FATSz16;

	block_num = 0;
	RelSec = 0;
	nCS0 = 0;
	error_val = send_command(17, block_num);
	if(error_val == NO_ERRORS)
	{
		error_val = read_block(512, array_name);
		if(error_val == NO_ERRORS)
		{
			offset = 0;
			value8 = read8(offset, array_name);
			if(value8 != JUMP_INST1 && value8 != JUMP_INST2)
			{
				offset = RELSEC_OFFSET;
				block_num = read32(offset, array_name);
				RelSec = block_num;
				error_val = send_command(17, block_num * 512);
				if(error_val == NO_ERRORS)
				{
					error_val = read_block(512, array_name);
					if(error_val == NO_ERRORS)
					{
						offset = 0;
						value8 = read8(offset, array_name);
						if(value8 != JUMP_INST1 && value8 != JUMP_INST2)
						{
							error_val = DRIVE_ERROR;
						}
					}
				}
			}
			if(error_val == NO_ERRORS)
			{
				// Read values
				BytesPerSec_g = read16(0x0B, array_name);
				SecPerClus_g = read8(0x0D, array_name);
				RsvdSecCnt = read16(0x0E, array_name);
				NumFATs = read8(0x10, array_name);
				RootEntCnt = read16(0x11, array_name);
				TotSec16 = read16(0x13, array_name);
				TotSec32 = read32(0x20, array_name);
				FATSz16 = read16(0x16, array_name);
				FATSz32 = read32(0x24, array_name);
				RootClus = read32(0x2C, array_name);

				// Calculate RootDirSectors
				RootDirSecs_g = ((RootEntCnt * 32) + (BytesPerSec_g - 1)) / BytesPerSec_g;

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

				// Calculate CountofClus
				CountofClus = DataSec / SecPerClus_g;

				// Determine FAT type
				if(CountofClus < 65525)
				{
					FATtype_g = FAT16;
					error_val = FAT16_ERROR; // FAT16 unsupported
				}
				else
				{
					FATtype_g = FAT32;
				}

				StartofFAT_g = RsvdSecCnt + RelSec;
				FirstDataSec_g = RsvdSecCnt + (NumFATs * FATSz) + RootDirSecs_g + RelSec;
				FirstRootDirSec_g = RsvdSecCnt + (NumFATs * FATSz) + RelSec;
			}
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
