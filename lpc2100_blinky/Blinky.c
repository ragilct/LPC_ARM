/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  BLINKY.C:  LED Flasher                                                    */
/*                                                                            */
/******************************************************************************/
                  
#include <LPC21xx.H>                       /* LPC21xx definitions */

extern void init_serial (void);            /* Initialize Serial Interface     */
extern int  putchar (int ch);              /* Write character to Serial Port  */
extern int  getchar (void);                /* Read character from Serial Port */


void puthex (int hex) {                    /* Write Hex Digit to Serial Port  */
  if (hex > 9) putchar('A' + (hex - 10));
  else         putchar('0' +  hex);
}

void putstr (char *p) {                    /* Write string */
  while (*p) {
    putchar (*p++);
  }
}


void delay (void) {                        /* Delay function */
  unsigned int cnt;
  unsigned int val;

  ADCR |= 0x01000000;                      /* Start A/D Conversion */
  do {
    val = ADDR;                            /* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);       /* Wait for end of A/D Conversion */
  ADCR &= ~0x01000000;                     /* Stop A/D Conversion */
  val = (val >> 6) & 0x03FF;               /* Extract AIN0 Value */

  putstr ("\nAIN0 Result = 0x");           /* Output A/D Conversion Result */
  puthex((val >> 8) & 0x0F);               /* Write 1. Hex Digit */
  puthex((val >> 4) & 0x0F);               /* Write 2. Hex Digit */
  puthex (val & 0x0F);                     /* Write 3. Hex Digit */

  val = (val >> 2) << 12;                  /* Adjust Delay Value */
  for (cnt = 0; cnt < val; cnt++);         /* Delay */
}


int main (void) {
  unsigned int n;

  IODIR1 = 0x00FF0000;                     /* P1.16..23 defined as Outputs  */
  ADCR   = 0x002E0401;                     /* Setup A/D: 10-bit AIN0 @ 3MHz */
  init_serial();                           /* Initialize Serial Interface   */

  while (1) {                              /* Loop forever */
    for (n = 0x00010000; n <= 0x00800000; n <<= 1) {
      /* Blink LED 0, 1, 2, 3, 4, 5, 6, 7 */
      IOSET1 = n;                          /* Turn on LED */
      delay();                             /* Delay */
      IOCLR1 = 0x00FF0000;                 /* Turn off LEDs */
    }
  }
}
