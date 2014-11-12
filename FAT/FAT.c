#include "FAT.h"

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