/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#include "system.h"
#include "wl_module.h"

Long_Payload long_payload = 0;
Short_Payload short_payload = 0;

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    /* TODO Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Uart setup */
    /*SPBRGH = 0;
    SPBRGL = (((SYS_FREQ / THE_BAUD_RATE) / 64) - 1);
    BRGH = 0;
    BRG16 = 0;
    SYNC = 0;
    SPEN = 1;
    TXEN = 1;*/

    // disable adc
    ANSELC = 0;

    /* Setup nRF24L01 + spi */
    wl_module_init();

    /* Enable interrupts */
    INTCONbits.PEIE = 1; // peripheral interrupts enabled
    INTCONbits.GIE = 1; // global interrupt enable

    /* Low power setup */
    WDTCONbits.SWDTEN = 0; // WDT off for now

}

void putch(unsigned char byte) {
    while (!TRMT);
    TXREG = byte;
}

