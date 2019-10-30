/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*                                                                            */
/******************************************************************************/

#include <LPC21xx.H>                     /* LPC22xx definitions               */

#define CR     0x0D


void init_serial (void)  {               /* Initialize Serial Interface       */
  PINSEL0 = 0x00050000;                  /* Enable RxD1 and TxD1              */
  U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
  U1DLL = 97;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U1LCR = 0x03;                          /* DLAB = 0                          */
}


int putchar (int ch)  {                  /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(U1LSR & 0x20));
    U1THR = CR;                          /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}


int getchar (void)  {                    /* Read character from Serial Port   */

  while (!(U1LSR & 0x01));

  return (U1RBR);
}