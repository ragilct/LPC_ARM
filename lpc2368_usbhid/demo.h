/*----------------------------------------------------------------------------
 *      Name:    DEMO.H
 *      Purpose: USB HID Demo Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

/* Push Button Definitions */
#define PBINT  0x00000400  /* P2.10 */

/* LED Definitions */
#define LEDMSK 0x000000FF /* P2.0..7 */

/* HID Demo Variables */
extern BYTE InReport;
extern BYTE OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetOutReport (void);
