/*----------------------------------------------------------------------------
 *      Name:    DEMO.H
 *      Purpose: USB Audio Demo Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

/* Clock Definitions */
#define CPU_CLOCK 12000000              /* CPU Clock */
#define VPB_CLOCK (CPU_CLOCK/1)         /* VPB Clock */

/* Audio Definitions */
#define DATA_FREQ 32000                 /* Audio Data Frequency */
#define P_S       32                    /* Packet Size */
#if USB_DMA
#define P_C       4                     /* Packet Count */
#else
#define P_C       1                     /* Packet Count */
#endif
#define B_S       (8*P_C*P_S)           /* Buffer Size */

/* Push Button Definitions */
#define PBINT     0x00000400            /* P2.10 */

/* LED Definitions */
#define LEDMSK    0x000000FF            /* P2.0..7 */

/* Audio Demo Variables */
extern BYTE  Mute;                      /* Mute State */
extern DWORD Volume;                    /* Volume Level */
extern WORD  VolCur;                    /* Volume Current Value */
extern DWORD InfoBuf[P_C];              /* Packet Info Buffer */
extern short DataBuf[B_S];              /* Data Buffer */
extern WORD  DataOut;                   /* Data Out Index */
extern WORD  DataIn;                    /* Data In Index */
extern BYTE  DataRun;                   /* Data Stream Run State */
