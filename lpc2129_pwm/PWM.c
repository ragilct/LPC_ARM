/******************************************************************************/
/*  This file is part of the uVision/ARM development tools                    */
/*  Copyright KEIL ELEKTRONIK GmbH 2002-2004                                  */
/******************************************************************************/
/*                                                                            */
/*  PWM.C:  LED Flasher                                                    */
/*                                                                            */
/******************************************************************************/
                  
#include <LPC21xx.H>                       /* LPC21xx definitions  */

void PWM0_isr(void)  __irq
{
  PWMIR       |= 0x00000001;               /* Clear match0 interrupt */
  VICVectAddr  = 0x00000000;
}


void poll_ADC (void) {                     /* Get ADC Value and set PWM */
  unsigned int val; 
  static unsigned int oldval;

  ADCR |= 0x01000000;                      /* Start A/D Conversion */
  do {
    val = ADDR;                            /* Read A/D Data Register */
  } while ((val & 0x80000000) == 0);       /* Wait for end of A/D Conversion */
  ADCR &= ~0x01000000;                     /* Stop A/D Conversion */
  val = (val >> 6) & 0x03FF;               /* Extract AIN0 Value */

  if ((val != oldval)) { 
    PWMMR2 = val;                               
    PWMLER = 0x4;                          /* Enable Shadow latch */
    oldval = val; 
  }
}


void init_PWM (void) {
  VICVectAddr8 = (unsigned)PWM0_isr;        /* Set the PWM ISR vector address */
  VICVectCntl8 = 0x00000028;                /* Set channel */
  VICIntEnable = 0x00000100;                /* Enable the interrupt */

  PINSEL0 |= 0x00028008;                    /* Enable P0.7 and P0.1 as PWM output */
  PWMPR    = 0x00000000;                    /* Load prescaler  */
  
  PWMPCR = 0x00000C0C;                      /* PWM channel 2 & 3 double edge control, output enabled */
  PWMMCR = 0x00000003;                      /* On match with timer reset the counter */
  PWMMR0 = 0x400;                           /* set cycle rate to sixteen ticks       */
  PWMMR1 = 0;                               /* set rising  edge of PWM2 to 100 ticks    */
  PWMMR2 = 0x200;                           /* set falling edge of PWM2 to 200 ticks   */
  PWMMR3 = 0x400;                           /* set rising  edge of PWM3 to 100 ticks    */
  PWMLER = 0xF;                             /* enable shadow latch for match 1 - 3   */ 
  PWMTCR = 0x00000002;                      /* Reset counter and prescaler           */ 
  PWMTCR = 0x00000009;                      /* enable counter and PWM, release counter from reset */
}


int main (void) {

  IODIR1 = 0x00FF0000;                      /* P1.16..23 defined as Outputs  */
  ADCR   = 0x01200401;                      /* Setup A/D: 10-bit AIN0 @ 3MHz */
  init_PWM();

  while (1) {                               /* Loop forever */
    poll_ADC();     
  }
}
