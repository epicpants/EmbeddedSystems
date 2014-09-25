#include "memory_test.h"

static unsigned char code code_mem[]="This is a test of a string in code memory";
static unsigned char xdata xdata_mem[50];

/***********************************************************************
DESC:  Returns the pointer to a string in code memory
INPUT: global string definition
RETURNS: pointer to the global string definition
CAUTION: 
************************************************************************/
unsigned char * code_memory_init(void)
{
   return code_mem;
}


/***********************************************************************
DESC:  Copies a string of characters from code memory to xram
INPUT: uses two global pointers defined above
RETURNS: pointer to the string in xdata memory
CAUTION: 
************************************************************************/
unsigned char * xdata_memory_init(void)
{
   unsigned char index, value;
   
   
   for(index=0;index<30;index++)
   {
       value=code_mem[index];
       xdata_mem[index]=value;
   }
   xdata_mem[30]='x';
   xdata_mem[31]='d';
   xdata_mem[32]='a';
   xdata_mem[33]='t';
   xdata_mem[34]='a'; 
   for(index=34;index<41;index++)
   {
       value=code_mem[index];
       xdata_mem[index+1]=value;
   }

   return xdata_mem;
}
