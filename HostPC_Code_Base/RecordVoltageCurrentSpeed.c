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
# include<math.h>
# include"CircularBuff.h"
# include"MatrixOps.h"

// defines pertaining to control system
//

# define N (float) 25.0 // one period
# define STEPS (float) 1024.0
# define REF (float) 5.0
# define OFFSET (float) 512.0
# define DSET (char) 132 // corresponds to 52% duty cycle. Will produce 120V open circuit
# define VSET (float) 75.0 // desired voltage at the load
# define kp (float) .005 // proportional gain
# define ki (float) 20*kp // integral gain
# define MATROWS N
# define MATCOLS 2
# define MATINIT (float) 0.001
# define Ts (float) 0.001

// defines pertaining to communications
//
# define RECVBUFLEN 12 // 2 bytes for each of the 4 signals, 4 bytes for the timestamp on the source voltage
# define SENDBUFLEN 1
# define SPEEDBUFFLEN 12
# define SRVRPORT 5555
# define SPEEDPORT 5556

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
uint16_t * speedmeas = NULL;
float * timestamp = NULL;


char recordbuf[50]; // for data recording
int main(void)
{

  /* ==========================================================  *\
   *                                                             *
   *    *
   * - Initialization of the necessary structures for Control -  *
   *    *
   *     *
   /* ==========================================================  */

  FILE * fp;
  fp = fopen("RMS.txt", "a"); // for data recording
  int x = 0;


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
  char * speedbuff = NULL;

  recvbuf = malloc(sizeof(char) * RECVBUFLEN);
  speedbuff = malloc(sizeof(char)* SPEEDBUFFLEN);
  // point the pointers inside the receive buffer
  //
  ch0currRes = (uint16_t *) recvbuf; // Vs
  ch1currRes = (uint16_t *) (recvbuf + 2); // Is
  ch2currRes = (uint16_t *) (recvbuf + 4); // Vl
  ch3currRes = (uint16_t *) (recvbuf + 6); // Il
  timestamp = (float *) (recvbuf + 8); // tVs
  speedmeas = (uint16_t *) speedbuff;


  // instantiate the structure to hold information about the server
  //
  struct sockaddr_in SRVRADDR, CLIENTADDR1, CLIENTADDR2, SPEEDADDR;

  int mysock, slen1, speedsock, slen2 = sizeof(SRVRADDR) , recv_len;

  // create the UDP  server socket object
  //
  mysock = socket(AF_INET , SOCK_DGRAM , 0);
  speedsock = socket(AF_INET , SOCK_DGRAM , 0);

  // ensure the socket opened correctly
  //
  if (mysock == -1)
    {
      printf("Could not create server socket\n. :(");
    }
  else printf(" Server Socket Opened Successfully!!! :)\n");

  if (speedsock == -1)
    {
      printf("Could not create server speed socket\n. :(");
    }
  else printf(" Server Speed Socket Opened Successfully!!! :)\n");
  // fill the structure with details about the server socket
  memset((char *)&SRVRADDR, 0, sizeof(SRVRADDR));
  memset((char *)&SPEEDADDR, 0, sizeof(SPEEDADDR));
  SRVRADDR.sin_family = AF_INET; // specify IPv4 protocol
  SRVRADDR.sin_port = htons(SRVRPORT); // servers port 5555
  SRVRADDR.sin_addr.s_addr = htonl(0xC0A80001); //servers IP is 192.168.0.1
  SPEEDADDR.sin_family = AF_INET; // specify IPv4 protocol
  SPEEDADDR.sin_port = htons(SPEEDPORT); // servers port 5555
  SPEEDADDR.sin_addr.s_addr = htonl(0xC0A80001); //servers IP is 192.168.0.1

  //bind socket to port
  if( bind(mysock , (const struct sockaddr*) &SRVRADDR, sizeof(SRVRADDR) ) == -1)
    {
      printf("Error: Couldn't bind to socket. Program Terminated\n.");
      return 0;
    }

  if( bind(speedsock , (const struct sockaddr*) &SPEEDADDR, sizeof(SPEEDADDR) ) == -1)
    {
      printf("Error: Couldn't bind to speed socket. Program Terminated\n.");
      return 0;
    }
  printf("Waiting for data...\n");
  //keep listening for data
  int i = 0;
  while(1)
    {

      //try to receive some data, this is a blocking call
      //

      if ((recv_len = recvfrom(mysock, recvbuf, RECVBUFLEN, 0, (struct sockaddr *) &CLIENTADDR1, (unsigned int *) &slen1)) == -1)
	{
	  printf("Error: recvfrom()\n");
	  return (0);
	}
      //print details of the client/peer and the data received
      //

      printf("Received packet from %s:%d\n", inet_ntoa(CLIENTADDR1.sin_addr), ntohs(CLIENTADDR1.sin_port));
      printf("%u, %u, %u, %u, %f\n" , *ch0currRes, *ch1currRes, *ch2currRes, *ch3currRes, *timestamp);

      if ((recv_len = recvfrom(speedsock, speedbuff, SPEEDBUFFLEN, 0, (struct sockaddr *) &CLIENTADDR2, (unsigned int *) &slen2)) == -1)
	{
	  printf("Error: recvfrom()\n");
	  return (0);
	}
      //print details of the client/peer and the data received
      //
      printf("Received packet from %s:%d\n", inet_ntoa(CLIENTADDR2.sin_addr), ntohs(CLIENTADDR2.sin_port));
      printf("Speed: %u\n", *speedmeas);


      /*
       * This section is for collecting data. Comment out when not recording data
       */
      //x = sprintf(recordbuf, "%f, %f, %u, %f \n", *RMS_res0, *timestamp, (d_send & 0x00FF), VatGen);
      x = sprintf(recordbuf, "%u, %u, %u, %u, %f \n", *ch0currRes, *speedmeas, *ch2currRes, *ch3currRes, *timestamp);

      fwrite(recordbuf, sizeof(char), x, fp);

    }
}
