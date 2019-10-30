/*------------------------------------------------------------------------------
SIO.C:  Serial Communication Routines for Philips LPC2100.

Copyright 2004-2006 KEIL Software, Inc.
------------------------------------------------------------------------------*/
#include <LPC21xx.H>
#include <string.h>
#include <limits.h>
#include "sio.h"

/*------------------------------------------------------------------------------
Notes:

The length of the receive and transmit buffers must be a power of 2.

Each buffer has a next_in and a next_out index.

If next_in = next_out, the buffer is empty.

(next_in - next_out) % buffer_size = the number of characters in the buffer.
------------------------------------------------------------------------------*/
#define TBUF_SIZE   256	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   256      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
struct buf_st {
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  char buf [RBUF_SIZE];     /* Buffer */
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

static struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((unsigned short)(tbuf.in - tbuf.out))

static unsigned int tx_restart = 1;   /* NZ if TX restart is required */

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void sio_irq (void) __irq {
  volatile char dummy;
  volatile char IIR;
  struct buf_st *p;

  /*------------------------------------------------
  Repeat while there is at least one interrupt source.
  ------------------------------------------------*/
  while (((IIR = U1IIR) & 0x01) == 0) {
     switch (IIR & 0x0E) {
       case 0x06: /* Receive Line Status */
         dummy = U1LSR;  /* Just clear the interrupt source */
         break;

       case 0x04: /* Receive Data Available */
       case 0x0C: /* Character Time-Out */
         p = &rbuf;

         if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
           p->buf [p->in & (RBUF_SIZE-1)] = U1RBR;
           p->in++;
         }
         break;

       case 0x02: /* THRE Interrupt */
         p = &tbuf;

         if (p->in != p->out) {
           U1THR = p->buf [p->out & (TBUF_SIZE-1)];
           p->out++;
           tx_restart = 0;
         }
         else {
           tx_restart = 1;
         }
         break;

       case 0x00: /* Modem Interrupt */
         dummy = U1MSR;  /* Just clear the interrupt source */
         break;

       default:
         break;
     }
  }

  VICVectAddr = 0; /* Acknowledge Interrupt */
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void com_initialize (void) {
  volatile char dummy;

  /*------------------------------------------------
  Clear com buffer indexes.
  ------------------------------------------------*/
  tbuf.in = 0;
  tbuf.out = 0;
  tx_restart = 1;

  rbuf.in = 0;
  rbuf.out = 0;

  /*------------------------------------------------
  Setup serial port registers.
  ------------------------------------------------*/
  PINSEL0 = 0x00050000;    /* Enable RxD1 and TxD1 */

  U1LCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
  U1IER = 0;               /* Disable UART1 Interrupts */

  VICVectAddr0 = (unsigned long)sio_irq;
  VICVectCntl0 = 0x20 | 7; /* UART1 Interrupt */
  VICIntEnable = 1 << 7;   /* Enable UART1 Interrupt */

  com_baudrate (115200);   /* setup for 115200 baud (15MHz) */
  /*** Note that with the PLL and VPB setup the pclk is 15MHz. ***/
  /*** This does NOT generate nice baudrates!!! ***/
  /*** Most connection problems are baudrate mismatches. ***/
  /*** Symptoms we've seen include reception on PC but no ***/
  /*** reception on the target system. ***/

  dummy = U1IIR;   /* Read IrqID - Required to Get Interrupts Started */
  U1IER = 3;       /* Enable UART1 RX and THRE Interrupts */
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void com_baudrate (unsigned int baudrate) {
  unsigned long DLreload;

  DLreload = (15000000UL / baudrate) / 16UL;

  U1LCR |= 0x80;           /* Set DLAB */
  U1DLL = DLreload;
  U1DLM = (DLreload >> 8);
  U1LCR &= ~0x80;          /* Clear DLAB */
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int __swi(8) com_putchar (int c);
int __SWI_8              (int c) {
  struct buf_st *p = &tbuf;

  /*------------------------------------------------
  If the buffer is full, return an error value.
  ------------------------------------------------*/
  if (SIO_TBUFLEN >= TBUF_SIZE)
    return (-1);

  /*------------------------------------------------
  Add the data to the transmit buffer.  If the
  transmit interrupt is disabled, then enable it.
  ------------------------------------------------*/
  if (tx_restart)	{
    tx_restart = 0;
    U1THR = c;
  }
  else {
    p->buf [p->in & (TBUF_SIZE - 1)] = c;
    p->in++;
  }

  return (0);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int __swi(9) com_getchar (void);
int __SWI_9              (int c) {
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

