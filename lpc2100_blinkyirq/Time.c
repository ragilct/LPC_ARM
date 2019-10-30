/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  TIME.C:  Time Functions for 100Hz Clock Tick                              */
/*                                                                            */
/******************************************************************************/

#include <LPC21XX.H>                            // LPC21XX Peripheral Registers
#include "Timer.h"

long timeval;

void tc0 (void) __attribute__ ((interrupt));    // Generate Interrupt 

/* Setup the Timer Counter 0 Interrupt */
void init_timer (void) {
    TIMER0_MR0 = 149999;                        // 10mSec = 150.000-1 counts
    TIMER0_MCR = 3;                             // Interrupt and Reset on MR0
    TIMER0_TCR = 1;                             // Timer0 Enable
    VICVectAddr0 = (unsigned long)tc0;          // set interrupt vector in 0
    VICVectCntl0 = 0x20 | 4;                    // use it for Timer 0 Interrupt
    VICIntEnable = 0x00000010;                  // Enable Timer0 Interrupt
}


/* Timer Counter 0 Interrupt executes each 10ms @ 60 MHz CPU Clock */
void tc0 (void) {
    timeval++;
    TIMER0_IR = 1;                              // Clear interrupt flag
    VICVectAddr = 0;                            // Acknowledge Interrupt
}
