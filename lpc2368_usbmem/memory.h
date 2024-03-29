/*----------------------------------------------------------------------------
 *      Name:    MEMORY.H
 *      Purpose: USB Memory Storage Demo Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#define CCLK            60000000    /* CPU Clock */

/* LED Definitions */
#define LED_MSK         0x000000FF  /* P2.0..7 */
#define LED_RD          0x00000001  /* P2.0 */
#define LED_WR          0x00000002  /* P2.1 */
#define LED_CFG         0x00000010  /* P2.4 */
#define LED_SUSP        0x00000020  /* P2.5 */

/* MSC Disk Image Definitions */
#define MSC_ImageSize   0x00001000

extern const unsigned char DiskImage[MSC_ImageSize];   /* Disk Image */
