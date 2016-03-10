#include "system.h"
#include <xc.h>
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>
#include <float.h>
#include "sht21.h"
#include "i2c.h"

//==============================================================================

uint8_t SHT2x_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
//==============================================================================
{
    uint8_t crc = 0;
    uint8_t byteCtr;
    //calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr) {
        crc ^= (data[byteCtr]);
        for (uint8_t bitt = 8; bitt > 0; --bitt) {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
    if (crc != checksum) return CHECKSUM_ERROR;
    else return 0;
}

//===========================================================================

uint8_t SHT2x_ReadUserRegister(uint8_t *pRegisterValue)
//===========================================================================
{
    uint8_t checksum; //variable for checksum byte
    uint8_t error = 0; //variable for error code

    i2c_start();
    error |= i2c_wr(I2C_ADR_W);
    error |= i2c_wr(USER_REG_R);
    i2c_start();
    error |= i2c_wr(I2C_ADR_R);
    *pRegisterValue = i2c_rd(ACK);
    checksum = i2c_rd(NO_ACK);
    error |= SHT2x_CheckCrc(pRegisterValue, 1, checksum);
    i2c_stop();
    return error;
}

//===========================================================================

uint8_t SHT2x_WriteUserRegister(uint8_t *pRegisterValue)
//===========================================================================
{
    uint8_t error = 0; //variable for error code

    i2c_start();
    error |= i2c_wr(I2C_ADR_W);
    error |= i2c_wr(USER_REG_W);
    error |= i2c_wr(*pRegisterValue);
    i2c_stop();
    return error;
}

//===========================================================================

uint8_t SHT2x_MeasurePoll(etSHT2xMeasureType eSHT2xMeasureType, uint16_t *pMeasurand)
//===========================================================================
{
    uint8_t checksum; //checksum
    uint8_t data[2]; //data array for checksum verification
    uint8_t error = 0; //error variable
    uint16_t i = 0; //counting variable

    *pMeasurand = 0;

    //-- write I2C sensor address and command --
    i2c_start();
    error |= i2c_wr(I2C_ADR_W); // I2C Adr
    switch (eSHT2xMeasureType) {
        case HUMIDITY:
            error |= i2c_wr(TRIG_RH_MEASUREMENT_POLL);
            break;
        case TEMP:
            error |= i2c_wr(TRIG_T_MEASUREMENT_POLL);
            break;
        default: return CMD_ERROR;
    }
    //-- poll every 8ms for measurement ready. Timeout afloater 25 retries (200ms)--
    do {
        __delay_us(5);
        i2c_start();
        sleep(WDT_8_MS); //delay 8ms
        if (i++ >= 20) break;
    } while (i2c_wr(I2C_ADR_R) == ACK_ERROR);
    if (i >= 25) error |= TIME_OUT_ERROR;

    //-- read two data bytes and one checksum byte --
    //pMeasurand->s16.u8H = data[0] = i2c_rd(ACK);
    //pMeasurand->s16.u8L = data[1] = i2c_rd(ACK);
    data[0] = i2c_rd(ACK);
    data[1] = i2c_rd(ACK);
    *pMeasurand = (data[0] << 8) | data[1];
    checksum = i2c_rd(NO_ACK);

    //-- verify checksum --
    error |= SHT2x_CheckCrc(data, 2, checksum);
    i2c_stop();

    return error;
}

//===========================================================================

uint8_t SHT2x_SofloatReset()
//===========================================================================
{
    uint8_t error = 0; //error variable

    i2c_start();
    error |= i2c_wr(I2C_ADR_W); // I2C Adr
    error |= i2c_wr(SOFT_RESET); // Command
    i2c_stop();

    sleep(WDT_16_MS); // wait till sensor has restarted

    return error;
}

//==============================================================================

float SHT2x_CalcRH(uint16_t u16sRH)
//==============================================================================
{
    float humidityRH; // variable for result

    u16sRH &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --

    humidityRH = -6.0 + 125.0 / 65536 * (float) u16sRH; // RH= -6 + 125 * SRH/2^16
    return humidityRH;
}

//==============================================================================

float SHT2x_CalcTemperatureC(uint16_t u16sT)
//==============================================================================
{
    float temperatureC; // variable for result

    u16sT &= ~0x0003; // clear bits [1..0] (status bits)

    //-- calculate temperature [°C] --
    temperatureC = -46.85 + 175.72 / 65536 * (float) u16sT; //T= -46.85 + 175.72 * ST/2^16
    return temperatureC;
}

//==============================================================================

uint8_t SHT2x_GetSerialNumber(uint8_t u8SerialNumber[])
//==============================================================================
{
    uint8_t error = 0; //error variable

    //Read from memory location 1
    i2c_start();
    error |= i2c_wr(I2C_ADR_W); //I2C address
    error |= i2c_wr(0xFA); //Command for readout on-chip memory
    error |= i2c_wr(0x0F); //on-chip memory address
    i2c_start();
    error |= i2c_wr(I2C_ADR_R); //I2C address
    u8SerialNumber[5] = i2c_rd(ACK); //Read SNB_3
    i2c_rd(ACK); //Read CRC SNB_3 (CRC is not analyzed)
    u8SerialNumber[4] = i2c_rd(ACK); //Read SNB_2
    i2c_rd(ACK); //Read CRC SNB_2 (CRC is not analyzed)
    u8SerialNumber[3] = i2c_rd(ACK); //Read SNB_1
    i2c_rd(ACK); //Read CRC SNB_1 (CRC is not analyzed)
    u8SerialNumber[2] = i2c_rd(ACK); //Read SNB_0
    i2c_rd(NO_ACK); //Read CRC SNB_0 (CRC is not analyzed)
    i2c_stop();

    //Read from memory location 2
    i2c_start();
    error |= i2c_wr(I2C_ADR_W); //I2C address
    error |= i2c_wr(0xFC); //Command for readout on-chip memory
    error |= i2c_wr(0xC9); //on-chip memory address
    i2c_start();
    error |= i2c_wr(I2C_ADR_R); //I2C address
    u8SerialNumber[1] = i2c_rd(ACK); //Read SNC_1
    u8SerialNumber[0] = i2c_rd(ACK); //Read SNC_0
    i2c_rd(ACK); //Read CRC SNC0/1 (CRC is not analyzed)
    u8SerialNumber[7] = i2c_rd(ACK); //Read SNA_1
    u8SerialNumber[6] = i2c_rd(ACK); //Read SNA_0
    i2c_rd(NO_ACK); //Read CRC SNA0/1 (CRC is not analyzed)
    i2c_stop();

    return error;
}