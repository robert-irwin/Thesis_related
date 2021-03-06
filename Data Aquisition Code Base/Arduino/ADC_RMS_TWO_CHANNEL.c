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
#define CH0 (char) 0
#define CH1 (char) 1
#define DEFAULT (char) 4

// instantiate structures
//
CircularBuff circ0;
CircularBuff circ1;
RMS RMS0;
RMS RMS1;

// create pointers to the structures
//
CircularBuff* cb0 = &circ0;
CircularBuff* cb1 = &circ1;
RMS* rms0 = &RMS0;
RMS* rms1 = &RMS1;

// create global variables
int prev_sam0 = 0;
int prev_sam1 = 0;
int ZERO = 512;
int * zero = &ZERO;
char rms_null = 0;
char * RMS_res0 = &rms_null;
char * RMS_res1 = &rms_null;

int main (void)
{
  // *** Initialize all structures ***
  //
  CircBuffInit(cb0, (int) N);
  RMS_Init(rms0, REF, STEPS, OFFSET, N);
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
  static char channel = DEFAULT;
  int res = 0;
  static int temp0 = 0;
  static int temp1 = 0;

  res = ADC;
  // Only read every other sample on each channel to allow time for transmission
  //
  switch(channel){
  case CH0:
    ADMUX ^= 0x01;
    channel = CH1;
    temp0 = res;
    if ((circ0.count) == N)
      {
	CircBuffPut(cb0, (int *) &temp0);
	CircBuffGet(cb0, &prev_sam0);
	calculateRMS(rms0, &temp0, &prev_sam0);
	RMS_res0 = (char *) &RMS0.rms_value;
	break;
      }
    else
      {
	CircBuffPut(cb0, (int *) &temp0);
	calculateRMS(rms0, &temp0, zero);
	RMS_res0 = (char *) &RMS0.rms_value;
	break;
      }
  case CH1:
    channel = DEFAULT;
    temp1 = res;
    flag = 0;
    if ((circ1.count) == N)
      {
	CircBuffPut(cb1, (int *) &temp1);
	CircBuffGet(cb1, &prev_sam1);
	calculateRMS(rms1, &temp1, &prev_sam1);
	RMS_res1 = (char *) &RMS1.rms_value;
	// send channel 0
	//
	USART328P_Transmit(RMS_res0,4);
	USART328P_Transmit((char *) &temp0,2);
	// send channel 1
	//
	USART328P_Transmit(RMS_res1,4);
	USART328P_Transmit((char *) &temp1,2);
	break;
      }
    else
      {
	CircBuffPut(cb1, (int *) &temp1);
	calculateRMS(rms1, &temp1, zero);
	RMS_res1 = (char *) &RMS1.rms_value;
	// send channel 0
	//
	USART328P_Transmit(RMS_res0,4);
	USART328P_Transmit((char *) &temp0,2);
	// send channel 1
	//
	USART328P_Transmit(RMS_res1,4);
	USART328P_Transmit((char *) &temp1,2);
	break;
      }
  default:
    if (flag == 12)
      {
	ADMUX ^= 0x01;
	channel = CH0;
      }
    else flag++;
    break;
  }
}
