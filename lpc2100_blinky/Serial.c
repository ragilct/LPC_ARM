/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  SERIAL.C:  Low Level Serial Routines                                      */
/*                                                                            */
/******************************************************************************/

#include <LPC21xx.H>                     /* LPC21xx definitions               */

#define CR     0x0D


void init_serial (void)  {               /* Initialize Serial Interface       */
  PINSEL0 = 0x00050000;                  /* Enable RxD1 and TxD1              */ 
  UART1_LCR = 0x83;                      /* 8 bits, no Parity, 1 Stop bit     */
  UART1_DLL = 97;                        /* 9600 Baud Rate @ 15MHz VPB Clock  */
  UART1_LCR = 0x03;                      /* DLAB = 0                          */
}


int putchar (int ch)  {                  /* Write character to Serial Port    */

  if (ch == '\n')  {
    while (!(UART1_LSR & 0x20));
    UART1_THR = CR;                      /* output CR */
  }
  while (!(UART1_LSR & 0x20));
  return (UART1_THR = ch);
}


int getchar (void)  {                    /* Read character from Serial Port   */

  while (!(UART1_LSR & 0x01));

  return (UART1_RBR);
}
