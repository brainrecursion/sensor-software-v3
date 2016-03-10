#include <xc.h>
#include "i2c.h"
#include "system.h"

void i2c_init() {
    SDA_TRIS = 1; //input
    SCL_TRIS = 1; //input
}

void i2c_uninit() {
    SDA_TRIS = 1; //input
    SCL_TRIS = 1; //input
}

//....................................................................
// This function generates an I2C Start Condition
//....................................................................

void i2c_start(void) {
    SDA_TRIS = 0; //output
    SCL_TRIS = 0; //output
    SDA = 1; // ensure SDA & SCL are high
    SCL = 1;
    __delay_us(10);
    SDA = 0; // pull SDA low
    __delay_us(10);
    SCL = 0; // pull SCL low
    __delay_us(10);
}


//....................................................................
// This function generates an I2C Stop Condition
//....................................................................

void i2c_stop(void) {
    SDA_TRIS = 0; //output
    SCL_TRIS = 0; //output
    SCL = 0; // ensure SCL is low
    SDA = 0; // SDA low
    __delay_us(10);
    SCL = 1; // pull SCL high
    __delay_us(10);
    SDA = 1; // allow SDA to be pulled high
    __delay_us(10);
}


//....................................................................
// Outputs a bit to the I2C bus
//....................................................................

void bit_out(unsigned char data) {
    unsigned int i;

    SCL = 0; // ensure SCL is low
    SDA_TRIS = 0; // configure SDA as an output
    SDA = (data >> 7); // output the MSB
    for (i = 0; i < 2; i++) NOP();
    SCL = 1; // pull SCL high to clock bit
    for (i = 0; i < 3; i++) NOP();
    SCL = 0; // pull SCL low for next bit
}


//....................................................................
// Inputs a bit from the I2C bus
//....................................................................

void bit_in(unsigned char *data) {
    unsigned int i;

    SCL = 0; // ensure SCL is low
    SDA_TRIS = 1; // configure SDA as an input
    SCL = 1; // bring SCL high to begin transfer
    for (i = 0; i < 3; i++) NOP();
    *data |= SDA_PIN; // input the received bit
    SCL = 0; // bring SCL low again.
}


//....................................................................
// Writes a byte to the I2C bus
//....................................................................

unsigned char i2c_wr(unsigned char data) {
    uint8_t mask, error = 0;
    SDA_TRIS = 0;
    for (mask = 0x80; mask > 0; mask >>= 1) //shift bit for masking (8 times)
    { //masking txByte, write bit to SDA-Line        

        if ((mask & data) == 0) SDA = 0;
        else SDA = 1;
        __delay_us(1); //data set-up time (t_SU;DAT)
        SCL = 1; //generate clock pulse on SCL
        __delay_us(5); //SCL high time (t_HIGH)
        SCL = 0;
        __delay_us(1); //data hold time(t_HD;DAT)
    }
    __delay_us(1);
    SDA_TRIS = 1; //release SDA-line
    SCL = 1; //clk #9 for ack
    __delay_us(5); //data set-up time (t_SU;DAT)
    if (SDA_PIN == 1) error = 1; //check ack from i2c slave
    SCL = 0;
    __delay_us(20); //wait time to see byte package on scope
    return error; //return error code
}


//....................................................................
// Reads a byte from the I2C bus
//....................................................................

unsigned char i2c_rd(unsigned char ack) {
    uint8_t mask, rxByte = 0;
    SDA_TRIS = 1; //release SDA-line
    for (mask = 0x80; mask > 0; mask >>= 1) //shift bit for masking (8 times)
    {
        SCL = 1; //start clock on SCL-line
        __delay_us(1); //data set-up time (t_SU;DAT)
        __delay_us(3); //SCL high time (t_HIGH)
        if (SDA_PIN == 1) rxByte = (rxByte | mask); //read bit
        SCL = 0;
        __delay_us(1); //data hold time(t_HD;DAT)
    }
    __delay_us(1);
    SDA_TRIS = 0;
    SDA = ack; //send acknowledge if necessary
    __delay_us(1); //data set-up time (t_SU;DAT)
    SCL = 1; //clk #9 for ack
    __delay_us(5); //SCL high time (t_HIGH)
    SCL = 0;
    SDA_TRIS = 1; //release SDA-line
    __delay_us(20); //wait time to see byte package on scope
    return rxByte; //return byte
}


//.............................................................................
//          Polls the bus for ACK from device
//.............................................................................

void ack_poll(unsigned char control) {
    unsigned char result = 1;

    while (result) {
        i2c_start(); // generate Restart condition
        result = i2c_wr(control); // send control byte (WRITE command)
    }

    i2c_stop(); // generate Stop condition
}