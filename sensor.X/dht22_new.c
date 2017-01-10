#include "system.h"
#include <xc.h>
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "dht22_new.h"
#include <stdio.h>
#include <float.h>

/**
 * DHT22 sensor start
 */
void start_signal() {
    DHT_PIN_DIR = OUTPUT; // set data port to output
    DHT_PIN = 0; // data output LOW
    sleep(DHTLIB_DHT_WAKEUP_MS); // Low for at least 1ms
    DHT_PIN = 1; // set output HIGH
    __delay_us(20); // High for 20-40 us
    DHT_PIN_DIR = INPUT; // Data port is input
    //sensor should now set input to LOW for next 80us
}

/**
 *  DHT22 sensor - check response from dht22
 */
int check_response() {
    // GET ACKNOWLEDGE or TIMEOUT
    uint16_t loopCnt = 0;
    while (!DHT_PIN) {
        if (++loopCnt >= DHTLIB_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;
    }

    loopCnt = 0;
    while (DHT_PIN) {
        if (++loopCnt >= DHTLIB_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;
    }

    return DHTLIB_OK;
}

/**
 * DHT22 sensor - read data
 */
int ReadByte(uint8_t *byte) {
    *byte = 0;
    uint16_t loopCnt;

    for (int i = 0; i < 8; i++) {

        loopCnt = 0;
        while (!DHT_PIN) {
            if (++loopCnt >= DHTLIB_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;
        }

        __delay_us(40);
        if (DHT_PIN) *byte |= 1 << (7 - i); // If time > 40us, Data is 1

        loopCnt = 0;
        while (DHT_PIN) {
            if (++loopCnt >= DHTLIB_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;
        }

    }
    return DHTLIB_OK;
}

int done(int err){
    //DHT_PIN_DIR = OUTPUT;
    //DHT_PIN = 1;
    return err;
}

/**
 * DHT22 main
 */
int get_temp_hum(float* temp, float* hum) {
    int err;
    uint8_t RH_Byte1, RH_Byte2, T_Byte1, T_Byte2, CheckSum;

    start_signal();
    err = check_response();

    if (err) {
        return done(err);
    } else {
        if (err = ReadByte(&RH_Byte1)) return done(err);
        if (err = ReadByte(&RH_Byte2)) return done(err);
        if (err = ReadByte(&T_Byte1)) return done(err);
        if (err = ReadByte(&T_Byte2)) return done(err);
        if (err = ReadByte(&CheckSum)) return done(err);

        *temp = T_Byte1 & 0x7F;
        *temp *= 256;
        *temp += T_Byte2;
        *temp /= 10;
        if (T_Byte1 & 0x80)
            *temp *= -1;

        *hum = RH_Byte1;
        *hum *= 256;
        *hum += RH_Byte2;
        *hum /= 10;


        if (CheckSum != (uint8_t)(RH_Byte1+RH_Byte2+T_Byte1+T_Byte2)){
            //printf(" %x %x %x %x %x err\n",RH_Byte1,RH_Byte2,T_Byte1,T_Byte2,CheckSum);
            return done(DHTLIB_ERROR_CHECKSUM);
        }else{
            //printf(" %x %x %x %x %x \n",RH_Byte1,RH_Byte2,T_Byte1,T_Byte2,CheckSum);
        }

    }

    return done(DHTLIB_OK);
}
