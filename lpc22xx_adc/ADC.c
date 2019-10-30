
#include <lpc22xx.h>					/* LPC2200 definitions             */
#include <stdio.h>                      /* standard I/O .h-file            */
#define VREF  3

unsigned int val;


extern void init_serial (void);  


int main(void) {

init_serial();                            /* Initialize Serial Interface */


/* Setup the A/D converter */
IODIR1 = 0x00FF0000;   				   /* P1.16..23 defined as Outputs       */
VPBDIV = 0x02;			               /*Set the Pclk to 30 Mhz              */
ADCR   = 0x00210601;                   /* Setup A/D: 10-bit AIN0 @ 3MHz      */


while(1) {

ADCR  |= 0x01000000;                   /* Start A/D Conversion               */
while ((ADDR & 0x80000000) == 0); 	   /*Wait for the conversion to complete */
val = ((ADDR >> 6) & 0x03FF);	       /*Extract the A/D result 	         */

  printf ("Digital Value %4u = %01u.%04u Volts\r",
         (unsigned) val,
         (unsigned) (val * VREF) >> 10,                          /* Output Integer Portion */
         (unsigned) ((val * VREF * 10000UL) >> 10UL) % 10000);   /* Output Decimal Portion */

  }

}
