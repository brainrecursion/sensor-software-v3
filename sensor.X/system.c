/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void) {
    /* TODO Add clock switching code if appropriate.  */
    // 16MHz internal osc
    OSCCON = 0b1111010;
    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
}

void sleep(const uint8_t interval) {
    WDTCONbits.WDTPS = interval;
    WDTCONbits.SWDTEN = 1;
    SLEEP();
    WDTCONbits.SWDTEN = 0;
}