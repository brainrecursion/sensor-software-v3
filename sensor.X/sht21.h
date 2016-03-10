/* 
 * File:   sht21.h
 * Author: 
 *
 * Created on 26 November 2015, 14:43
 */

#ifndef SHT21_H
#define	SHT21_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Error codes
typedef enum{
  ACK_ERROR                = 0x01,
  TIME_OUT_ERROR           = 0x02,
  CHECKSUM_ERROR           = 0x04,
  UNIT_ERROR               = 0x08,
  CMD_ERROR                = 0x16
}etError;

const uint16_t POLYNOMIAL = 0x131;  //P(x)=x^8+x^5+x^4+1 = 100110001

// sensor command
typedef enum{
  TRIG_T_MEASUREMENT_HM    = 0xE3, // command trig. temp meas. hold master
  TRIG_RH_MEASUREMENT_HM   = 0xE5, // command trig. humidity meas. hold master
  TRIG_T_MEASUREMENT_POLL  = 0xF3, // command trig. temp meas. no hold master
  TRIG_RH_MEASUREMENT_POLL = 0xF5, // command trig. humidity meas. no hold master
  USER_REG_W               = 0xE6, // command writing user register
  USER_REG_R               = 0xE7, // command reading user register
  SOFT_RESET               = 0xFE  // command soft reset
}etSHT2xCommand;

typedef enum {
  SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
  SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
  SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
  SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
  SHT2x_RES_MASK           = 0x81  // Mask for res. bits (7,0) in user reg.
} etSHT2xResolution;

typedef enum {
  SHT2x_EOB_ON             = 0x40, // end of battery
  SHT2x_EOB_MASK           = 0x40, // Mask for EOB bit(6) in user reg.
} etSHT2xEob;

typedef enum {
  SHT2x_HEATER_ON          = 0x04, // heater on
  SHT2x_HEATER_OFF         = 0x00, // heater off
  SHT2x_HEATER_MASK        = 0x04, // Mask for Heater bit(2) in user reg.
} etSHT2xHeater;

// measurement signal selection
typedef enum{
  HUMIDITY,
  TEMP
}etSHT2xMeasureType;

typedef enum{
  I2C_ADR_W                = 128,   // sensor I2C address + write bit
  I2C_ADR_R                = 129    // sensor I2C address + read bit
}etI2cHeader;




//==============================================================================
uint8_t SHT2x_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
//==============================================================================
// calculates checksum for n bytes of data and compares it with expected
// checksum
// input:  data[]       checksum is built based on this data
//         nbrOfBytes   checksum is built for n bytes of data
//         checksum     expected checksum
// return: error:       CHECKSUM_ERROR = checksum does not match
//                      0              = checksum matches

//==============================================================================
uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue);
//==============================================================================
// reads the SHT2x user register (8bit)
// input : -
// output: *pRegisterValue
// return: error

//==============================================================================
uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue);
//==============================================================================
// writes the SHT2x user register (8bit)
// input : *pRegisterValue
// output: -
// return: error

//==============================================================================
uint8_t SHT2x_MeasurePoll(etSHT2xMeasureType eSHT2xMeasureType, uint16_t *pMeasurand);
//==============================================================================
// measures humidity or temperature. This function polls every 10ms until
// measurement is ready.
// input:  eSHT2xMeasureType
// output: *pMeasurand:  humidity / temperature as raw value
// return: error
// note:   timing for timeout may be changed

//==============================================================================
uint8_t SHT2x_SoftReset();
//==============================================================================
// performs a reset
// input:  -
// output: -
// return: error

//==============================================================================
float SHT2x_CalcRH(uint16_t u16sRH);
//==============================================================================
// calculates the relative humidity
// input:  sRH: humidity raw value (16bit scaled)
// return: pHumidity relative humidity [%RH]

//==============================================================================
float SHT2x_CalcTemperatureC(uint16_t u16sT);
//==============================================================================
// calculates temperature
// input:  sT: temperature raw value (16bit scaled)
// return: temperature [�C]

//==============================================================================
uint8_t SHT2x_GetSerialNumber(uint8_t u8SerialNumber[]);
//==============================================================================
// gets serial number of SHT2x according application note "How To
// Read-Out the Serial Number"
// note:   readout of this function is not CRC checked
//
// input:  -
// output: u8SerialNumber: Array of 8 bytes (64Bits)
//         MSB                                         LSB
//         u8SerialNumber[7]             u8SerialNumber[0]
//         SNA_1 SNA_0 SNB_3 SNB_2 SNB_1 SNB_0 SNC_1 SNC_0
// return: error


#ifdef	__cplusplus
}
#endif

#endif	/* SHT21_H */
