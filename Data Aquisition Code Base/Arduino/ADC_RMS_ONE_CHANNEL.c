/*
 * main.c
 *
 *  Created on: Jun 3, 2016
 *      Author: robertirwin
 */
#include    <avr/io.h>
#include    <stdlib.h>
#include    <avr/interrupt.h>
#include    <util/delay.h>
#include    "AtMega328P_UART.h"
#include    <string.h>
#include"CircularBuff.h"
#include"DiscreteRMS.h"

// Define Constants
//
#define BAUD 76800
#define MYUBRR F_OSC/16/BAUD-1
#define FS (float) 125000.0/13.0/13.0
#define N (float) 13.0
#define STEPS (float) 1024.0
#define REF (float) 5.0
#define OFFSET (float) 512.0

// instantiate structures
//
CircularBuff circ1;
RMS RMS1;

// create pointers to the structures
//
CircularBuff* cb1 = &circ1;
RMS* rms1 = &RMS1;

// create global variables
int prev_sam = 0;
int ZERO = 512;
int * zero = &ZERO;
char * RMS_res;
int main (void)
{
  // *** Initialize all structures ***
  //
  CircBuffInit(cb1, (int) N);
  RMS_Init(rms1, REF, STEPS, OFFSET, N);

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

  // configure reference voltage for ADC (internal reference)
  //
  ADMUX |= 1 << REFS0;

  // Enable interrupts in ADC
  //
  ADCSRA |= 1 << ADIE;

  // Enable Auto Triggering
  //
  ADCSRA |= 1 << ADATE;

  // Start the first conversion
  //
  _delay_ms(5000);
  ADCSRA |= 1 << ADSC;

  while(1)
    {
    }

  return 0;
}

// define the interrupt service routine
//

ISR(ADC_vect)
{
  static int flag = 0;
  int res = 0;
  res = ADC;
  int temp;
  // Only read every other sample on each channel to allow time for transmission
  //
  if (flag == 12)
    {
      temp = res;
      flag = 0;
      if ((circ1.count) == N)
	{
	  CircBuffPut(cb1, (int *) &temp);
	  CircBuffGet(cb1, &prev_sam);
	  calculateRMS(rms1, &temp, &prev_sam);

	  RMS_res = (char *) &RMS1.rms_value;
	  USART328P_Transmit(RMS_res,4);
	  //USART328P_Transmit((char *) prev_sam,2);
	  USART328P_Transmit((char *) &res,2);
	}
      else
	{
	  CircBuffPut(cb1, (int *) &temp);
	  calculateRMS(rms1, &temp, zero);
	  RMS_res = (char *) &RMS1.rms_value;
	  USART328P_Transmit(RMS_res,4);
	  //USART328P_Transmit((char *) circ1.tailptr,2);
	  USART328P_Transmit((char *) &res ,2);
	}
    }
  else flag++;
}
