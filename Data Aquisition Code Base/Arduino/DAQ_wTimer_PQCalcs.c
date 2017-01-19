/*
 * main.c
 *
 *  Created on: Jun 3, 2016
 *      Author: robertirwin
 */
# include    <avr/io.h>
# include    <stdlib.h>
# include    <avr/interrupt.h>
# include    <util/delay.h>
# include    <string.h>
# include    "AtMega328P_UART.h"
# include"CircularBuff.h"
# include"DiscreteRMS.h"
# include"w5500.h"
# include "AtMega328P_SPI.h"
# include<stdint.h>

// Define Constants
//
# define N (float) 20.0 // 3 periods
# define STEPS (float) 1024.0
# define REF (float) 5.0
# define OFFSET (float) 512.0
# define CH0 (char) 0
# define CH1 (char) 1
# define CH2 (char) 2
# define CH3 (char) 3
# define    SEND (char) 4
# define DEFAULT (char) 5

# define DATABUF_LEN 44 //bytes

// initialize the data buffer
//
char * dataBuf = NULL;



/* The data packet to be sent over the network is depicted below
 *
 * |--4 bytes--|--4 bytes--|--4 bytes--|--4 bytes--|-- 2 bytes--|-- 2 bytes --|
 * ----------------------------------------------------------------------------------
 * | Real Power| Reac Power|   I_RMS   |   V_RMS   | Curr I sam | Curr V sam  |
 * | (ch 0 & 1)| (ch 0 & 1)|   (ch 0)  |   (ch 1)  |   (ch 0)   |   (ch 1 )   |
 * ----------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------
 * |--4 bytes--|--4 bytes--|--4 bytes--|--4 bytes--|-- 2 bytes--|-- 2 bytes --|
 * ----------------------------------------------------------------------------------
 * | Real Power| Reac Power|   I_RMS   |   V_RMS   | Curr I sam | Curr V sam  |
 * | (ch 2 & 3)| (ch 2 & 3)|   (ch 2)  |   (ch 3)  |   (ch 2)   |   (ch 3 )   |
 * ----------------------------------------------------------------------------------
 */

// define constants for the w5500
//
# defineMAXBUFFER 50

char myDEFGATEWAY[DGATE_LEN] = {0xC0, 0xA8, 0x01, 0x01}; //192.168.1.1
char myMACADDR[HWADD_LEN] = {0x90, 0xA2, 0xDA, 0x10, 0xA3, 0xD4};
char mySUBMASK[SUBMASK_LEN] = {0xFF, 0xFF, 0xFF, 0x00}; //255.255.255.0
char myIP[IP_LEN] = {0xC0, 0xA8, 0x00, 0x02}; // 192.168.0.2
char sPort[PORT_LEN] = {0x1F, 0x90}; // 8080
char dMAC[HWADD_LEN] = {0x40, 0x6C, 0x8F, 0x58, 0x56, 0xA2};
char dIP[IP_LEN] = {0xC0, 0xA8, 0x00, 0x01}; // 192.168.0.1
char dPort[PORT_LEN] = {0x15, 0xB3}; // 5555

// instantiate structures
//
CircularBuff circ0;
CircularBuff circ1;
CircularBuff circ2;
CircularBuff circ3;
RMS RMS0;
RMS RMS1;
RMS RMS2;
RMS RMS3;
PowerCalc POW01;
PowerCalc POW23;
w5500packet PAC;

// create pointers to the structures
//
CircularBuff* cb0 = &circ0;
CircularBuff* cb1 = &circ1;
CircularBuff* cb2 = &circ2;
CircularBuff* cb3 = &circ3;
RMS* rms0 = &RMS0;
RMS* rms1 = &RMS1;
RMS* rms2 = &RMS2;
RMS* rms3 = &RMS3;
PowerCalc * pow01 = &POW01;
PowerCalc * pow23 = &POW23;
w5500packet * pac = &PAC;

// create global variables
//
uint16_t ch0prevRes = 0;
uint16_t ch1prevRes = 0;
uint16_t ch2prevRes = 0;
uint16_t ch3prevRes = 0;
uint16_t zero = (uint16_t) OFFSET;
float rms_null = 0.0;
char sockID;
uint16_t * ch0currRes = NULL;
uint16_t * ch1currRes = NULL;
float * timeV01 = NULL;
uint16_t countV01 = 0;
uint16_t * ch2currRes = NULL;
uint16_t * ch3currRes = NULL;

// pointers to the calculated RMS value from the
// respective ADC inputs
//
float * RMS_res0 = &rms_null;
float * RMS_res1 = &rms_null;
float * RMS_res2 = &rms_null;
float * RMS_res3 = &rms_null;

// pointers to real and reactive power results for channels
// zero and one, current and voltage, respectively.
//
float * avgpow01 = &rms_null;
float * qpow01 = &rms_null;
float * avgpow23 = &rms_null;
float * qpow23 = &rms_null;

// Make the current ADC channel a global variable
//
static char channel = DEFAULT;

int main (void)
{
  // allocate memory for the data buffer
  //
  dataBuf = malloc(sizeof(char) * DATABUF_LEN);
  if (dataBuf == NULL) return 0;

  // construct the data packet
  //
  avgpow01 = (float *) dataBuf;
  qpow01 = (float *) (dataBuf + 4);
  RMS_res0 = (float *) (dataBuf + 8);
  RMS_res1 = (float *) (dataBuf + 12);
  ch0currRes = (uint16_t *) (dataBuf + 16);
  ch1currRes = (uint16_t *) (dataBuf + 18);
  avgpow23 = (float *) (dataBuf + 20);
  qpow23 = (float *) (dataBuf + 24);
  RMS_res2 = (float *) (dataBuf + 28);
  RMS_res3 = (float *) (dataBuf + 32);
  ch2currRes = (uint16_t *) (dataBuf + 36);
  ch3currRes = (uint16_t *) (dataBuf + 38);
  timeV01 = (float *) (dataBuf + 40);

  // initialize timer variable
  //
  *timeV01 = 0.0;

  // *** Initialize all structures ***
  //
  CircBuffInit(cb0, (int) N);
  CircBuffInit(cb1, (int) N);
  CircBuffInit(cb3, (int) N);
  CircBuffInit(cb2, (int) N);
  PowerCalc_Init(pow01, REF, STEPS, OFFSET, N);
  PowerCalc_Init(pow23, REF, STEPS, OFFSET, N);
  RMS_Init(rms2, REF, STEPS, OFFSET, N);
  RMS_Init(rms3, REF, STEPS, OFFSET, N);
  RMS_Init(rms0, REF, STEPS, OFFSET, N);
  RMS_Init(rms1, REF, STEPS, OFFSET, N);

  // *** Configure the SPI interface ***
  //
  SPI328P_MasterInit();

  // *** Initialize the structure responsible for SPI communication ***
  //
  w5500packetInit(pac, MAXBUFFER);

  // *** Configure the physical and IP layer of the hardware network stack ***
  //
  w5500SourceHWConfig(pac, myDEFGATEWAY, mySUBMASK, myMACADDR, myIP);

  // open a socket according to the UDP protocol
  //
  sockID = w5500UDPSockConfig(pac, sPort, dMAC, dIP, dPort);
  if (sockID == 0) return 0; // error opening socket

  // *** Configure the ADC ***
  //

  // Disable Digital Input Pins on ADC Channels
  //
  DIDR0 |= 0x0F;

  // Enable a prescaler which dictates the clock frequency of the ADC (determined by 16MHz clock)
  //
  ADCSRA |= (0x07 << ADPS0); // 125kHz

  // Enable interrupts in the ADC
  //
  ADCSRA |= 1 << ADIE;

  // Enable Auto Triggering of ADC
  //
  ADCSRA |= 1 << ADATE;
  // Trigger ADC off of timer0 = OCR0A
  //
  ADCSRB |= (3 << ADTS0);

  // Turn on the ADC
  //
  ADCSRA |= 1 << ADEN;

  // Enable Global Interrupts
  //
  sei();

  // configure reference voltage for ADC (internal 5V reference)
  //
  ADMUX |= 1 << REFS0;

  // Enable interrupts in Timer0 output compare A
  //
  TIMSK0 |= (1 << OCIE0A);

  // Configure the 16-bit counter (for timestamps)
  // 16MHz/1 = 16MHz
  TCCR1B |= 1 << CS10;

  OCR0A = 104; // set compare value for ISR, 16e6/64/104 = 2400Hz --> sample rate of ADC
  TCCR0A |= (3 << COM0A0); // set OC0A on compare match (cleared by executing ISR for Timer0 Comp match)
  TCCR0B |= (3 << CS00); // set timer prescaler *64*

  // Start the first conversion
  //
  //ADCSRA |= (1 << ADSC);
  TCNT0 = 0; // start the timer at 0
  TCNT1 = 0x0000;
  while(1)
    {
    }

  return 0;
}

// define the interrupt service routine
//

ISR(TIMER0_COMPA_vect)
{
  // variable that holds the value from the ADC data register
  // it is necessary to read the ADC data register in order to update
  //
  static uint16_t res = 0;
  TCNT0 = 0x00; // reset counter0
  sei();

  // read the ADC (mandatory in order for the register to update)
  //
  res = ADC;

  switch(channel){
  case CH0:
    // computing with channel 0 results
    //
    ADMUX++;
    channel = CH1;
    *ch0currRes = res;

    // Enter the if once the circular buffer is full
    //
    if ((circ0.count) == N)
      {
	CircBuffPut(cb0, ch0currRes);
	CircBuffGet(cb0, &ch0prevRes);
	calculateRMS(rms0, ch0currRes, &ch0prevRes);
	*RMS_res0 = RMS0.rms_value;
	break;
      }
    // Otherwise, calculate RMS with the current sample and 0
    //
    else
      {
	CircBuffPut(cb0, ch0currRes);
	calculateRMS(rms0, ch0currRes, &zero);
	*RMS_res0 = RMS0.rms_value;
	break;
      }

  case CH1:
    // computing with channel 1 results
    //
    ADMUX++;
    channel = CH2;
    *ch1currRes = res;

    if ((circ1.count) == N)
      {
	CircBuffPut(cb1, ch1currRes);
	CircBuffGet(cb1, &ch1prevRes);
	calculateRMS(rms1, ch1currRes, &ch1prevRes);
	*RMS_res1 = RMS1.rms_value;
	Calculate_AVGPower(pow01, ch1currRes, ch0currRes, &ch1prevRes, &ch0prevRes);
	*avgpow01 = POW01.AVG_pow;
	Calculate_QPower(pow01, &RMS0.rms_value, &RMS1.rms_value);
	*qpow01 = POW01.Q_pow;
	break;
      }
    else
      {
	CircBuffPut(cb1, ch1currRes);
	calculateRMS(rms1, ch1currRes, &zero);
	*RMS_res1 = RMS1.rms_value;
	Calculate_AVGPower(pow01, ch1currRes, ch0currRes, &zero, &zero);
	*avgpow01 = POW01.AVG_pow;
	Calculate_QPower(pow01, &RMS0.rms_value, &RMS1.rms_value);
	*qpow01 = POW01.Q_pow;
	break;
      }

  case CH2:
    // computing with channel 2 results
    //
    ADMUX &= ~(0x03);
    channel = CH3;
    *ch2currRes = res;

    if ((circ2.count) == N)
      {
	CircBuffPut(cb2, ch2currRes);
	CircBuffGet(cb2, &ch2prevRes);
	calculateRMS(rms2, ch2currRes, &ch2prevRes);
	*RMS_res2 = RMS2.rms_value;
	break;
      }
    else
      {
	CircBuffPut(cb2, ch2currRes);
	calculateRMS(rms2, ch2currRes, &zero);
	*RMS_res2 = RMS2.rms_value;
	break;
      }

  case CH3:
    // computing with channel 3 results
    //
    channel = SEND;
    *ch3currRes = res;

    // Enter the if once the circular buffer is full
    //
    if ((circ3.count) == N)
      {
	CircBuffPut(cb3, ch3currRes);
	CircBuffGet(cb3, &ch3prevRes);
	calculateRMS(rms3, ch3currRes, &ch3prevRes);
	*RMS_res3 = RMS3.rms_value;
	Calculate_AVGPower(pow23, ch3currRes, ch2currRes, &ch3prevRes, &ch2prevRes);
	*avgpow23 = POW23.AVG_pow;
	Calculate_QPower(pow23, &RMS2.rms_value, &RMS3.rms_value);
	*qpow23 = POW23.Q_pow;
      }
    // Otherwise, calculate RMS with the current sample and 0
    //
    else
      {
	CircBuffPut(cb3, ch3currRes);
	calculateRMS(rms3, ch3currRes, &zero);
	*RMS_res3 = RMS3.rms_value;
	Calculate_AVGPower(pow23, ch3currRes, ch2currRes, &zero, &zero);
	*avgpow23 = POW23.AVG_pow;
	Calculate_QPower(pow23, &RMS2.rms_value, &RMS3.rms_value);
	*qpow23 = POW23.Q_pow;
      }
    break;

  case SEND:
    ADMUX++;
    channel = CH0;

    // send the packet
    //
    w5500UDPsend(pac, sockID, dataBuf, DATABUF_LEN);
    break;

  default:
    // enter the default case on the first interrupt because the
    // ADC takes longer on the first conversion due to the initialization
    // of hardware. (ignore very first sample)
    //
    TCNT1 = 0x0000;
    ADMUX++;
    channel = CH0;
    break;
  }
}

ISR(ADC_vect){
  // get timer value
  //
  if (channel == CH0){
    countV01 = TCNT1;
    TCNT1 = 0x0000; // reset counter1
    *timeV01 += countV01/16000.0; // milliseconds
  }
  sei();
}
