/*
 * IAP In-System Application Programming Demo
 */

#include <LPC21xx.H>

// Clock Frequency

#define XTAL   12000000                      // Oscillator Frequency

#ifdef BYPASS_IAP
#define CPUCLK  XTAL                         // CPU Clock without PLL
#else
#define CPUCLK  (XTAL*4)                     // CPU Clock with PLL
#endif

#define CCLK   (XTAL / 1000)                 // CPU Clock without PLL in kHz


// Phase Locked Loop (PLL) definitions
#define        PLL_BASE        0xE01FC080  // PLL Base Address 
#define        PLLCON_OFS      0x00        // PLL Control Offset
#define        PLLSTAT_OFS     0x08        // PLL Status Offset 
#define        PLLFEED_OFS     0x0C        // PLL Feed Offset 
#define        PLLCON_PLLE     0x01		   // PLL Enable 
#define 	   PLLCON_PLLD	   0x00		   // PLL Disable
#define        PLLCON_PLLC     0x03		   // PLL Connect(0x02) | PLL Enable
#define        PLLSTAT_PLOCK   0x0400	   //1<<10 // PLL Lock Status


struct iap_in {
  unsigned int cmd;
  unsigned int par[4];  
};

typedef void (*IAP)(struct iap_in *in, unsigned int *result);
#define iap_entry ((IAP) 0x7FFFFFF1)            // IAP entry point


/* Default Interrupt Function: may be called when interrupts are disabled */
void def_isr (void) __irq  {
 ;
}


#ifdef BYPASS_IAP
/*
 * Switch CPU to PLL clock
 */
void start_pll (void)  {
  __asm  {
        LDR     R0, =PLL_BASE
        MOV     R1, #0xAA
        MOV     R2, #0x55

        // Enable PLL
        MOV     R3, #PLLCON_PLLE
        STR     R3, [R0, #PLLCON_OFS]
        STR     R1, [R0, #PLLFEED_OFS]
        STR     R2, [R0, #PLLFEED_OFS]

        // Wait until PLL Locked
        LDR     R2, =PLLSTAT_PLOCK 
    PLL_Loop:
        LDR     R3, [R0, #PLLSTAT_OFS]
        CMP     R3, R2
        BEQ     PLL_Loop

        // Switch to PLL Clock
        MOV     R2, #0x55
        MOV     R3, #PLLCON_PLLC
        STR     R3, [R0, #PLLCON_OFS]
        STR     R1, [R0, #PLLFEED_OFS]
        STR     R2, [R0, #PLLFEED_OFS]
  }
}


/*
 * Switch CPU to standard XTAL
 */
void stop_pll(void) __arm  {
  __asm  {
        LDR     R0, =PLL_BASE
        MOV     R1, #0xAA
        MOV     R2, #0x55

        // Disable PLL
        MOV     R3, #PLLCON_PLLD
        STR     R3, [R0, #PLLCON_OFS]
        STR     R1, [R0, #PLLFEED_OFS]
        STR     R2, [R0, #PLLFEED_OFS]
  }
}

#endif

/*
 * Convert 'addr' to sector number
 */
unsigned int get_secnum (void *addr)  {
  unsigned int n;

  n = ((unsigned int) addr >> 13) & 0x1F;        // pseudo sector number

  if (n >= (0x30000 >> 13))  {
    n -= 14;                                    // high small 8kB Sectors (
  }
  else if (n >= (0x10000 >> 13))  {
    n  = 7 + (n >> 3);                          // large 64kB Sectors
  }
  return (n);                                   // sector number
}


/*
 * Erase Sector between 'start' and 'end'
 * Return:  IAP error code (0 when OK)
 * NOTES:  start needs to be a 256 byte boundary
 *         size should be 256, 512, 1024 or 4089
 */
unsigned int erase (void* start, void* end)  {
  struct iap_in  iap;                      // IAP input parameters
  unsigned int result[16];                 // IAP results
  unsigned int save_VicInt;                // for saving of interrupt enable register

  save_VicInt = VICIntEnable;              // save interrupt enable status
  VICIntEnClr = 0xFFFFFFFF;                // disable all interrupts

#ifdef BYPASS_IAP
  stop_pll();                              // IAP requires to run without PLL
#endif

  iap.cmd = 50;                            // IAP Command: Prepare Sectors for Write
  iap.par[0] = get_secnum (start);         // start sector
  iap.par[1] = get_secnum (end);           // end sector
  iap_entry (&iap, result);                // call IAP function
  if (result[0])  goto exit;               // an error occured?

  iap.cmd = 52;                            // IAP command: Erase Flash
  iap.par[0] = get_secnum (start);         // start sector
  iap.par[1] = get_secnum (end);           // end sector
  iap.par[2] = CCLK;                       // CPU clock
  iap_entry (&iap, result);                // call IAP function

exit:

#ifdef BYPASS_IAP
  start_pll();                             // start PLL
#endif

  VICIntEnable = save_VicInt;              // enable interrupts
  return (result[0]);
}


/*
 * Program *data to flash_addr. number of bytes specified by size
 * Return:  IAP error code (0 when OK)
 * Note: 
 */
unsigned int program (void *flash_addr, void *data, unsigned int size)  {
  struct iap_in  iap;                      // IAP input parameters
  unsigned int result[16];                 // IAP results
  unsigned int save_VicInt;                // for saving of interrupt enable register

  save_VicInt = VICIntEnable;              // save interrupt enable status
  VICIntEnClr = 0xFFFFFFFF;                // disable all interrupts

#ifdef BYPASS_IAP
  stop_pll();                              // IAP requires to run without PLL
#endif

  iap.cmd = 50;                            // IAP Command: Prepare Sectors for Write
  iap.par[0] = get_secnum (flash_addr);    // start sector
  iap.par[1] = iap.par[0];                 // end Sektor
  iap_entry (&iap, result);                // call IAP function
  if (result[0])  goto exit;               // an error occured?

  iap.cmd = 51;                            // IAP Command: Copy RAM to Flash
  iap.par[0] = (unsigned int) flash_addr;  // destination-addr
  iap.par[1] = (unsigned int) data;        // source-addr
  iap.par[2] = size;                       // number of bytes
  iap.par[3] = CCLK;                       // CPU clock
  iap_entry (&iap, result);                // call IAP function

exit:

#ifdef BYPASS_IAP
  start_pll();                             // start PLL
#endif

  VICIntEnable = save_VicInt;              // enable interrupts
  return (result[0]);
}



unsigned char vals[512];


void main (void)  {
  unsigned int i;

  unsigned int volatile start;

  for (start = 0; start < 1000000; start++) {
    ;    // wait for debugger connection (about 0.3 sec)
  }

  VICDefVectAddr = (unsigned int) def_isr;       // for spurious interrupts

  for (i = 0; i < sizeof (vals); i++)  {
    vals[i] = (unsigned char) i;
  }

  program (0x30000, vals, sizeof (vals));
  program (0x31000, vals, sizeof (vals));
  program (0x32000, vals, sizeof (vals));
  erase   (0x30000, 0x31FFF);
  erase   (0x32000, 0x33FFF);

  while (1);
}


