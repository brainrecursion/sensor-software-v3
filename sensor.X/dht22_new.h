/* 
 * File:   dht22_new.h
 *
 * Created on 07 February 2014, 15:26
 */

#ifndef DHT22_NEW_H
#define	DHT22_NEW_H

#ifdef	__cplusplus
extern "C" {
#endif

int get_temp_hum(float* temp, float* hum);

#define DHTLIB_TIMEOUT             50 /* 100-200us */
#define DHTLIB_DHT_WAKEUP_MS       WDT_1_MS

#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#ifdef	__cplusplus
}
#endif

#endif	/* DHT22_NEW_H */

