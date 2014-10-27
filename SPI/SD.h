//public #defines:
#define UNKNOWN_TYPE (0)
#define SD (1)
#define SDHC(2)

//SD Card public variables:
uint8 SD_Card_type = UNKNOWN_TYPE;


//SD Card public functions:
uint8 SD_Card_init();
uint8 read_block(uint16 number_of_bytes, uint8* array);