/*
 * main.c
 *
 *  Created on: May 31, 2016
 *      Author: robertirwin
 */

#include    <avr/io.h>
#include    <stdlib.h>
#include    <avr/interrupt.h>
#include    <util/delay.h>
#include	"AtMega328P_UART.h"
#include	<string.h>
// Define Constants for USART
//
#define BAUD 76800
#define MYUBRR F_OSC/16/BAUD-1
#define CH0 0
#define CH1 1
#define CH2 2
#define CH3 3
#define FIRST 4

#define NUM_INPUT 4

int main (void)
{
    // *** Configure the UART (for testing) ***
    //
    USART328P_Init(MYUBRR);

    // *** Configure the ADC ***
    //

    // Enable a prescaler (determined by 16MHz clock)
    //
    ADCSRA |= 7 << ADPS0; // 125kHz

    // Turn on the ADC
    //
    ADCSRA |= 1 << ADEN;

    // Enable Global Interrupts
    //
    sei();

    // Enable interrupts in ADC
    //
    ADCSRA |= 1 << ADIE;

    // Enable Auto Triggering
    //
    ADCSRA |= 1 << ADATE;


    // Configure reference voltage for ADC ( 5V internal reference)
    //
    ADMUX |= (1 << REFS0);

    // Start the first conversion
    //
	_delay_ms(5000);
    ADCSRA |= 1 << ADSC;
    ADMUX++; // change channel for next conversion

    while(1)
    {
    }

    return 0;
}

// define the interrupt service routine
//

ISR(ADC_vect)
{
	static char flag = 0;
	// String Variable holding the conversion
	//
	//static char MUXMASK = 0x01;
	static char FourChannelRes[NUM_INPUT + 1];
	static char tempRes[NUM_INPUT + 1];
	static char channel = CH1;
	int unlockRes = NULL;

	unlockRes = ADC;

	// Only read every other sample on each channel to allow time for transmission
	//

	switch (channel) // marks the input currently being sampled
	{
		case CH0:
			// Storing Channel 3 Results
			//
			tempRes[3] = ADCL;
			tempRes[4] |= (ADCH << 6);
			// store the temp result in the permanent result for transmission
			//
			//flag = 0;
			memmove(FourChannelRes, tempRes, NUM_INPUT+1);
			channel = FIRST;
			// Transmit the Result
			//
			USART328P_Transmit(FourChannelRes, NUM_INPUT+1);
			break;

		case CH1:
			// Storing Channel 0 Results
			//
			tempRes[0] = ADCL;
			tempRes[4] = ADCH;
			channel = CH2;
			ADMUX++; //switch to channel 2
			break;

		case CH2:
			// Storing Channel 1 Results
			//
			tempRes[1] = ADCL;
			tempRes[4] |= (ADCH << 2) ;
			channel = CH3;
			ADMUX++; //switch to channel 3
			break;

		case CH3:
			//Storing Channel 2 Results
			//
			tempRes[2] = ADCL;
			tempRes[4] |= (ADCH << 4);
			channel = CH0;
			ADMUX &= ~(0x03); // back to channel 0
			break;

		default:
			//if (flag == 1)
			//{
				channel = CH1;
				ADMUX++;
			//}
			//else flag++;
			break;

		} // end case statement


}



