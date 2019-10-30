/*------------------------------------------------------------------------------
SIO.H:  Serial Communication Routines for Philips LPC2100.

Copyright 2004-2005 KEIL Software, Inc.
------------------------------------------------------------------------------*/
#ifndef LPC2100_SIO_H
#define LPC2100_SIO_H

void com_initialize (void);

void com_baudrate (
  unsigned int baudrate);

int com_putchar (
  int c) __swi(8);

int com_getchar (void) __swi(9);

#endif

