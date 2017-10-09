/*
 * main.c
 *
 *  Created on: Jun 12, 2017
 *      Author: robertirwin
 */

# include <stdint.h>
# include <stdlib.h>
# include <math.h>
# include <Winsock2.h>
# include <ws2tcpip.h>
# include <stdio.h>
# include "CircularBuff.h"
# include "MatrixOps.h"

// defines pertaining to control system
//

# define N (float) 100.0 // one period
# define STEPS (float) 1024.0
# define REF (float) 5.0
# define OFFSET (float) 512.0
# define DSET (char) 30 // corresponds to 52% duty cycle. Will produce 120V open circuit
# define VSET (float) 75.0 // desired voltage at the load
# define kp (float) .005 // proportional gain
# define ki (float) 20*kp // integral gain
# define MATROWS N
# define MATCOLS 2
# define MATINIT (float) 0.001
# define Ts (float) 0.001

# define Ploss1 (unsigned int) 0 // probability of packet loss on state
# define Ploss2 (unsigned int) 0 // probability of packet loss on control
//# define BiasAttackV (float) 0 // bias voltage
# define AddOnAttackDeltaV (float) 0.0 // increase in voltage every sample
# define RandAddOnAttackMaxdevV (int) 0 // maximum random added voltage is +-V/2

// defines pertaining to communications
//
# define RECVBUFLEN 12 // 2 bytes for each of the 4 signals, 4 bytes for the timestamp on the source voltage
# define SENDBUFLEN 1
# define SPEEDBUFFLEN 12
# define SRVRPORT 5555
# define SPEEDPORT 5556
# define ARD_CTRL_IP "192.168.0.3"
# define ARD_CTRL_PORT 5050
# define LOCALloopPort 5000
# define LOCAL_IP "127.0.0.1"
# define LOCALBUFLEN 13 // 4 bytes for timestamp, RMS at gen and RMS at controller

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

// declare variables for communicating with controller arduino
//

float Verr_old = 0.0;
float Verr_new = 0.0;
float new_d = 0;
char d_send = 0.0;
float old_z = 0.0;
float new_z = 0.0;
float addonAttackV = 0.0;
int randAddonAttackV = 0;
float VatGen = 0.0;

CircularBuff circ0;
CircularBuff * cb0 = &circ0;

MyMat MAT;
MyMat * mat = &MAT;
MyMat TRANS;
MyMat * trans = &TRANS;
// temporary matrices for V,I,P,Q calculations
//
MyMat m1;
MyMat * M1 = &m1;
MyMat m3;
MyMat * M3 = &m3;
MyMat VsrcMAT;
MyMat * Vsrcmat = &VsrcMAT;
MyMat TEMPMAT;
MyMat * tempMat = &TEMPMAT;
MyMat  AVEC;
MyMat * A_vec = & AVEC;
MyMat  BVEC;
MyMat * B_vec = & BVEC;

float * row2add = NULL;
float * Vsrc_vec = NULL;
uint16_t * Vsrc_arr = NULL;

void createMatrixforPowCalc(MyMat * mat, MyMat * trans);
int bernoulli_p(unsigned int P);
int random_int(int MaxV);

char recordbuf[50]; // for data recording

// instantiate the structure to hold information about the server
//
struct sockaddr_in SRVRADDR;
struct sockaddr_in CLIENTADDR1;
struct sockaddr_in CLIENTADDR2;
struct sockaddr_in SPEEDADDR;
struct sockaddr_in LOCALLOOP;
struct sockaddr_in ARDSRVR;
#pragma comment(lib, "Ws2_32.lib")
int main(void)
{
  // Initialize winsock
  WSADATA wsa;
  printf("\nInitialising Winsock...");
  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
  {
	printf("Failed. Error Code : %d",WSAGetLastError());
	exit(EXIT_FAILURE);
  }
  printf("Initialised.\n");
  printf("Failed. Error Code : %d",WSAGetLastError());
//SOCKADDR_IN SRVRADDR, CLIENTADDR1, CLIENTADDR2, SPEEDADDR, LOCALLOOP, ARDSRVR;

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
  int q = 0;
  srand(1);
  CircBuffInit(cb0, (int) N);

  float * RMS_res0 = malloc(sizeof(float)); // variable to hold rms result

  float * initArray = malloc(MATROWS * MATCOLS * sizeof(float));

  for (int i = 0; i < (MATROWS * MATCOLS); i++)
    {
      initArray[i] = MATINIT;
    }

  MyMatInit(mat, MATROWS, MATCOLS, (void *) initArray, TYPE_FLOAT);
  free(initArray); // free the memory as the elements are now stored in mat->Matrix

  uint16_t zero = (uint16_t) OFFSET;

  for (int i = 0; i < N-1; i++)
    {
      CircBuffPut(cb0, &zero);
    }

  row2add = malloc(2 * sizeof(float));
  Vsrc_arr = malloc(N * sizeof(uint16_t));
  Vsrc_vec = malloc(N* sizeof(float));
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

  char * sendbuf = NULL;

  sendbuf = malloc(sizeof(char) * SENDBUFLEN);
  recvbuf = malloc(sizeof(char) * RECVBUFLEN);
  speedbuff = malloc(sizeof(char)* SPEEDBUFFLEN);

  *sendbuf = DSET;
  // point the pointers inside the receive buffer
  //
  ch0currRes = (uint16_t *) recvbuf; // Vs
  ch1currRes = (uint16_t *) (recvbuf + 2); // Is
  ch2currRes = (uint16_t *) (recvbuf + 4); // Vl
  ch3currRes = (uint16_t *) (recvbuf + 6); // Il
  timestamp = (float *) (recvbuf + 8); // tVs
  speedmeas = (uint16_t *) speedbuff;

  // intialize a buffer for the real time plotting packet
  //
  char * localpac = malloc(sizeof(char) * LOCALBUFLEN);


  SOCKET mysock, ardsock, speedsock;

  // create the UDP  server socket object
  //
  mysock = socket(AF_INET , SOCK_DGRAM , 0);
  speedsock = socket(AF_INET , SOCK_DGRAM , 0);
	printf("Failed. Error Code : %d",WSAGetLastError());
  // ensure the socket opened correctly
  //
  if (mysock == INVALID_SOCKET)
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
  memset((char *)&CLIENTADDR2, 0, sizeof(CLIENTADDR2));
  memset((char *)&CLIENTADDR1, 0, sizeof(CLIENTADDR1));
  memset((char *)&SPEEDADDR, 0, sizeof(SPEEDADDR));
  memset((char *)&ARDSRVR, 0, sizeof(ARDSRVR));
  SRVRADDR.sin_family = AF_INET; // specify IPv4 protocol
  SRVRADDR.sin_addr.s_addr = htonl(0xC0A80001); //servers IP is 192.168.0.1
  SRVRADDR.sin_port = htons(SRVRPORT); // servers port 5555
  SPEEDADDR.sin_family = AF_INET; // specify IPv4 protocol
  SPEEDADDR.sin_port = htons(SPEEDPORT); // servers port 5555
  SPEEDADDR.sin_addr.s_addr = htonl(0xC0A80001); //servers IP is 192.168.0.1

  unsigned int slen1 = sizeof(SRVRADDR);
  unsigned int slen2 = sizeof(SRVRADDR);
  unsigned int slen3 = sizeof(SRVRADDR);
  unsigned int recv_len = sizeof(SRVRADDR);
  ardsock = socket(AF_INET, SOCK_DGRAM, 0);
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
  ARDSRVR.sin_addr.s_addr= inet_addr(ARD_CTRL_IP);
  /*
  if (inet_pton(AF_INET, ARD_CTRL_IP, &ARDSRVR.sin_addr) == 0)
    {
      printf("Cannot connect to %s.\n", ARD_CTRL_IP);
      return(0);
    }
*/
  if (inet_addr(&ARDSRVR.sin_addr) == 0)
      {
        printf("Cannot connect to %s.\n", ARD_CTRL_IP);
        return(0);
      }
  //bind socket to port
  if( bind(mysock , (struct sockaddr*) &SRVRADDR, sizeof(SRVRADDR) ) == SOCKET_ERROR)
    {
	  printf("Bind failed with error code : %d" , WSAGetLastError());
	          exit(EXIT_FAILURE);
    }
	printf("After Bind. Error Code : %d\n",WSAGetLastError());

  if( bind(speedsock , (const struct sockaddr*) &SPEEDADDR, sizeof(SPEEDADDR) ) == -1)
    {
      printf("Error: Couldn't bind to speed socket. Program Terminated\n.");
      return 0;
    }

  printf("Waiting for data...\n");
  //keep listening for data
  int i = 0;
  int count = 0;
  float BiasAttackV = 0.0;
  while(1)
    {

      //try to receive some data, this is a blocking call
      //

      if ((recv_len = recvfrom(mysock, recvbuf, RECVBUFLEN, 0, (SOCKADDR *) &CLIENTADDR1, &slen1)) == SOCKET_ERROR)
	{
	  //printf("Error: recvfrom() voltage\n");
    	  wprintf(L"recvfrom failed with error %d  %d\n", WSAGetLastError(), mysock);
     return (0);
	}

      /*count++;
      if (count > 20000){
	Ploss1 = 0;
	BiasAttackV = 2;
      }
      if (count > 40000){
	BiasAttackV = 3;
      }
      if (count > 60000){
	BiasAttackV = 5;
      }
      */
      //print details of the client/peer and the data received
      //

      printf("Received packet from %s:%d\n", inet_ntoa(CLIENTADDR1.sin_addr), ntohs(CLIENTADDR1.sin_port));
      printf("%u, %u, %u, %u, %f\n" , *ch0currRes, *ch1currRes, *ch2currRes, *ch3currRes, *timestamp);

     if ((recv_len = recvfrom(speedsock, speedbuff, SPEEDBUFFLEN, 0, (struct sockaddr *) &CLIENTADDR2, (unsigned int *) &slen2)) == -1)
	{
    	 wprintf(L"recvfrom speed failed with error %d  %d\n", WSAGetLastError(), mysock);
	  return (0);
	}

      //print details of the client/peer and the data received
      //

      printf("Received packet from %s:%d\n", inet_ntoa(CLIENTADDR2.sin_addr), ntohs(CLIENTADDR2.sin_port));
      printf("Speed: %u\n", *speedmeas);

      // store the data in the circular buffer
      //
      CircBuffPut(cb0, ch0currRes);

      // build the row to append to the matrix
      //
      *row2add = sin(377 * (*timestamp)/1000);
      *(row2add + 1) = cos(377 * (*timestamp)/1000);

      // Append the new row to the matrix
      //
      MyMatAppendRow(mat, row2add);
      //printf("Main Matrix:\n");
      //displayMatrix(mat);

      createMatrixforPowCalc(mat, trans); // create matrix M3 (from rmsfilter8.m)

      GetOld2New(cb0, Vsrc_arr);
      // convert from 0->1023 to -2.5->2.5
      //
      for (int i = 0; i < N; i++)
	{
	  *(Vsrc_vec+i) = (float) ((float) (*(Vsrc_arr+i) - OFFSET) * (float)(REF / STEPS));
	}

      MyMatInit(Vsrcmat, N, 1, (void *) Vsrc_vec, TYPE_FLOAT);
      //printf("Voltage vector:\n");
      //displayMatrix(Vsrcmat);

      // Compute A_vec and B_vec (in reference to rmsfilter8.m)
      //
      MyMatMultiply(M3, trans, tempMat);
      MyMatMultiply(tempMat, Vsrcmat, A_vec);

      if (q == 1000)
	{
	  randAddonAttackV = random_int(RandAddOnAttackMaxdevV);
	  q = 0;
	}
      VatGen=sqrt((pow((float) *((float *)A_vec->rows[0]), 2) + pow((float) *((float *)A_vec->rows[1]), 2))/2);
      // simulate packet loss on state channel, implement holding method at DAQ unit
      if (bernoulli_p(Ploss1))
	{
	  //calculate RMS
	  //
	  *RMS_res0=sqrt((pow((float) *((float *)A_vec->rows[0]), 2) + pow((float) *((float *)A_vec->rows[1]), 2))/2);
	  *RMS_res0 = (*RMS_res0 * (float) 64.86);// 65.5367);
	  //VatGen = *RMS_res0;
	  *RMS_res0 += BiasAttackV;
	  addonAttackV += AddOnAttackDeltaV;
	  *RMS_res0 += addonAttackV;
	  randAddonAttackV = random_int(RandAddOnAttackMaxdevV);
	  *RMS_res0 += randAddonAttackV;
	}

      printf("RMS Value: %f\n", *RMS_res0);
      Verr_new = (VSET - *RMS_res0); // calculate error signal
      //printf("%f   %f\n", Verr_new, Verr_old);
      new_z = old_z + (Verr_new * Ts);
      old_z = new_z;
      new_d = (kp * Verr_new) + (ki * new_z);
      if (new_d < 0.1) new_d = 0.1;
      if (new_d > 0.9) new_d = 0.9;
      d_send = (char) (255 * new_d);
      printf("Duty Cycle: %u\n", (int) (d_send & 0x00FF));
      *sendbuf = d_send;
      /*
       * This section is for collecting data. Comment out when not recording data
       */
      x = sprintf(recordbuf, "%f, %u, %f, %u, %f \n", *RMS_res0, *speedmeas, *timestamp, (d_send & 0x00FF), VatGen);
      //x = sprintf(recordbuf, "%u, %u, %u, %u, %f \n", *ch0currRes, *speedmeas, *ch2currRes, *ch3currRes, *timestamp);

      fwrite(recordbuf, sizeof(char), x, fp);

      if (bernoulli_p(Ploss2)){
	if((i = sendto(ardsock, sendbuf, SENDBUFLEN, 0, (struct sockaddr *) &ARDSRVR, slen3)) == -1)
	  {
		wprintf(L"sendto failed with error %d  %d\n", WSAGetLastError(), mysock);
		printf("Error: sendto()\n");
	    return(-1);
	  }
	printf("Sent to %s:%d\n", inet_ntoa(ARDSRVR.sin_addr), ntohs(ARDSRVR.sin_port));
	printf("Data sent to Arduino: %u\n", (*sendbuf & 0x00FF));
      }

      // free all the memory necessary
      //
      free(trans->matrix); free(M3->matrix); free(tempMat->matrix);
      free(Vsrcmat->matrix); free(A_vec->matrix);
      free(trans->rows); free(M3->rows); free(tempMat->rows);
      free(Vsrcmat->rows); free(A_vec->rows);
      q++;
    }
  fclose(fp); // for data recording
  return(0);
}


// This function creates the matrix M3 from rmsfilter8.m
// Ensure after M3 is done being used, the memory for it is freed
//

void createMatrixforPowCalc(MyMat * mat, MyMat * trans)
{
  // find the transpose
  //
  MyMatTranspose(mat, trans);
  MyMatMultiply(trans, mat, M1);

  float factor = 0.0;
  float * M2 = malloc(4 * sizeof(float));
  M2[0] = *((float *) (M1->rows[1] + 1 * sizeof(float)));
  M2[1] = (*((float *) (M1->rows[0] + 1 * sizeof(float)))) * -1.0;
  M2[2] = (*((float *) (M1->rows[1]))) * -1.0;
  M2[3] = *((float *) (M1->rows[0]));

  factor = *((float *) (M1->rows[0])) * (*(float *)(M1->rows[1] + 1 * sizeof(float)));
  factor -= *((float *) (M1->rows[0] + 1 * sizeof(float))) * (*(float *)(M1->rows[0]));
  for (int i = 0; i < 4; i ++)
    {
      M2[i] = M2[i]/factor;
    }
  // create matrix M3
  //
  MyMatInit(M3, 2, 2, (void *) M2, TYPE_FLOAT);

  // free memory allocated for M2 and M1
  //
  free(M2); free(M1->matrix); free(M1->rows);

}

// P is the probability of a failed bernoulli trial
int bernoulli_p(unsigned int P)
{
  int high = 99;
  unsigned int randnum = 0;

  randnum = rand() % (high+1);
  if (randnum < (P)) return(0);
  else return(1);
}

int random_int(int MaxV)
{
  if (MaxV == 0) return(0);
  int randnum = 0;
  randnum = rand() % (MaxV+1);
  randnum -= (MaxV/2);
  return(randnum);

}

