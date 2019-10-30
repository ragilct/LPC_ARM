/*----------------------------------------------------------------------------
 *      Name:    DEMO.C
 *      Purpose: USB Audio Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include <LPC23XX.H>                        /* LPC23xx definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"

#include "demo.h"
#include "LCD.h"


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
    FIO2CLR = LEDMSK;                       /* Turn Off all LEDs */
    FIO2SET = LEDMSK >> (7 - val);          /* LEDs show VU Meter */
  }

  T0IR = 1;                                 /* Clear Interrupt Flag */
  VICVectAddr = 0;                          /* Acknowledge Interrupt */
}


/* Main Program */

int main (void) {

  PINSEL10 = 0;                             /* Disable ETM interface */
  PINSEL1 = 0x00204000;                     /* Select AOUT,AIN0 */
  FIO2DIR  = LEDMSK;                        /* LEDs, port 2, bit 0~7 output only */

  PCONP  |= (1 << 12);                      /* Enable power to AD block    */
  AD0CR   = 0x00200E01;                     /* ADC: 10-bit AIN0 @ 4MHz */
  DACR    = 0x00008000;                     /* DAC Output set to Middle Point */

  /* Setup Timer Counter 0: Periodic Timer with Interrupt at DATA_FREQ Rate */
  T0MR0 = VPB_CLOCK/DATA_FREQ - 1;          /* TC0 Match Value 0 */
  T0MCR = 3;                                /* TCO Interrupt and Reset on MR0 */
  T0TCR = 1;                                /* TC0 Enable */

  /* Setup Timer Counter 0 Interrupt */
  VICVectAddr4 = (unsigned long)tc0_isr;    /* TC0 Interrupt -> Vector 4   */
  VICVectCntl4 = 0x02;                      /* TC0 Interrupt -> Priority 2 */
  VICIntEnable = 1 << 4;                    /* Enable TC0 Interrupt */

  lcd_init();
  lcd_clear();
  lcd_print (" MCB2300  Audio ");
  set_cursor (0, 1);
  lcd_print ("  www.keil.com  ");

  USB_Init();                               /* USB Initialization */
  USB_Connect(TRUE);                        /* USB Connect */

  while (1);                                /* Loop forever */
}
