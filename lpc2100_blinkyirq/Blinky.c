/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  BLINKY.C:  LED Flasher                                                    */
/*                                                                            */
/******************************************************************************/
                  
#include <LPC21xx.H>                        /* LPC21xx definitions */
#include "Timer.h"

extern long volatile timeval;

void wait (void)  {                         /* wait function */
  unsigned long i;

  i = timeval;
  while ((i + 10) != timeval);              /* wait 100ms */
}

int main (void) {
  unsigned int j;                           /* LED var */

  IODIR1 = 0xFF0000;                        /* P1.16..23 defined as Outputs */

  init_timer();

  while (1)  {                                  /* Loop forever */
    for (j = 0x010000; j < 0x800000; j <<= 1) { /* Blink LED 0,1,2,3,4,5,6 */
      IOSET1 = j;                               /* Turn on LED */
      wait ();                                  /* call wait function */
      IOCLR1 = j;                               /* Turn off LED */
    }
    for (j = 0x800000; j > 0x010000; j >>=1 ) { /* Blink LED 7,6,5,4,3,2,1 */
      IOSET1 = j;                               /* Turn on LED */
      wait ();                                  /* call wait function */
      IOCLR1 = j;                               /* Turn off LED */
    }
  }
}
