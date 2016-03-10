/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        16000000L
#define _XTAL_FREQ SYS_FREQ
#define FCY             SYS_FREQ/4
#include <stdint.h>        /* For uint8_t definition */

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */

void sleep (const uint8_t interval);

#define INPUT 1
#define OUTPUT 0

//#define RF_PWR_PIN_DIR TRISC6
//#define RF_PWR_PIN LATC6
#define RF_PWR_PIN_DIR TRISA2
#define RF_PWR_PIN LATA2

#define LED_RED LATC2
#define LED_RED_DIR TRISC2

#define LED_GRN LATC5
#define LED_GRN_DIR TRISC5

//#define LDR_DATA_PIN RC3
#define LDR_DATA_DIR TRISC3

#define DHT_PIN_DIR TRISC4
#define DHT_PIN LATC4

#define SDA_TRIS  TRISC4
#define SCL_TRIS  TRISA5
#define SDA       LATC4
#define SDA_PIN   RC4
#define SCL       LATA5

enum {
    WDT_1_MS = 0b00000,
    WDT_2_MS = 0b00001,
    WDT_4_MS = 0b00010,
    WDT_8_MS = 0b00011,
    WDT_16_MS = 0b00100,
    WDT_32_MS = 0b00101,
    WDT_64_MS = 0b00110,
    WDT_128_MS = 0b00111,
    WDT_256_MS = 0b01000,
    WDT_512_MS = 0b01001,
    WDT_1_SEC = 0b01010,
    WDT_2_SEC = 0b01011,
    WDT_4_SEC = 0b01100,
    WDT_8_SEC = 0b01101,
    WDT_16_SEC = 0b01110,
    WDT_32_SEC = 0b01111,
    WDT_64_SEC = 0b10000,
    WDT_128_SEC = 0b10001,
    WDT_256_SEC = 0b10010,
}; // end of WDT intervals enum

