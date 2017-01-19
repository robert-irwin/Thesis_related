/*
 * main.c
 *
 *  Created on: Aug 15, 2016
 *      Author: robertirwin
 */

/*
 * main.c
 *
 *  Created on: Aug 15, 2016
 *      Author: robertirwin
 */

# include    <avr/io.h>
# include    <stdlib.h>
# include    <avr/interrupt.h>
# include    <util/delay.h>
# include    <string.h>
# include"w5500.h"
# include "AtMega328P_SPI.h"
# include<stdint.h>

# define DATABUF_LEN 200 //bytes
# defineMAXBUFFER 200

char myDEFGATEWAY[DGATE_LEN] = {0xC0, 0xA8, 0x01, 0x01}; //192.168.1.1
char myMACADDR[HWADD_LEN] = {0x90, 0xA2, 0xDA, 0x10, 0xA3, 0xB1};
char mySUBMASK[SUBMASK_LEN] = {0xFF, 0xFF, 0xFF, 0x00}; //255.255.255.0
char myIP[IP_LEN] = {0xC0, 0xA8, 0x00, 0x03}; // 192.168.0.3
char sPort[PORT_LEN] = {0x13, 0xBA}; // 5050

w5500packet PAC;
w5500packet * pac = &PAC;

char sockID;

// initialize the data buffer
//
char * dataBuf = NULL;

int main (void)
{
  // allocate memory for the data buffer
  //
  dataBuf = malloc(sizeof(char) * DATABUF_LEN);
  if (dataBuf == NULL) return 0;
  *dataBuf = 0x1A;

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
  sockID = w5500UDPServerSockConfig(pac, sPort);
  if (sockID == 0) return 0; // error opening socket

  // *** Configure the PWM Hardware ***

  // Set the proper pin on PortD to be an output
  //
  DDRD |= (1 << PD3);

  // Configure PWM on counter0 to set at BOTTOM and clear and compare match
  //
  TCCR2A |= (1 << COM2B1) | (3 << WGM20);
  OCR2B = 26; // initial 50% duty cycle on pin OC2A

  // set the frequency of the PWM signal
  //
  TCCR2B |= (3 << CS20); //16,000,000/32/256 = 2kHz

  // receive the control signal
  //
  int recvlen = 0;
  while(1)
    {
      while((recvlen = w5500UDPrecv(pac, sockID, dataBuf)) == 0); // wait for data
      OCR2B = *dataBuf;

    }


}

