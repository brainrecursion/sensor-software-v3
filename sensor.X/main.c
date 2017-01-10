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
#include <string.h> /* memset */
#include "dht22_new.h"
#include "wl_module.h"

#define NODE_ID 9
#define SW_VERSION 2

uint16_t measure_light() {
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
    FVRCONbits.ADFVR = 0b01;
    FVRCONbits.FVREN = 1; // Enable FVR module
    while (!FVRCONbits.FVRRDY); // Wait for FVR to be stable
    __delay_us(50);

    ADCON1bits.ADFM = 0b1; // Right justify result
    ADCON0bits.CHS = 0b11111; // FVR is ADC input
    ADCON1bits.ADPREF = 0b00; // Positive ref is Vdd (default)
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
        DHT_PIN_DIR = INPUT;

        RF_PWR_PIN = 1;

        LDR_DATA_DIR = INPUT;
    } else {
        DHT_PIN_DIR = INPUT;
        RF_PWR_PIN = 0;
    }
}

void main(void) {
    int err;
    float humidity;
    Status_Payload status_payload;
    TEMP_HUM_LIGHT_Payload temp_hum_light_payload;
    memset(&temp_hum_light_payload,0,sizeof(TEMP_HUM_LIGHT_Payload));    
    temp_hum_light_payload.header.msg_id = TEMP_HUM_LIGHT_DATA;
    temp_hum_light_payload.header.nodeId = NODE_ID;
    
    memset(&status_payload,0,sizeof(Status_Payload)); 
    status_payload.header.msg_id = STATUS_DATA;
    status_payload.header.nodeId = NODE_ID;
    status_payload.sw_version = SW_VERSION;

    /* set all pins to input to save power. */
    TRISC = 0xffffff;
    TRISA = 0xffffff;
    TRISB = 0xffffff;

    RF_PWR_PIN_DIR = 0;
    peripheral_pwr(false);

    // never works first time, so dummy call...
    measure_power();
    
    LED_RED_DIR = 0;
    LED_GRN_DIR = 0;
    LED_RED = 0b0;
    LED_GRN = 0b0;

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

    /* Check payload size */
    if ((status_payload_size != sizeof (Status_Payload))||(temp_hum_light_payload_size != sizeof (TEMP_HUM_LIGHT_Payload))) {
        while (1) {
            LED_RED = 0b1;
            LED_GRN = 0b1;
            sleep(WDT_256_MS);
            LED_RED = 0b0;
            LED_GRN = 0b0;
            sleep(WDT_256_MS);
        }
    }

    sleep(WDT_1_SEC);
    
    LED_RED = 0b1;
    LED_GRN = 0b1;
    sleep(WDT_256_MS);
    LED_RED = 0b0;
    LED_GRN = 0b0;

    /* Main loop */
    while (1) {        
        peripheral_pwr(true);
        //wait for settling time
        sleep(WDT_2_SEC);

        temp_hum_light_payload.light = measure_light();

        sleep(WDT_128_MS);
        if (err = get_temp_hum(&temp_hum_light_payload.temperature, &humidity)) {
            status_payload.sensor_err++;
            temp_hum_light_payload.humidity = 255;
            temp_hum_light_payload.temperature = 255;
        }else{
            temp_hum_light_payload.humidity = (uint8_t)(humidity+0.5);
        }
        
        wl_module_tx_config(wl_module_TX_NR_0);

        temp_hum_light_payload.header.count++;
        err = wl_module_send((unsigned char *) &temp_hum_light_payload, sizeof (TEMP_HUM_LIGHT_Payload));
        if (err){
            status_payload.rf_error++;
        }
        
        
        if(temp_hum_light_payload.header.count % 12 == 1){
            /* send long payload */
            status_payload.millivolts = measure_power();
            status_payload.header.count++;
            err |= wl_module_send((unsigned char *) &status_payload, sizeof (Status_Payload));
        }

        peripheral_pwr(false);
        
        if (err){
            LED_RED = 0b1;
            sleep(WDT_1_MS);
            LED_RED = 0b0;
        }else{
            LED_GRN = 0b1;
            sleep(WDT_1_MS);
            LED_GRN = 0b0;
        }
        
        sleep(WDT_256_SEC);
        sleep(WDT_32_SEC);
    }
}


