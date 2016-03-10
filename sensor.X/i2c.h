/* 
 * File:   i2c.h
 * Author: 
 *
 * Created on 26 November 2015, 13:55
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

    void i2c_init();
    void i2c_uninit();
void i2c_start(void);				// generates an I2C start condition
void i2c_stop(void);				// generates an I2C stop condition
void bit_out(unsigned char data);               // outputs a bit to I2C bus
void bit_in(unsigned char *data);               // inputs a bit from the I2C bus
unsigned char i2c_wr(unsigned char i2c_data);   // writes a byte to the I2C bus
unsigned char i2c_rd(unsigned char ack);	// reads a byte from the I2C bus
void ack_poll(unsigned char control);


/*#define SDA_TRIS  TRISCbits.TRISC4
#define SCL_TRIS  TRISAbits.TRISA5
#define SDA       PORTCbits.RC4
#define SCL       PORTAbits.RA5*/

//#define ACK       0x00
//#define NACK      0x80

// I2C acknowledge
typedef enum{
  ACK                      = 0,
  NO_ACK                   = 1,
}etI2cAck;


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

