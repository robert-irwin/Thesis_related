/*
 * AtMega328P_UART.c
 *
 *  Created on: Jun 3, 2016
 *      Author: robertirwin
 */

#include    <avr/io.h>
#include    <stdlib.h>
#include    <avr/interrupt.h>
#include    <util/delay.h>
#include	"AtMega328P_UART.h"
#include	<string.h>

// Function Definitions
//

/*	Name: USART328P_INIT
	Description: Takes an unsigned integer defined by F_OSC/16/BAUD-1
				 and initializes the the UART according to the baud rate
				 specified by BAUD with no parity checking and 1 stop bit.
*/
void USART328P_Init( unsigned int ubrr)
{
	// Set baud rate
	//
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable transmitter
	//
	UCSR0B = (1<<TXEN0);
	// Set frame format: 8data, 1stop bit, no parity
	//
	UCSR0C = (3<<UCSZ00);
}

/*	Name: USART328P_Transmit
	Description: Transmits multiple characters over the UART on the ATmega328P.
				 USART_INIT must be run prior to this function.
*/
void USART328P_Transmit( char* data, int num_bytes )
{
	int i = 0;
	while ( (i < num_bytes) )
	{
		// Wait for empty transmit buffer
		//
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = *data;
		data++;
		i++;
	}
}
