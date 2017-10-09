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
# include	"CircularBuff.h"
# include	"DiscreteRMS.h"
# include	"w5500.h"
# include 	"AtMega328P_SPI.h"
# include	<stdint.h>

// Define Constants
//

# define 	CH0 (char) 0
# define 	CH1 (char) 1
# define 	CH2 (char) 2
# define 	CH3 (char) 3
# define    SEND (char) 4
# define 	DEFAULT (char) 5

# define 	DATABUF_LEN 12 //bytes

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
# define	MAXBUFFER 50

char myDEFGATEWAY[DGATE_LEN] = {0xC0, 0xA8, 0x01, 0x01}; //192.168.1.1
char myMACADDR[HWADD_LEN] = {0x90, 0xA2, 0xDA, 0x10, 0xA3, 0xD4};
char mySUBMASK[SUBMASK_LEN] = {0xFF, 0xFF, 0xFF, 0x00}; //255.255.255.0
char myIP[IP_LEN] = {0xC0, 0xA8, 0x00, 0x02}; // 192.168.0.2
char sPort[PORT_LEN] = {0x1F, 0x90}; // 8080
char dMAC[HWADD_LEN] = {0x40, 0x6C, 0x8F, 0x58, 0x56, 0xA2};
char dIP[IP_LEN] = {0xC0, 0xA8, 0x00, 0x01}; // 192.168.0.1
char dPort[PORT_LEN] = {0x15, 0xB3}; // 5555

w5500packet	 PAC;

w5500packet * pac = &PAC;

// create global variables
//

char sockID;
uint16_t * ch0currRes = NULL;
uint16_t * ch1currRes = NULL;
float * timeV01 = NULL;
uint16_t countV01 = 0;
uint16_t * ch2currRes = NULL;
uint16_t * ch3currRes = NULL;

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

	// construct the packet
	//
	ch0currRes = (uint16_t *) dataBuf;
	ch1currRes = (uint16_t *) (dataBuf + 2);
	ch2currRes = (uint16_t *) (dataBuf + 4);
	ch3currRes = (uint16_t *) (dataBuf + 6);
	timeV01 = (float *) (dataBuf + 8);
	// initialize timer variable
	//
	*timeV01 = 0.0;

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

    // Enable Global Interrupts
    //
    sei();

    // Turn on the ADC
    //
    ADCSRA |= 1 << ADEN;

    // First Conversion (for initialization of the hardware)
    //
    ADCSRA |= 1 << ADSC;

    _delay_ms(5000);

    // Enable interrupts in the ADC
    //
    ADCSRA |= 1 << ADIE;

    OCR0A = 50; // set compare value for ISR, 16e6/64/62 = 4000Hz --> sample rate of ADC

    TCCR0B |= (3 << CS00); // set timer0 prescaler *32*

    // Trigger ADC off of timer0 = OCR0A
    //
    ADCSRB |= (3 << ADTS0);


    // Enable Auto Triggering of ADC
    //
    ADCSRA |= 1 << ADATE;

    TCCR0A |= (3 << COM0A0); // set OC0A on compare match (cleared by executing ISR for Timer0 Comp match)

    TCNT0 = 0; // start the timer at 0

    // configure reference voltage for ADC (internal 5V reference)
    //
    ADMUX |= 1 << REFS0;

    // Enable interrupts in Timer0 output compare A
    //
    TIMSK0 |= (1 << OCIE0A);

    // Configure the 16-bit counter (for timestamps)
    // 16MHz/1 = 16MHz
    TCCR1B |= 1 << CS10;
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

    	break;

    case CH1:
    	// computing with channel 1 results
    	//
    	ADMUX++;
    	channel = CH2;
        *ch1currRes = res;

        break;

    case CH2:
        // computing with channel 2 results
    	//
    	ADMUX++;
    	channel = CH3;
       	*ch2currRes = res;

    	break;

    case CH3:
    	// computing with channel 3 results
    	//
    	channel = SEND;
    	ADMUX &= ~(0x03);
    	*ch3currRes = res;

    	break;

    case SEND:
    	//ADMUX++;
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
    	//ADMUX++;
    	channel = CH0;
    	break;
    }
}

ISR(ADC_vect){
	// get timer value
	//
	sei();
	if (channel == CH1){
		countV01 = TCNT1;
		TCNT1 = 0x0000; // reset counter1
		*timeV01 += countV01/16000.0; // milliseconds
	}
}
