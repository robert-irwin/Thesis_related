/*
 * AtMega328P_SPI.c
 *
 *  Created on: Jun 27, 2016
 *      Author: robertirwin
 */

#include    <avr/io.h>
#include    <stdlib.h>
#include	"AtMega328P_SPI.h"

/* Name:		SPI328P_MasterInit
 * Description:	Initializes the the data direction register of port B (SPI interface),
 * 				such that the MOSI and SCK pins are set to outputs. This function also
 * 				enables he SPI interface, and sets the SCK frequency to 1 MHz.
 */
void SPI328P_MasterInit(void)
{
	// Set MOSI, SCK, and SS as outputs, all others input
	//
	DDRB |= (1<<PB3)|(1<<PB5)|(1<<PB2); // digital IO pins 11, 13 and 10, respectively
	// Enable SPI, Master, set clock rate fck/16 (1MHz)
	//
	SPCR |= (1<<SPE)|(1<<MSTR);
	SPSR |= (1<<SPI2X);
	//Set SS Pin High
	//
	PORTB |= (1<<PB2);
}

/* Name:		SPI328P_Master_Transaction
 * Description:	Sends 'numbytes' number of bytes over the SPI interface configured as master.
 * 				This function writes then reads data. The data that is read is stored in the input
 * 				buffer 'recvbuf'.
 * 				SPI328P_MasterInit must be run prior to this function
 */
void SPI328P_MasterTransaction(char * data, int numbytes, char * recvbuf)
{
	//active SPI
	//
	PORTB &= ~(1<<PB2);
	for (int i = 0; i < numbytes; i++)
	{
		// Start transmission
		//
		SPDR = *data;
		// move to the next byte of data
		//
		// Wait for transmission complete
		//
		while(!(SPSR & (1<<SPIF)));
		// read the incoming bytes
		//
		*recvbuf = SPDR;
		recvbuf++;
		data++;
	}
	// disable transmission
	//
	PORTB |= (1<<PB2);
}
