/*
 * main.c
 *
 *  Created on: Aug 5, 2016
 *      Author: robertirwin
 */

# include <stdint.h>
# include<stdio.h>
# include <stdlib.h>
# include<sys/socket.h>
# include<arpa/inet.h>
# include"CircularBuff.h"
# include"DiscreteRMS.h"

// defines pertaining to control system
//

# define N (float) 17.0 // one period
# define STEPS (float) 1024.0
# define REF (float) 5.0
# define OFFSET (float) 512.0
# define DSET (char) 132 // corresponds to 52% duty cycle. Will produce 120V open circuit
# define VSET (float) 120.0 // desired voltage at the load
# define kp (float) 0.5 // proportional gain
# define ki (float) 0 // integral gain
# define MATROWS N
# define MATCOLS 2
# define MATINIT (float) 0.001
# define Ts (float) 0.001
// defines pertaining to communications
//
# define RECVBUFLEN 12 // 2 bytes for each of the 4 signals, 4 bytes for the timestamp on the source voltage
# define SENDBUFLEN 1
# define SRVRPORT 5555
# define ARD_CTRL_IP "192.168.0.3"
# define ARD_CTRL_PORT 5050

// declare global variables for each of the sampled channel
//

uint16_t ch0prevRes = 0;
uint16_t ch1prevRes = 0;
uint16_t ch2prevRes = 0;
uint16_t ch3prevRes = 0;
uint16_t * ch0currRes = NULL;
uint16_t * ch1currRes = NULL;
uint16_t * ch2currRes = NULL;
uint16_t * ch3currRes = NULL;
float * timestamp = NULL;

// declare variables for communicating with controller arduino
//

float Verr_old = 0.0;
float Verr_new = 0.0;
int new_d = 0;
static int old_d = (int) DSET;

CircularBuff circ0;
CircularBuff * cb0 = &circ0;

RMS RMS0;
RMS * rms0 = &RMS0;

PowerCalc POW01;
PowerCalc * pow01 = &POW01;

int main(void)
{

  /* ==========================================================  *\
   *                                                             *
   *    *
   * - Initialization of the necessary structures for Control -  *
   *    *
   *     *
   /* ==========================================================  */

  CircBuffInit(cb1, (int) N);

  RMS_Init(rms1, REF, STEPS, OFFSET, N);

  float * RMS_res0 = malloc(sizeof(float *)); // variable to hold rms result

  PowerCalc_Init(pow01, REF, STEPS, OFFSET, N);

  uint16_t zero = (uint16_t) OFFSET;

  /* ==========================================================  *\
   *                                                             *
   *    *
   * --------- Initialization of the necessary sockets --------  *
   *    *
   *     *
   /*=========================================================  */

  // initialize a buffer to hold received data
  //
  char * recvbuf = NULL;
  char * sendbuf = NULL;
  sendbuf = malloc(sizeof(char) * SENDBUFLEN);
  recvbuf = malloc(sizeof(char) * RECVBUFLEN);
  *sendbuf = DSET;
  // point the pointers inside the receive buffer
  //
  ch0currRes = (uint16_t *) recvbuf; // Vs
  ch1currRes = (uint16_t *) (recvbuf + 2); // Is
  ch2currRes = (uint16_t *) (recvbuf + 4); // Vl
  ch3currRes = (uint16_t *) (recvbuf + 6); // Il
  timestamp = (float *) (recvbuf + 8); // tVs

  // instantiate the structure to hold information about the server
  //
  struct sockaddr_in SRVRADDR, CLIENTADDR, ARDSRVR;

  int mysock, slen, ardsock = sizeof(SRVRADDR) , recv_len;

  // create the socket object for the arduino controller
  //
  ardsock = socket(AF_INET, SOCK_DGRAM, 0);

  // ensure the socket opened correctly
  //
  if (ardsock == -1)
    {
      printf("Could not create Arduino socket\n. :(");
      return(0);
    }
  else printf("Arduino Socket Opened Successfully!!! :)\n");

  // fill the structure with relevent info about the arduino socket
  //
  ARDSRVR.sin_family = AF_INET;
  ARDSRVR.sin_port = htons(ARD_CTRL_PORT);
  if (inet_aton(ARD_CTRL_IP, &ARDSRVR.sin_addr) == 0)
    {
      printf("Cannot connect to %s.\n", ARD_CTRL_IP);
      return(0);
    }

  // create the UDP  server socket object
  //
  mysock = socket(AF_INET , SOCK_DGRAM , 0);

  // ensure the socket opened correctly
  //
  if (mysock == -1)
    {
      printf("Could not create server socket\n. :(");
    }
  else printf(" Server Socket Opened Successfully!!! :)\n");

  // fill the structure with details about the server socket
  SRVRADDR.sin_family = AF_INET; // specify IPv4 protocol
  SRVRADDR.sin_port = htons(SRVRPORT); // servers port 5555
  SRVRADDR.sin_addr.s_addr = htonl(0xC0A80001); //servers IP is 192.168.0.1

  //bind socket to port
  if( bind(mysock , (const struct sockaddr*) &SRVRADDR, sizeof(SRVRADDR) ) == -1)
    {
      printf("Error: Couldn't bind to socket. Program Terminated\n.");
      return 0;
    }

  printf("Waiting for data...\n");
  //keep listening for data
  int i = 0;
  while(1)
    {

      //try to receive some data, this is a blocking call
      //

      if ((recv_len = recvfrom(mysock, recvbuf, RECVBUFLEN, 0, (struct sockaddr *) &CLIENTADDR, (unsigned int *) &slen)) == -1)
	{
	  printf("Error: recvfrom()\n");
	  return (0);
	}
      //print details of the client/peer and the data received
      //
      printf("Received packet from %s:%d\n", inet_ntoa(CLIENTADDR.sin_addr), ntohs(CLIENTADDR.sin_port));
      printf("%u, %u, %u, %u, %f\n" , *ch0currRes, *ch1currRes, *ch2currRes, *ch3currRes, *timestamp);

      /* ==========================================================  *\
       *                                                             *
       *    *
       * ---------------- Calculations and Control ----------------  *
       *    *
       *     *
       /*=========================================================  */

      // this is the old method
      //
      if ((circ1.count) == N)
	{
	  CircBuffPut(cb0, ch0currRes);
	  CircBuffGet(cb0, &ch0prevRes);
	  calculateRMS(rms1, ch0currRes, &ch0prevRes);
	  *RMS_res0 = RMS0.rms_value * 65.5367;
	  /*
            Calculate_AVGPower(pow01, ch1currRes, ch0currRes, &ch1prevRes, &ch0prevRes);
            *avgpow01 = POW01.AVG_pow;
            Calculate_QPower(pow01, &RMS0.rms_value, &RMS1.rms_value);
            *qpow01 = POW01.Q_pow;
            */
	}
      else
	{
	  CircBuffPut(cb0, ch0currRes);
	  calculateRMS(rms0, ch0currRes, &zero);
	  *RMS_res0 = RMS0.rms_value * 65.5367;
	  /*
            Calculate_AVGPower(pow01, ch1currRes, ch0currRes, &zero, &zero);
            *avgpow01 = POW01.AVG_pow;
            Calculate_QPower(pow01, &RMS0.rms_value, &RMS1.rms_value);
            *qpow01 = POW01.Q_pow;
            */

	}
      printf("RMS Value Old Calculation: %f\n", *RMS_res0);

      Verr_new = (VSET - *RMS_res0); // calculate error signal
      printf("%f   %f\n", Verr_new, Verr_old);
      new_d = old_d + (int)(kp * (Verr_new - Verr_old)) + (int)((ki * (Verr_new + Verr_old)/2) * Ts); // PI control
      Verr_old = Verr_new; // store the most current error signal
      old_d = new_d; // store the most recent duty cycle
      *sendbuf = (char) new_d & 0x00FF;
      //*sendbuf = (char) DSET;
      // send the pwm signal to the arduino
      //
      printf("new D: %u\n", new_d & 0x00FF);

      if((i = sendto(ardsock, sendbuf, SENDBUFLEN, 0, (struct sockaddr *) &ARDSRVR, slen)) == -1)
	{
	  printf("Error: sendto()\n");
	  return(-1);
	}

      printf("Data sent to Arduino: %u\n", (*sendbuf & 0x00FF));
    }
  return(0);
}
