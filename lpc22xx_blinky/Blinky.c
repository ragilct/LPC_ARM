/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  BLINKY.C:  LED Flasher                                                    */
/*                                                                            */
/******************************************************************************/

#include <LPC22XX.H>                        /* LPC22XX Peripheral Registers */


unsigned long counter _at_ 0x81000000;      /* Counter Variable in ExtMem */


extern long volatile timeval;

void wait (void)  {                         /* wait function */
  unsigned long i;

  i = timeval;
  while ((timeval - i) != 10);              /* wait 100ms */
}


extern void init_timer (void);

int main (void) {

  init_timer();								/* Initialize Timer */

  IODIR0 = 0x00000100;                      /* P0.8 defined as Output */

  while (1) {                               /* Loop forever */
    IOCLR0 = 0x00000100;                    /* Turn LED On  (P0.8 = 0) */
	wait();                                 /* Wait */
    IOSET0 = 0x00000100;                    /* Turn LED Off (P0.8 = 1) */
	wait();                                 /* Wait */
    counter++;                              /* Increment Counter */
  }
}
