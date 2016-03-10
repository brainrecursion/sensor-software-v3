/*
        Modifications by Elia Ritterbusch
        http://eliaselectronics.com

	Copyright (c) 2011 by Ernst Buchmann

	Code based on the work of Stefan Engelke and Brennan Ball

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.


*/
#include <xc.h>
#include "wl_module.h"
#include "nRF24L01.h"
#include "spi.h"
#include "system.h"
#include "user.h"

// Defines for setting the wl_module registers for transmitting or receiving mode
#define TX_POWERUP wl_module_config_register(CONFIG, wl_module_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP wl_module_config_register(CONFIG, wl_module_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )

// Flag which denotes transmitting mode
volatile unsigned char PTX;

void wl_module_init(void)
// Initializes pins and interrupt to communicate with the wl_module
// Should be called in the early initializing phase at startup.
{
    // Define CSN and CE as Output and set them to default
    wl_module_CSN_dir = 0;
    wl_module_CE_dir = 0;
    wl_module_CE_lo;
    wl_module_CSN_hi;

    // set up INT2
    /*ANSELAbits.ANSA2 = 0; // digital input buffer enabled
    TRISA2 = 1; // RA2 as input
    OPTION_REGbits.INTEDG = 0; // trigger interrupt on falling edge
    INTCONbits.INTE = 1; // enable INT2 interrupt
*/
    // Initialize spi module
    spi_init();
}


void wl_module_config(void)
// Sets the important registers in the wl-module and powers the module
// in receiving mode
{
    // Set RF channel
    wl_module_config_register(RF_CH,wl_module_CH);
	// Set data speed & Output Power configured in wl_module.h
	wl_module_config_register(RF_SETUP,wl_module_RF_SETUP);
	// Set length of incoming payload
    wl_module_config_register(RX_PW_P0, wl_module_PAYLOAD);

    // Start receiver
    PTX = 0;        // Start in receiving mode
    RX_POWERUP;     // Power up in receiving mode
    wl_module_CE_hi;     // Listening for pakets
}

extern void wl_module_rx_config(void)
// Sets the important registers in the wl-module and powers the module
// in receiving mode
{
	unsigned char data[5];
    // Set RF channel
    wl_module_config_register(RF_CH,wl_module_CH);
	// Set data speed & Output Power configured in wl_module.h
	wl_module_config_register(RF_SETUP,wl_module_RF_SETUP);
    wl_module_config_register(SETUP_AW,SETUP_AW_3BYTES);
    
    wl_module_config_register(FEATURE,wl_module_FEATURE);
    wl_module_config_register(DYNPD,0x3f);
	//Enable all RX Data-Pipes
	wl_module_config_register(EN_RXADDR, EN_RXADDR_ERX_ALL);
	//Set RX_Address Pipe 0
	data[0]= data[1]= data[2]= data[3]= data[4]= RX_ADDR_P0_B0_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 5, 0);
	//Set RX_Address Pipe 1
	data[0]= data[1]= data[2]= data[3]= data[4]= RX_ADDR_P1_B0_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 5, 1);
	//Set RX_Address Pipe 2-5
	data[0]=RX_ADDR_P2_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 1, 2);
	data[0]=RX_ADDR_P3_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 1, 3);
	data[0]=RX_ADDR_P4_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 1, 4);
	data[0]=RX_ADDR_P5_DEFAULT_VAL;
	wl_module_set_rx_addr(data, 1, 5);
    // Set length of incoming payload
    wl_module_config_register(RX_PW_P0, wl_module_PAYLOAD);
	wl_module_config_register(RX_PW_P1, wl_module_PAYLOAD);
	wl_module_config_register(RX_PW_P2, wl_module_PAYLOAD);
	wl_module_config_register(RX_PW_P3, wl_module_PAYLOAD);
	wl_module_config_register(RX_PW_P4, wl_module_PAYLOAD);
	wl_module_config_register(RX_PW_P5, wl_module_PAYLOAD);


    // Start receiver
    PTX = 0;        // Start in receiving mode
    RX_POWERUP;     // Power up in receiving mode
    wl_module_CE_hi;     // Listening for packets
}

// Sets the wl-module as one of the six sender. Define for every sender a unique Number (wl_module_TX_NR_x)
// when you call this Function.
//  Each TX will get a TX-Address corresponding to the RX-Device.
// RX_Address_Pipe_0 must be the same as the TX-Address
extern void wl_module_tx_config(unsigned char tx_nr)
{
	unsigned char tx_addr[5];

    // Set RF channel
    wl_module_config_register(RF_CH,wl_module_CH);
	// Set data speed & Output Power configured in wl_module.h
	wl_module_config_register(RF_SETUP,wl_module_RF_SETUP);
	//Config the CONFIG Register (Mask IRQ, CRC, etc)
	wl_module_config_register(CONFIG, wl_module_CONFIG);
    // Set length of incoming payload
    //wl_module_config_register(RX_PW_P0, wl_module_PAYLOAD);
    wl_module_config_register(SETUP_AW,SETUP_AW_3BYTES);
    
    wl_module_config_register(FEATURE,wl_module_FEATURE);
    wl_module_config_register(DYNPD,0x3f);

	wl_module_config_register(SETUP_RETR,(SETUP_RETR_ARD_1000 | SETUP_RETR_ARC_15));

	//set the TX address for the pipe with the same number as the iteration
			switch(tx_nr)
			{
				case 0: //setup TX address as default RX address for pipe 0 (E7:E7:E7:E7:E7)
					tx_addr[0] = tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P0_B0_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
				case 1: //setup TX address as default RX address for pipe 1 (C2:C2:C2:C2:C2)
					tx_addr[0] = tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P1_B0_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
				case 2: //setup TX address as default RX address for pipe 2 (C2:C2:C2:C2:C3)
					tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P1_B0_DEFAULT_VAL;
					tx_addr[0] = RX_ADDR_P2_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
				case 3: //setup TX address as default RX address for pipe 3 (C2:C2:C2:C2:C4)
					tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P1_B0_DEFAULT_VAL;
					tx_addr[0] = RX_ADDR_P3_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
				case 4: //setup TX address as default RX address for pipe 4 (C2:C2:C2:C2:C5)
					tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P1_B0_DEFAULT_VAL;
					tx_addr[0] = RX_ADDR_P4_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
				case 5: //setup TX address as default RX address for pipe 5 (C2:C2:C2:C2:C6)
					tx_addr[1] = tx_addr[2] = tx_addr[3] = tx_addr[4] = RX_ADDR_P1_B0_DEFAULT_VAL;
					tx_addr[0] = RX_ADDR_P5_DEFAULT_VAL;
					wl_module_set_TADDR(tx_addr,3);
					wl_module_set_RADDR(tx_addr,3);
					break;
			}

	PTX =0;
	TX_POWERUP;
}

//sets the TX address in the TX_ADDR register
//unsigned char * address is the actual address to be used.  It should be sized
//	according to the tx_addr length specified to the nrf24l01.
//unsigned int len is the length of the address.  Its value should be specified
//	according to the tx_addr length specified to the nrf24l01.
extern void wl_module_set_tx_addr(unsigned char * address, unsigned char len)
{
	wl_module_write_register(TX_ADDR, address, len);
}

//sets up the 24L01 as a transmitter
//this function takes the existing contents of the CONFIG register and simply
//  clears the PRIM_RX bit in the CONFIG register.
//note: if the read value of the CONFIG register already has the PRIM_RX bit cleared, this
//  function exits in order to not make an unecessary register write.
extern void wl_module_set_as_tx(void)
{
	unsigned char config;

	wl_module_read_register(CONFIG, &config, 1);

	if((config & CONFIG_PRIM_RX) == 0)
		return;

	config &= (~CONFIG_PRIM_RX);

	wl_module_write_register(CONFIG, &config, 1);

	wl_module_CE_lo;
}

//powers down the 24L01
//this function takes the existing contents of the CONFIG register and simply
//  clears the PWR_UP bit in the CONFIG register.
//note: if the read value of the CONFIG register already has the PWR_UP bit cleared, this
//  function exits in order to not make an unecessary register write.
extern void wl_module_power_down(void)
{
	unsigned char config;

	wl_module_read_register(CONFIG, &config, 1);

	if((config & CONFIG_PWR_UP) == 0)
		return;

	config &= (~CONFIG_PWR_UP);

	wl_module_write_register(CONFIG, &config, 1);

	wl_module_CE_lo;
}

//sets the RX address in the RX_ADDR register that is offset by rxpipenum
//unsigned char * address is the actual address to be used.  It should be sized
//	according to the rx_addr length that is being filled.
//unsigned int len is the length of the address.  Its value should be specified
//	according to the rx_addr length specified to the nrf24l01.
//unsigned char rxpipenum is the pipe number (zero to five) whose address is being
//	specified.  If an invalid address (greater than five) is supplied, the function
//  does nothing.
extern void wl_module_set_rx_addr(unsigned char * address, unsigned char len, unsigned char rxpipenum)
{
	if(rxpipenum > 5)
		return;

	wl_module_write_register(RX_ADDR_P0 + rxpipenum, address, len);
}

extern void wl_module_get_rx_addr(unsigned char *data, unsigned char rxpipenum, unsigned char len)
{

	if((rxpipenum > 5))
		return;

	wl_module_read_register(RX_ADDR_P0 + rxpipenum, data, len);


}

//sets the RX payload width on the pipe offset by rxpipenum
//unsigned char payloadwidth is the length of the payload for the pipe referenced in
//  rxpipenum.  It must be less than or equal to 32.  If an invalid payload width is
//  specified, the function does nothing.
//unsigned char rxpipenum is the pipe number (zero to five) whose address is being
//	specified.  If an invalid address (greater than five) is supplied, the function
//  does nothing.
extern void wl_module_set_rx_pw(unsigned char payloadwidth, unsigned char rxpipenum)
{
	if((rxpipenum > 5) || (payloadwidth > 32))
		return;

	wl_module_write_register(RX_PW_P0 + rxpipenum, &payloadwidth, 1);
}

//gets the RX payload width on the pipe offset by rxpipenum
//unsigned char rxpipenum is the pipe number (zero to five) whose address is being
//	specified.  If an invalid address (greater than five) is supplied, the function
//  does nothing.
extern unsigned char wl_module_get_rx_pw(unsigned char rxpipenum)
{
	unsigned char data;

	if((rxpipenum > 5))
		return 0;

	wl_module_read_register(RX_PW_P0 + rxpipenum, &data, 1);

	return data;
}

//returns the current pipe in the 24L01's STATUS register
extern unsigned char wl_module_get_rx_pipe(void)
{
	return wl_module_get_rx_pipe_from_status(wl_module_get_status());
}

extern unsigned char wl_module_get_rx_pipe_from_status(unsigned char status)
{
	return ((status & 0xE) >> 1);
}

void wl_module_set_RADDR(unsigned char * adr, unsigned char size)
// Sets the receiving address
{
    wl_module_CE_lo;
    wl_module_write_register(RX_ADDR_P0,adr,size);
    wl_module_CE_hi;
}

void wl_module_set_TADDR(unsigned char * adr, unsigned char size)
// Sets the transmitting address
{
    wl_module_write_register(TX_ADDR, adr, size);
}



extern unsigned char wl_module_data_ready(void)
// Checks if data is available for reading
{
    //if (PTX) return 0;
    unsigned char status;
    // Read wl_module status
    wl_module_CSN_lo;                                // Pull down chip select
    status = spi_fast_shift(NOOP);               // Read status register
    wl_module_CSN_hi;                                // Pull up chip select
    return status & (1<<RX_DR);
}

//returns true if TX_EMPTY bit in FIFO_STATUS register is set, false otherwise
extern unsigned char wl_module_fifo_tx_empty(void)
{
	unsigned char data;

	wl_module_read_register(FIFO_STATUS, &data, 1);


	return (data & FIFO_STATUS_TX_EMPTY);
}

//returns true if RX_EMPTY bit in FIFO_STATUS register is set, false otherwise
extern unsigned char wl_module_fifo_rx_empty(void)
{
	unsigned char data;

	wl_module_read_register(FIFO_STATUS, &data, 1);

	return (data & FIFO_STATUS_RX_EMPTY);
}

//returns the current RF channel in RF_CH register
extern unsigned char wl_module_get_rf_ch(void)
{
	unsigned char data;

	wl_module_read_register(RF_CH, &data, 1);

	return data;
}

//returns the current RF_SETUP Register
extern unsigned char wl_module_get_rf_setup(void)
{
	unsigned char data;

	wl_module_read_register(RF_SETUP, &data, 1);

	return data;
}

//returns the current PLOS_CNT value in OBSERVE_TX register
extern unsigned char wl_module_get_plos_cnt(void)
{
	unsigned char data;

	wl_module_read_register(OBSERVE_TX, &data, 1);

	return ((data & OBSERVE_TX_PLOS_CNT) >> 4);
}

//returns the current ARC_CNT value in OBSERVE_TX register
extern unsigned char wl_module_get_arc_cnt(void)
{
	unsigned char data;

	wl_module_read_register(OBSERVE_TX, &data, 1);

	return (data & OBSERVE_TX_ARC_CNT);
}

//return the value of the status register
extern unsigned char wl_module_get_status(void)
{
	return wl_module_get_one_byte(NOOP);
}

extern unsigned char wl_module_get_rx_pipe_reading_status(void)
{
	unsigned char pipe;
	pipe = wl_module_get_one_byte(NOOP);
	return ((pipe & 0x0E) >> 1);
}

extern unsigned char wl_module_get_one_byte(unsigned char command)
{
unsigned char status;

wl_module_CSN_lo;
status = spi_fast_shift(command);
wl_module_CSN_hi;

return status;

}

extern unsigned char wl_module_get_data(unsigned char * data)
// Reads wl_module_PAYLOAD bytes into data array
{
	unsigned char status;
    wl_module_CSN_lo;                               // Pull down chip select
    status = spi_fast_shift( R_RX_PAYLOAD );            // Send cmd to read rx payload
    spi_transfer_sync(data,data,wl_module_PAYLOAD); // Read payload
    wl_module_CSN_hi;                               // Pull up chip select
    wl_module_config_register(STATUS,(1<<RX_DR));   // Reset status register
	return status;
}

void wl_module_config_register(unsigned char reg, unsigned char value)
// Clocks only one byte into the given wl-module register
{
    wl_module_CSN_lo;
    spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg));
    spi_fast_shift(value);
    wl_module_CSN_hi;
}

void wl_module_read_register(unsigned char reg, unsigned char * value, unsigned char len)
// Reads an array of bytes from the given start position in the wl-module registers.
{
    wl_module_CSN_lo;
    spi_fast_shift(R_REGISTER | (REGISTER_MASK & reg));
    spi_transfer_sync(value,value,len);
    wl_module_CSN_hi;
}

void wl_module_write_register(unsigned char reg, unsigned char * value, unsigned char len)
// Writes an array of bytes into inte the wl-module registers.
{
    wl_module_CSN_lo;
    spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg));
    spi_transmit_sync(value,len);
    wl_module_CSN_hi;
}


unsigned char wl_module_send(unsigned char * value, unsigned char len)
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
{
    
    //while (PTX) {}                  // Wait until last paket is send
    int i = 0;
    unsigned char err = 0;

    wl_module_CE_lo;

    PTX = 1;                        // Set to transmitter mode
    TX_POWERUP;                     // Power up

    wl_module_CSN_lo;                    // Pull down chip select
    spi_fast_shift( FLUSH_TX );     // Write cmd to flush tx fifo
    wl_module_CSN_hi;                    // Pull up chip select

    wl_module_CSN_lo;
    
    spi_fast_shift( W_TX_PAYLOAD ); // Write cmd to write payload
    
    
    spi_transmit_sync(value,len);   // Write payload
    
    wl_module_CSN_hi;                    // Pull up chip select

    wl_module_CE_hi;                     // Start transmission
    
    i=0;
    while (PTX){
        sleep(WDT_1_MS);
        i++;
        unsigned char status = wl_module_get_status();
        if (status & (1 << TX_DS)) { // IRQ: Package has been sent
            wl_module_config_register(STATUS, status); //Clear Bit
            PTX = 0;
        }

        if (status & (1 << MAX_RT)) { // IRQ: Package has not been sent, send again
            wl_module_config_register(STATUS, status); // Clear Bit
            long_payload.rf_max_rt++;
            PTX = 0; // give up
            err = 1;
        }
        
        /* timeout incase something goes wrong */
        if (i>100){
            wl_module_config_register(STATUS, status); // Clear Bit
            long_payload.rf_timeout++;
            PTX = 0; // give up
            err = 1;
        }
    }
   wl_module_CE_lo;
   return err;
}
