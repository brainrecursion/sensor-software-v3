/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "wl_module.h"
#include "system.h"
#include <stdio.h>
//#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's 
 * are baseline devices.  Unfortunately the baseline detection macro is 
 * _PIC12 */
#ifndef _PIC12

#define LED_RED LATC2
#define LED_RED_DIR TRISC2
#define LED_GRN LATC5
#define LED_GRN_DIR TRISC5

void interrupt isr(void) {
    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */
    // external interrupt IRQ pin of NRF24L01

    /*if (INTCONbits.INTF) {
        LED_RED = 0b1;
        //LED_GRN = 0b1;
        unsigned char status;
        // Read wl_module status
        wl_module_CSN_lo; // Pull down chip select
        status = spi_fast_shift(NOOP); // Read status register
        wl_module_CSN_hi; // Pull up chip select

        if (status & (1 << TX_DS)) { // IRQ: Package has been sent
            wl_module_config_register(STATUS, (1 << TX_DS)); //Clear Interrupt Bit
            PTX = 0;
        }

        if (status & (1 << MAX_RT)) { // IRQ: Package has not been sent, send again
            wl_module_config_register(STATUS, (1 << MAX_RT)); // Clear Interrupt Bit

            NUM_ERRORS++;
            LED_RED = 0b1;
            // spi_fast_shift( REUSE_TX_PL ); // Write cmd to write payload
            PTX = 0; // give up
            //wl_module_CE_hi; // Start transmission
            //__delay_us(10);
            //wl_module_CE_lo;
        }

        if (status & (1 << TX_FULL)) { //TX_FIFO Full <-- this is not an IRQ
            wl_module_CSN_lo; // Pull down chip select
            spi_fast_shift(FLUSH_TX); // Flush TX-FIFO
            wl_module_CSN_hi; // Pull up chip select
        }
        // reset INT2 flag
        INTCONbits.INTF = 0;
    }
    else {
        // Unhandled interrupts
    }*/
}
#endif


