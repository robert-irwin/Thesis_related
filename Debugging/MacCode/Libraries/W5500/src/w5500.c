/*
 * w5500.c
 *
 *  Created on: Jun 30, 2016
 *      Author: robertirwin
 */

# include 	"AtMega328P_SPI.h"
# include 	"w5500.h"
# include 	<string.h>
# include	<stdint.h>


/* Name:		w5500packetInit
 * Description: Inputs are the pointer to the w500 packet structure, and the desired
 * 				maximum length of the data in bytes. The structure is initialized
 * 				according to these values
 */
void w5500packetInit(w5500packet * pac, int maxdatalen)
{
	pac->packet = malloc((maxdatalen + 2 + 1) * sizeof(char)); // data + addrphase + control phase
	pac->recvbuffer = malloc((maxdatalen + 2 + 1) * sizeof(char));
	pac->recvpntr = pac->recvbuffer + OFFSETADDR_LEN + CNTRLBIT_LEN;
	pac->addrpntr = pac->packet;
	pac->controlpntr = pac->addrpntr + OFFSETADDR_LEN;
	pac->datapntr = pac->controlpntr + CNTRLBIT_LEN;
	pac->datalen = 0;
	pac->packetlen = 0; // will be 3 + datalen (bytes)
	pac->MAXDATALEN = maxdatalen;
}

/* Name:		w5500packetWriteConstruct
 * Description:	Inputs are the pointer to the packet structure, the desired offset adress,
 * 				the desired control bits, the data, and the length of data to be sent.
 */
void w5500WritepacketConstruct(w5500packet * pac, char * offaddr, char cntrlbits, char * data, int len)
{
	pac->datalen = len;
	pac->packetlen = pac->datalen + OFFSETADDR_LEN + CNTRLBIT_LEN;
	*(pac->addrpntr+1) = *offaddr; // set address phase
	*(pac->addrpntr) = *(offaddr+1);
	*(pac->controlpntr) = (cntrlbits << BSB0) | (1 << RWB); // control phase
	memcpy(pac->datapntr, data, len); // data phase

}

/* Name:		w5500ReadPacketConstruct
 * Description: Inputs are the pointer to the w5500 packet structure, the offset address, the
 * 				control bits, and the number of bytes to be read. The bytes that are read are
 * 				stored in the member of the w5500packet structure called 'packet'.
 */
void w5500ReadpacketConstruct(w5500packet * pac, char * offaddr, char cntrlbits, int len)
{
	pac->datalen = len;
	pac->packetlen = pac->datalen + OFFSETADDR_LEN + CNTRLBIT_LEN;
	*(pac->addrpntr+1) = *offaddr; // set address phase
	*(pac->addrpntr) = *(offaddr+1);
	*(pac->controlpntr) = (cntrlbits << BSB0); // control phase
}

/* Name: 		w5500SourceHWConfig
 * Description:	Inputs are the pointer to w5500 packet structure, and the pointers
 * 				to the character arrays containing the default gateway address,
 * 				the subnet mask, the W5500 hardware address and the desired IP address
 * 				of the W5500. This function initializes the aforementioned parameters
 * 				of the W5500.
 */
void w5500SourceHWConfig(w5500packet * pac,char * dGate, char * subMask, char * sHWADD, char * sIP )
{
	uint16_t curroffset = WIZC_GAR0;
	char * offset = malloc(2);
	offset = (char *) &curroffset;
	// construct the packet that configures the default gateway
	//
	w5500WritepacketConstruct(pac, offset, CB_TAIL_COMMONREG_RD_SEQ, dGate, DGATE_LEN);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// construct the packet that configures the subnet mask
	//
	curroffset = WIZC_SUBR0;
	w5500WritepacketConstruct(pac, offset, CB_TAIL_COMMONREG_RD_SEQ, subMask, SUBMASK_LEN);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// construct the packet that configures the source hardware address
	//
	curroffset = WIZC_SHAR0;
	w5500WritepacketConstruct(pac, offset, CB_TAIL_COMMONREG_RD_SEQ, sHWADD, HWADD_LEN);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// construct the packet that configures the source hardware address
	//
	curroffset = WIZC_SIPR0;
	w5500WritepacketConstruct(pac, offset, CB_TAIL_COMMONREG_RD_SEQ, sIP, IP_LEN);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

}

/* Name:		w5500UDPSockConfig
 * Description:	Inputs are the pointer to the w5500packet structure, and the pointers
 * 				to the character arrays containing the source port, destination
 * 				hardware address, destination IP address, and
 * 				destination port. This function initializes the aforementioned
 * 				parameters and returns a character which represents the socket.
 */
char w5500UDPSockConfig(w5500packet * pac, char * sPort, char * dHWAdd, char * dIP, char * dPort)
{
	int curroffset = WIZS_SR;
	char * offset = (char *) &curroffset;
	char currsock = WIZC_SOCK0;
	char nosock = 0;
	char specUDP = Sn_MR_UDP;
	char openSock = Sn_CR_OPEN;
	// determine the status of the current socket
	//
	w5500ReadpacketConstruct(pac, offset, currsock, 1);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	// read the socket status from the data buffer and repeat until
	// an open socket is found
	while ( !(*(pac->recvpntr) == SOCK_CLOSED))
	{
		if (currsock == WIZC_SOCK7) // no open sockets, return 0
			return nosock;
		// try the next socket
		currsock += WIZC_NEXTSOCK;
		// determine the status of the current socket
		//
		w5500ReadpacketConstruct(pac, offset, currsock, 1);
		SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	}
	// now that we have found an open socket, open it in UDP mode
	curroffset = WIZS_PORT0; //source port register
	w5500WritepacketConstruct(pac, offset, currsock, sPort, PORT_LEN); //conf source port
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	curroffset = WIZS_DHAR0; // destination hardware address register
	w5500WritepacketConstruct(pac, offset, currsock, dHWAdd, HWADD_LEN); //conf dest MAC address
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	curroffset = WIZS_DIPR0; // destination IP address register
	w5500WritepacketConstruct(pac, offset, currsock, dIP, IP_LEN); //conf dest IP address
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	curroffset = WIZS_DPORT0; // destination port register
	w5500WritepacketConstruct(pac, offset, currsock, dPort, PORT_LEN); //conf dest port
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// the socket is now configured, open it in UDP mode
	curroffset = WIZS_MR; // mode register
	w5500WritepacketConstruct(pac, offset, currsock, &specUDP, 1); //specify UDP protocol
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	curroffset = WIZS_CR; // command register
	w5500WritepacketConstruct(pac, offset, currsock, &openSock, 1); //specify UDP protocol
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	return currsock;
}

/* Name:		w5500UDPServerSockConfig
 * Description:	Inputs are the pointer to the w5500packet structure, and the pointer
 * 				to the character array containing the source port. This function initializes
 * 				the aforementioned parameters and returns a character which represents
 * 				the socket.
 */
char w5500UDPServerSockConfig(w5500packet * pac, char * sPort)
{
	int curroffset = WIZS_SR;
	char * offset = (char *) &curroffset;
	char currsock = WIZC_SOCK0;
	char nosock = 0;
	char specUDP = Sn_MR_UDP;
	char openSock = Sn_CR_OPEN;
	// determine the status of the current socket
	//
	w5500ReadpacketConstruct(pac, offset, currsock, 1);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	// read the socket status from the data buffer and repeat until
	// an open socket is found
	while ( !(*(pac->recvpntr) == SOCK_CLOSED))
	{
		if (currsock == WIZC_SOCK7) // no open sockets, return 0
			return nosock;
		// try the next socket
		currsock += WIZC_NEXTSOCK;
		// determine the status of the current socket
		//
		w5500ReadpacketConstruct(pac, offset, currsock, 1);
		SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	}

	// now that we have found an open socket, open it in UDP mode
	curroffset = WIZS_PORT0; //source port register
	w5500WritepacketConstruct(pac, offset, currsock, sPort, PORT_LEN); //conf source port
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// the socket is now configured, open it in UDP mode
	curroffset = WIZS_MR; // mode register
	w5500WritepacketConstruct(pac, offset, currsock, &specUDP, 1); //specify UDP protocol
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	curroffset = WIZS_CR; // command register
	w5500WritepacketConstruct(pac, offset, currsock, &openSock, 1); //specify UDP protocol
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	return currsock;
}

/* Name:		w5500UDPsend
 * Description:	Takes the w5500 packet structure, the socket ID, the data to be sent,
 * 				and the length of the data in bytes. This function sends 'data' to the
 * 				destination configured when calling w5500UDPSockConfig
 */
void w5500UDPsend(w5500packet * pac, char sockID, char * data, int len)
{
	int curroffset = WIZS_TX_RD0; // address containing the address of the beginning of tx buff
	uint16_t txBuffBegin = 0; // holds the address of the head of the tx buffer
	uint16_t txBuffINC = 0; //holds the updated address of the tail of the tx buffer
	uint16_t temp = 0;
	char * updateTXbuff = (char *) &txBuffINC;
	char * offset = (char *) &curroffset;
	char sendcmd = Sn_CR_SEND;
	char selectTXbuff = sockID + WIZC_TX_BUFF;

	// find the starting point of the tx buffer
	//
	w5500ReadpacketConstruct(pac, offset, sockID, 2);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	txBuffBegin =(uint16_t) (*(pac->recvpntr+1) | (*(pac->recvpntr) << 8)); // beginning of tx buffer
	offset = (char *) &txBuffBegin;

	// put the data in the transmit buffer
	//
	w5500WritepacketConstruct(pac, offset, selectTXbuff, data, len);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// increment the tx_buf_WR to the end of the data to be transmitted
	//
	temp = txBuffBegin + (uint16_t)len;
	txBuffINC = (temp << 8) | (temp >> 8);

	curroffset = WIZS_TX_WR0;
	offset = (char *) &curroffset;
	w5500WritepacketConstruct(pac, offset, sockID, updateTXbuff, 2);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	// now send the data
	//
	curroffset = WIZS_CR;
	w5500WritepacketConstruct(pac, offset, sockID, &sendcmd, 1);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

}

/* Name: 		w5500UDPrecv
 * Description: Takes the w5500 packet structure, the socket ID, and the buffer the data
 * is to be stored in. Returns the amount of data received in bytes.
 */
int w5500UDPrecv(w5500packet * pac, char sockID, char * recvbuf)
{
	int curroffset = 0;
	uint16_t rxBuffBegin = 0; // holds the address of the head of the rx buffer
	uint16_t rxBuffINC = 0; //holds the updated address of the tail of the rx buffer
	uint16_t temp = 0;
	char * updateRXbuff = (char *) &rxBuffINC;
	char * offset = (char *) &curroffset;
	char recvcmd = Sn_CR_RECV;
	uint16_t rxBufsize = 0;
	char selectRXbuff = sockID + 2;

	curroffset = WIZS_RX_RD0; // address containing the address of the beginning of rx buff
	w5500ReadpacketConstruct(pac, offset, sockID, 2);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	rxBuffBegin = (uint16_t) (*(pac->recvpntr+1) | (*(pac->recvpntr) << 8)); // beginning of rx buffer

	// determine size of data received
	//
	curroffset = WIZS_RX_RSR0;
	w5500ReadpacketConstruct(pac, offset, sockID, 2);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);
	rxBufsize = (uint16_t) (*(pac->recvpntr+1) | (*(pac->recvpntr) << 8)); // number of bytes in rx buf

	// if we receive no data exit this function
	//
	if (rxBufsize == 0) return(0);

	// read the data in the buffer
	//
	curroffset = rxBuffBegin;
	w5500ReadpacketConstruct(pac, offset, selectRXbuff, (int) rxBufsize);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// move the data to the users receive buffer
	//
	memmove(recvbuf, pac->recvpntr, rxBufsize);

	// increment the Socket Read Data Register accordingly
	//
	temp = rxBuffBegin + rxBufsize;
	rxBuffINC = (temp << 8) | (temp >> 8);
	curroffset = WIZS_RX_RD0;
	w5500WritepacketConstruct(pac, offset, sockID, updateRXbuff, 2);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	// finish the recv process by notifying the w5500 about the
	//
	curroffset = WIZS_CR;
	w5500WritepacketConstruct(pac, offset, sockID, &recvcmd, 1);
	SPI328P_MasterTransaction(pac->packet, pac->packetlen, pac->recvbuffer);

	return(rxBufsize);
}
