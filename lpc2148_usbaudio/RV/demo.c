/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB Audio Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on Philips LPC2xxx microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#include <LPC214X.H>                        /* LPC214x definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"

#include "demo.h"


BYTE  Mute;                                 /* Mute State */
DWORD Volume;                               /* Volume Level */

#if USB_DMA
#pragma arm section zidata = "USB_RAM"
DWORD InfoBuf[P_C];                         /* Packet Info Buffer */
short DataBuf[B_S];                         /* Data Buffer */
#pragma arm section zidata
#else
short DataBuf[B_S];                         /* Data Buffer */
#endif

WORD  DataOut;                              /* Data Out Index */
WORD  DataIn;                               /* Data In Index */

BYTE  DataRun;                              /* Data Stream Run State */

WORD  PotVal;                               /* Potenciometer Value */

DWORD VUM;                                  /* VU Meter */

DWORD Tick;                                 /* Time Tick */


/*
 * Get Potenciometer Value
 */

void get_potval (void) {
  DWORD val;

  AD0CR |= 0x01000000;                      /* Start A/D Conversion */
  do {
    val  = AD0GDR;                          /* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);        /* Wait for end of A/D Conversion */
  AD0CR &= ~0x01000000;                     /* Stop A/D Conversion */
  PotVal = ((val >> 8) & 0xF8) +            /* Extract Potenciometer Value */
           ((val >> 7) & 0x08);
}


/*
 * Timer Counter 0 Interrupt Service Routine
 *   executed each 31.25us (32kHz frequency)
 */

void tc0_isr (void) __irq {
  long  val;
  DWORD cnt;

  if (DataRun) {                            /* Data Stream is running */
    val = DataBuf[DataOut];                 /* Get Audio Sample */
    cnt = (DataIn - DataOut) & (B_S - 1);   /* Buffer Data Count */
    if (cnt == (B_S - P_C*P_S)) {           /* Too much Data in Buffer */
      DataOut++;                            /* Skip one Sample */
    }
    if (cnt > (P_C*P_S)) {                  /* Still enough Data in Buffer */
      DataOut++;                            /* Update Data Out Index */
    }
    DataOut &= B_S - 1;                     /* Adjust Buffer Out Index */
    if (val < 0) VUM -= val;                /* Accumulate Neg Value */
    else         VUM += val;                /* Accumulate Pos Value */
    val  *= Volume;                         /* Apply Volume Level */
    val >>= 16;                             /* Adjust Value */
    val  += 0x8000;                         /* Add Bias */
    val  &= 0xFFFF;                         /* Mask Value */
  } else {
    val = 0x8000;                           /* DAC Middle Point */
  }

  if (Mute) {
    val = 0x8000;                           /* DAC Middle Point */
  }

  DACR = val & 0xFFC0;                      /* Set Speaker Output */

  if ((Tick++ & 0x03FF) == 0) {             /* On every 1024th Tick */
    get_potval();                           /* Get Potenciometer Value */
    if (VolCur == 0x8000) {                 /* Check for Minimum Level */
      Volume = 0;                           /* No Sound */
    } else {
      Volume = VolCur * PotVal;             /* Chained Volume Level */
    }
    val = VUM >> 20;                        /* Scale Accumulated Value */
    VUM = 0;                                /* Clear VUM */
    if (val > 7) val = 7;                   /* Limit Value */
    IOCLR1 = LEDMSK;                        /* Turn Off all LEDs */
    IOSET1 = LEDMSK >> (7 - val);           /* LEDs show VU Meter */
  }

  T0IR = 1;                                 /* Clear Interrupt Flag */
  VICVectAddr = 0;                          /* Acknowledge Interrupt */
}


/* Main Program */

int main (void) {

  PINSEL1 = 0x01080000;                     /* Select AOUT,AIN1 */
  IODIR1  = LEDMSK;                         /* LED's defined as Outputs */

  AD0CR   = 0x00200E02;                     /* ADC: 10-bit AIN1 @ 4MHz */
  DACR    = 0x00008000;                     /* DAC Output set to Middle Point */

  /* Setup Timer Counter 0: Periodic Timer with Interrupt at DATA_FREQ Rate */
  T0MR0 = VPB_CLOCK/DATA_FREQ - 1;          /* TC0 Match Value 0 */
  T0MCR = 3;                                /* TCO Interrupt and Reset on MR0 */
  T0TCR = 1;                                /* TC0 Enable */

  /* Setup Timer Counter 0 Interrupt */
  VICVectAddr1 = (unsigned long)tc0_isr;    /* TC0 Interrupt -> Vector 1 */
  VICVectCntl1 = 0x20 | 4;                  /* TC0 Interrupt -> IRQ Slot 1 */
  VICIntEnable = 1 << 4;                    /* Enable TC0 Interrupt */

  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1);                                /* Loop forever */
}
