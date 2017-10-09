/*
 * AtMega328P_UART.h
 * Description: My C header for use with the UART on the ATmega328P
 * Created on: May 31, 2016
 * Author: Robert Irwin
 */

#ifndef ATMEGA328P_UART_H_
#define ATMEGA328P_UART_H_


// Define Constants
//
#define F_OSC 16000000

// Function Declarations
//
/*	Name: 		 USART328P_INIT
 *	Description: Takes an unsigned integer defined by F_OSC/16/BAUD-1
 *				 and initializes the the UART according to the baud rate
 *				 specified by BAUD with odd parity checking and 2 stop bits.
 */
void USART328P_Init( unsigned int ubrr);

/*	Name: 		 USART328P_Transmit
 *	Description: Transmits multiple characters over the UART on the ATmega328P.
 *				 USART_INIT must be run prior to this function.
 */
void USART328P_Transmit( char* data, int num_bytes);


#endif /* ATMEGA328P_UART_H_ */
