/*------------------------------------------------------------------------------
MAIN.C:  Interrupt Driven SIO Using printf for Philips LPC2100.

Copyright 2004-2006 KEIL Software, Inc.
------------------------------------------------------------------------------*/
#include <LPC21xx.H>
#include <stdio.h>
#include "sio.h"

const char message [] =
  "This is a test to see if the interrupt driven serial I/O routines really work.";

int main (void) {
  /* General Purpose I/O Port 1 Setup ..............................*/
  IODIR1 = 0x00FF0000;          /* P1.16..23 defined as Outputs     */
  IOCLR1 = 0x00FF0000;          /* clear LEDs                       */

  com_initialize ();            /* init interrupt driven serial I/O */

  printf ("Interrupt-driven Serial I/O Example\r\n\r\n");

  while (1) {
    unsigned char c;

    printf ("Press a key. ");
    c = getchar ();
    printf ("\r\n");
    printf ("You pressed '%c'.\r\n\r\n", c);
  }
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
