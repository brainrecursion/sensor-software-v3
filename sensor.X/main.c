/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include "wl_module.h"
#include "sht21.h"
#include "i2c.h"

#define NODE_ID 33

uint8_t measure_light() {
    uint16_t li;
    // light measure
    ADCON1bits.ADPREF = 0b00;
    ADCON1bits.ADFM = 0b1;
    ADCON1bits.ADCS = 0b101;
    TRISC3 = 1;
    ANSELCbits.ANSC7 = 0b1;
    ADCON0bits.CHS = 7;
    ADCON0bits.ADON = 1;
    ADCON0bits.GO_nDONE = 0b1; //start
    while (ADCON0bits.GO_nDONE);
    li = ((ADRESH << 8) | ADRESL);
    li = li / 11; // convert to % (approx)
    return li;
}

uint16_t measure_power() {
    /* setup */
    //VREFCON0bits.FVRS = 0b01;   // Internal 1.024v ref
    FVRCONbits.ADFVR = 0b01;
    FVRCONbits.FVREN = 1; // Enable FVR module
    while (!FVRCONbits.FVRRDY); // Wait for FVR to be stable
    __delay_us(50);

    ADCON1bits.ADFM = 0b1; // Right justify result
    ADCON0bits.CHS = 0b11111; // FVR is ADC input
    ADCON1bits.ADPREF = 0b00; // Positive ref is Vdd (default)
    //ADCON1bits.NVCFG = 0b00;    // Negative ref is GND (default)
    ADCON0bits.ADON = 1; // Turn on ADC module

    int adc_val = 0;

    ADCON0bits.GO = 1; // Start a conversion
    while (ADCON0bits.GO_nDONE); // Wait for it to be completed

    adc_val = (ADRESH << 8); // Store the result in adc_val
    adc_val |= ADRESL;

    int millivolts = (8192 / adc_val) * 1024;
    millivolts /= 8;

    /* disable to save power */
    FVRCONbits.FVREN = 0;
    FVRCONbits.ADFVR = 0b00;

    return millivolts;
}

void peripheral_pwr(bool pwr) {
    if (pwr) {
        i2c_init();

        RF_PWR_PIN = 0b1;

        LDR_DATA_DIR = INPUT;
    } else {
        RF_PWR_PIN = 0b0;
        i2c_uninit();
        LED_GRN = 0b0;
        LED_RED = 0b0;
    }
}

void getTH(void) {
    uint8_t error = 0;
    uint16_t raw;
    
    error = SHT2x_MeasurePoll(HUMIDITY, &raw);
    if (error) {
        short_payload.humidity = 255;
    } else {
        short_payload.humidity = (uint8_t) (SHT2x_CalcRH(raw) + 0.5);
    }
    
    error = SHT2x_MeasurePoll(TEMP, &raw);
    if (error) {
        short_payload.temp = 255;
    } else {
        short_payload.temp = SHT2x_CalcTemperatureC(raw);
    }
}

void main(void) {
    uint8_t err;
    short_payload.nodeId = NODE_ID;
    long_payload.nodeId = NODE_ID;

    /* set all pins to input to save power. */
    TRISC = 0xffffff;
    TRISA = 0xffffff;
    TRISB = 0xffffff;

    RF_PWR_PIN_DIR = 0;
    peripheral_pwr(false);

    LED_RED_DIR = 0;
    LED_GRN_DIR = 0;
    LED_RED = 0b0;
    LED_GRN = 0b0;

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* Check payload size */
    if ((long_payload_size != sizeof (Long_Payload)) || (short_payload_size != sizeof (Short_Payload))) {
        /* Display error is they don't match */
        while (1) {
            LED_RED = 0b1;
            LED_GRN = 0b1;
            sleep(WDT_256_MS);
            LED_RED = 0b0;
            LED_GRN = 0b0;
            sleep(WDT_256_MS);
        }
    }

    LED_RED = 0b1;
    LED_GRN = 0b1;
    sleep(WDT_256_MS);
    LED_RED = 0b0;
    LED_GRN = 0b0;

    /* Main loop */
    while (1) {
        /* Turn on peripheral power */
        peripheral_pwr(true);
        //wait for settling time
        sleep(WDT_32_MS);
        
        getTH();
        wl_module_tx_config(wl_module_TX_NR_0);
        //short_payload.light = measure_light();

        short_payload.count++;
        /* Send short payload */
        err = wl_module_send((unsigned char *) &short_payload, sizeof (Short_Payload));

        if (short_payload.count % 12 == 1) {
            /* Send long payload */
            long_payload.millivolts = measure_power();
            long_payload.count++;
            wl_module_send((unsigned char *) &long_payload, sizeof (Long_Payload));
        }
        /* Turn of peripheral power */
        peripheral_pwr(false);
        if (err){
            LED_RED = 0b1;
            sleep(WDT_1_MS);
            LED_RED = 0b0;
            sleep(WDT_1_SEC);
        }else{
            LED_GRN = 0b1;
            sleep(WDT_1_MS);
            LED_GRN = 0b0;
        }
        sleep(WDT_256_SEC);
        sleep(WDT_32_SEC);
    }
}


