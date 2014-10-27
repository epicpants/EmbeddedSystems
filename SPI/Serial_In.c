#include <stdio.h>
#include "main.h"
#include "UART.h"
#include <stdlib.h>


/***********************************************************************
DESC:    Waits for user input of a long integer as ASCII characters
        
INPUT: Nothing
RETURNS: unsigned long
CAUTION: UART must be initialized first
************************************************************************/

uint32 long_serial_input(void)
{
   uint8 i,c,valsout[16];   
   uint32 output_val;
      for(i=0;i<16;i++)
      {
        valsout[i]=0;
      }
      output_val=0xffffffff;
      i=0;
      do
      {
         c=getchar();
         if((c>0x2f)&&(c<0x3a))
         {
           valsout[i]=c;
           i++;
         }
         else if ((c==BS)||(c==DEL))  // Putty uses DEL (127) or BS (0x08) for backspace
         {
           i--;
           valsout[i]=0;
           putchar(space);
           putchar(BS);
         }
         else if (c==CR||c==LF)
         {
            output_val=atol(valsout);
            putchar(CR);
			putchar(LF);
         }
         else
         {
           putchar(BS);
           putchar(space);
           putchar(BS);
         }
      }while(output_val==0xffffffff);

return output_val;
}
