/**
 * @file		w5500.h
 * @brief		W5500 HAL Header File.
 * This is used by socket.c
 * @version	1.0
 * @date		2013/02/22
 * @par Revision
 *			2013/02/22 - 1.0 Release
 * @author
 * \n\n @par Copyright (C) 2013 WIZnet. All rights reserved.
 */

#ifndef  _W5500_H_
#define  _W5500_H_

//#include "Types.h"

// NOTE** This library has function calls that rely on the AtMega328P_SPI.h header
//
# include 	"AtMega328P_SPI.h"
# include	<stdlib.h>

// ------------------define data packet lengths (bytes)---------------
#define OFFADDRLEN 2
#define CONTROLLEN 1

#define IINCHIP_ISR_DISABLE()  __disable_fault_irq()
#define IINCHIP_ISR_ENABLE() __enable_fault_irq()

//----------------------------- W5500 Common Registers : WIZC_Reg -----------------------------
#define WIZC_MR                          (0x0000)    /** @brief Mode Register address */
#define WIZC_GAR0                        (0x0001)    /** @brief Gateway IP Register address */
#define WIZC_SUBR0                       (0x0005)    /** @brief Subnet mask Register address */
#define WIZC_SHAR0                       (0x0009)    /** @brief Source MAC Register address */
#define WIZC_SIPR0                       (0x000F)    /** @brief Source IP Register address */
#define WIZC_INTLEVEL0                   (0x0013)    /** @brief set Interrupt low level timer register address */
#define WIZC_INTLEVEL1                   (0x0014)
#define WIZC_IR                          (0x0015)    /** @brief Interrupt Register */
#define WIZC_IMR                         (0x0016)    /** @brief Interrupt mask register */
#define WIZC_SIR                         (0x0017)    /** @brief Socket Interrupt Register */
#define WIZC_SIMR                        (0x0018)    /** @brief Socket Interrupt Mask Register */
#define WIZC_RTR0                        (0x0019)    /** @brief Timeout register address( 1 is 100us ) */
#define WIZC_RCR                         (0x001B)    /** @brief Retry count reigster */
#define WIZC_PTIMER                      (0x001C)    /** @briefPPP LCP Request Timer register  in PPPoE mode */
#define WIZC_PMAGIC                      (0x001D)    /** @brief PPP LCP Magic number register  in PPPoE mode */
#define WIZC_PHA0                        (0x001E)    /** @brief PPP Destination MAC Register address */
#define WIZC_PSID0                       (0x0024)    /** @brief PPP Session Identification Register */
#define WIZC_PMR0                        (0x0026)    /** @brief PPP Maximum Segment Size(MSS) register */
#define WIZC_UIPR0                       (0x0028)    /** @brief Unreachable IP register address in UDP mode */
#define WIZC_UPORT0                      (0x002C)    /** @brief Unreachable Port register address in UDP mode */
#define WIZC_PSTATUS                     (0x002E)    /** @brief PHY Status Register */
// Reserved			         (0x002F)
// Reserved			         (0x0030)
// Reserved			         (0x0031)
// Reserved			         (0x0032)
// Reserved			         (0x0033)
// Reserved			         (0x0034)
// Reserved			         (0x0035)
// Reserved			         (0x0036)
// Reserved			         (0x0037)
// Reserved			         (0x0038)
#define WIZC_VERSIONR                    (0x0039)    /** @brief chip version register address */


//----------------------------- W5500 Socket Registers : WIZS_Reg -----------------------------
#define CH_SIZE                       (0x0100)    /** @brief  size of each channel register map */

#define WIZS_MR                       (0x0000)    /** @brief socket Mode register */
#define WIZS_CR                       (0x0001)    /** @brief channel Sn_CR register */
#define WIZS_IR                       (0x0002)    /** @brief channel interrupt register */
#define WIZS_SR                       (0x0003)    /** @brief channel status register */
#define WIZS_PORT0                    (0x0004)    /** @brief source port register */
#define WIZS_DHAR0                    (0x0006)    /** @brief Peer MAC register address */
#define WIZS_DIPR0                    (0x000C)    /** @brief Peer IP register address */
#define WIZS_DPORT0                   (0x0010)    /** @brief Peer port register address */
#define WIZS_MSSR0                    (0x0012)    /** @brief Maximum Segment Size(Sn_MSSR0) register address */
// Reserved			      (0x0014)
#define WIZS_TOS                      (0x0015)    /** @brief IP Type of Service(TOS) Register */
#define WIZS_TTL                      (0x0016)    /** @brief IP Time to live(TTL) Register */
// Reserved			      (0x0017)
// Reserved			      (0x0018)
// Reserved			      (0x0019)
// Reserved			      (0x001A)
// Reserved			      (0x001B)
// Reserved			      (0x001C)
// Reserved			      (0x001D)
#define WIZS_RXMEM_SIZE               (0x001E)    /** @brief Receive memory size reigster */
#define WIZS_TXMEM_SIZE               (0x001F)    /** @brief Transmit memory size reigster */
#define WIZS_TX_FSR0                  (0x0020)    /** @brief Transmit free memory size register */
#define WIZS_TX_RD0                   (0x0022)    /** @brief Transmit memory read pointer register address */
#define WIZS_TX_WR0                   (0x0024)    /** @brief Transmit memory write pointer register address */
#define WIZS_RX_RSR0                  (0x0026)    /** @brief Received data size register */
#define WIZS_RX_RD0                   (0x0028)    /** @brief Read point of Receive memory */
#define WIZS_RX_WR0                   (0x002A)    /** @brief Write point of Receive memory */
#define WIZS_IMR                      (0x002C)    /** @brief socket interrupt mask register */
#define WIZS_FRAG                     (0x002D)    /** @brief Fragment field value in IP header register */
#define WIZS_KPALVTR                  (0x002F)    /** @brief Keep Alive Timer register */
#define WIZS_TSR                      (0x0030)    /** @brief Timer Status register */


//----------------------------- W5500 Register values  -----------------------------
/*
 @brief Sn_TSR Values
 */
#define TS_SEND_ACK                 (0x04)
#define TS_WAIT_ACK                 (0x02)
#define TS_DISABLE                  (0x01)

/* MODE register values */
#define MR_RST                       0x80     //< reset */
#define MR_WOL                       0x20     //< Wake on Lan */
#define MR_PB                        0x10     //< ping block */
#define MR_PPPOE                     0x08     //< enable pppoe */
#define MR_MACRAW_NOSIZECHK          0x04     //< enbale MACRAW NO SIZE CHECHK */
#define MR_UDP_FORCE_ARP             0x02     //< enbale UDP_FORCE_ARP CHECHK */

/* IR register values */
#define IR_CONFLICT                  0x80     //< check ip confict */
#define IR_UNREACH                   0x40     //< get the destination unreachable message in UDP sending */
#define IR_PPPoE                     0x20     //< get the PPPoE close message */
#define IR_MAGIC                     0x10     //< get the magic packet interrupt */
#define IR_SOCK(ch)                  (0x01 << ch) //< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE                  0x00     //< unused socket */
#define Sn_MR_TCP                    0x01     //< TCP */
#define Sn_MR_UDP                    0x02     //< UDP */
#define Sn_MR_IPRAW                  0x03     //< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW                 0x04     //< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE                  0x05     //< PPPoE */
#define Sn_MR_UNIBLOCK               0x10     //< Unicast Block in UDP Multicating*/
#define Sn_MR_ND                     0x20     //< No Delayed Ack(TCP) flag */
#define Sn_MR_BROADBLOCK             0x40     //< Broadcast blcok in UDP Multicating */
#define Sn_MR_MULTI                  0x80     //< support UDP Multicating */

/* Sn_MR values on MACRAW MODE */
#define Sn_MR_MAWRAW_BCASTBLOCK      0xC0     //< support Broadcasting On MACRAW MODE */
#define Sn_MR_MAWRAW_MCASTBLOCK      0xA0     //< support IPv4 Multicasting On MACRAW MODE */
#define Sn_MR_MAWRAW_IPV6BLOCK       0x90     //< support IPv6 Multicasting On MACRAW MODE */
#define Sn_MR_MAWRAW_BCASTMCAST      0xE0     //< support Broadcasting On MACRAW MODE */
#define Sn_MR_MAWRAW_BCASTIPV6       0xD0     //< support Broadcasting On MACRAW MODE */
#define Sn_MR_MAWRAW_MCASTIPV6       0xB0     //< support Broadcasting On MACRAW MODE */
//#define Sn_MR_MAWRAW_MFENALE       0x10     //< support MAC Fileter Enable On MACRAW MODE */

/* Sn_CR values */
#define Sn_CR_OPEN                   0x01     //< initialize or open socket */
#define Sn_CR_LISTEN                 0x02     //< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT                0x04     //< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON                 0x08     //< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE                  0x10     //< close socket */
#define Sn_CR_SEND                   0x20     //< update txbuf pointer, send data */
#define Sn_CR_SEND_MAC               0x21     //< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP              0x22     //<  send keep alive message */
#define Sn_CR_RECV                   0x40     //< update rxbuf pointer, recv data */

#ifdef __DEF_IINCHIP_PPP__
   #define Sn_CR_PCON                0x23
   #define Sn_CR_PDISCON             0x24
   #define Sn_CR_PCR                 0x25
   #define Sn_CR_PCN                 0x26
   #define Sn_CR_PCJ                 0x27
#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
   #define Sn_IR_PRECV               0x80
   #define Sn_IR_PFAIL               0x40
   #define Sn_IR_PNEXT               0x20
#endif
#define Sn_IR_SEND_OK                0x10     //< complete sending */
#define Sn_IR_TIMEOUT                0x08     //< assert timeout */
#define Sn_IR_RECV                   0x04     //< receiving data */
#define Sn_IR_DISCON                 0x02     //< closed socket */
#define Sn_IR_CON                    0x01     //< established connection */

/* Sn_SR values */
#define SOCK_CLOSED                  0x00     //< closed */
#define SOCK_INIT                    0x13     //< init state */
#define SOCK_LISTEN                  0x14     //< listen state */
#define SOCK_SYNSENT                 0x15     //< connection state */
#define SOCK_SYNRECV                 0x16     //< connection state */
#define SOCK_ESTABLISHED             0x17     //< success to connect */
#define SOCK_FIN_WAIT                0x18     //< closing state */
#define SOCK_CLOSING                 0x1A     //< closing state */
#define SOCK_TIME_WAIT               0x1B     //< closing state */
#define SOCK_CLOSE_WAIT              0x1C     //< closing state */
#define SOCK_LAST_ACK                0x1D     //< closing state */
#define SOCK_UDP                     0x22     //< udp socket */
#define SOCK_IPRAW                   0x32     //< ip raw mode socket */
#define SOCK_MACRAW                  0x42     //< mac raw mode socket */
#define SOCK_PPPOE                   0x5F     //< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP                   0        //< Dummy for IP */
#define IPPROTO_ICMP                 1        //< Control message protocol */
#define IPPROTO_IGMP                 2        //< Internet group management protocol */
#define IPPROTO_GGP                  3        //< Gateway^2 (deprecated) */
#define IPPROTO_TCP                  6        //< TCP */
#define IPPROTO_PUP                  12       //< PUP */
#define IPPROTO_UDP                  17       //< UDP */
#define IPPROTO_IDP                  22       //< XNS idp */
#define IPPROTO_ND                   77       //< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW                  255      //< Raw IP packet */

/* Control Bits */
#define CB_SOCK_NUM                  0xE0        //< 3bits : Socket[0-2] */
#define CB_MEM_SEL                   0x10        //< 1bit  : Memory Selection */
#define CM_MEM_DM                    0xE0        /** Memory Direct Access Mode **/
#define CB_RXBUF_SOCKREG_SEL         0x08        //< 1bit  : Socket RX Buffers or uint8 Registers Selection */

#define CB_WRITE_EN                  0x04        //< 1bit  : Write Enable */
#define CB_SEQ_EN                    0x02        //< 1bit  : SEQ Enable */

// < SPI Control Byte Structure for W5500 >
//
// |            Control            | : Byte
// =================================
// |  Block Selector   |Instruction| : Bits
// ---------------------------------
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// ---------------------------------
// |Socket Num |   Control Tail    | : Meaning of Bits
// ---------------------------------
// * By way of exception, Control[7:3] bits '00000' choose the Common registers.

#define CB_TAIL_COMMONREG_RD_SEQ      0x00
#define CB_TAIL_COMREG_RD             0x01
#define CB_TAIL_COMMONREG_WR_SEQ      0x04
#define CB_TAIL_COMREG_WR             0x05

#define CB_TAIL_SOCKETREG_RD_SEQ      0x08
#define CB_TAIL_SOCKETREG_RD          0x09
#define CB_TAIL_SOCKETREG_WR_SEQ      0x0C
#define CB_TAIL_SOCKETREG_WR          0x0D

#define CB_TAIL_TXBUF_RD_SEQ          0x10
#define CB_TAIL_TXBUF_RD              0x11
#define CB_TAIL_TXBUF_WR_SEQ          0x14
#define CB_TAIL_TXBUF_WR              0x15

#define CB_TAIL_RXBUF_RD_SEQ          0x18
#define CB_TAIL_RXBUF_RD              0x19
#define CB_TAIL_RXBUF_WR_SEQ          0x1C
#define CB_TAIL_RXBUF_WR              0x1D


//----------------------------- Windowfull Control Values  -----------------------------
#define WINDOWFULL_FLAG_ON		1
#define WINDOWFULL_FLAG_OFF		0
#define WINDOWFULL_MAX_RETRY_NUM	3
#define WINDOWFULL_WAIT_TIME		1000

// --------------- Define Control phase register description ------------------------
#define OM0 0
#define OM1 1
#define RWB 2
#define BSB0 3
#define BSB1 4
#define BSB2 5
#define BSB3 6
#define BSB4 7

// ----------------- Define the socket register identifiers -------------------------
#define WIZC_SOCK0 0x01
#define WIZC_SOCK1 0x05
#define WIZC_SOCK2 0x09
#define WIZC_SOCK3 0x0D
#define WIZC_SOCK4 0x11
#define WIZC_SOCK5 0x15
#define WIZC_SOCK6 0x19
#define WIZC_SOCK7 0x1D
#define WIZC_NEXTSOCK 4
#define WIZC_TX_BUFF 1
#define WIZC_RX_BUFF 2

// ----------------------Define Useful Constants for packet Sizing----------------------
#define OFFSETADDR_LEN 2
#define CNTRLBIT_LEN 1
#define DGATE_LEN 4
#define SUBMASK_LEN 4
#define HWADD_LEN 6
#define IP_LEN 4
#define PORT_LEN 2
// ---------- Define a structure to contain the SPI packet to be sent to the W5500------
typedef struct w5500packet
{
	char * packet;
	char * recvbuffer;
	char * recvpntr;
	char * addrpntr;
	char * controlpntr;
	char * datapntr;
	int datalen;
	int packetlen;
	int MAXDATALEN;
}w5500packet;

// -----------------------------------------------------------------------------------
// ----------------------- ***Function declarations*** -------------------------------
// -----------------------------------------------------------------------------------

/* Name:		w5500packetInit
 * Description: Inputs are the pointer to the w500 packet structure, and the desired
 * 				maximum length of the data in bytes. The structure is initialized
 * 				according to these values
 */
void w5500packetInit(w5500packet * pac, int maxdatalen);

/* Name:		w5500packetWriteConstruct
 * Description:	Inputs are the pointer to the packet structure, the desired offset adress,
 * 				the desired control bits, the data, and the length of data to be sent.
 */
void w5500WritepacketConstruct(w5500packet * pac, char * offaddr, char cntrlbits, char * data, int len);

/* Name:		w5500ReadPacketConstruct
 * Description: Inputs are the pointer to the w5500 packet structure, the offset address, the
 * 				control bits, and the number of bytes to be read. The bytes that are read are
 * 				stored in the member of the w5500packet structure called 'packet'.
 */
void w5500ReadpacketConstruct(w5500packet * pac, char * offaddr, char cntrlbits, int len);

/* Name: 		w5500SourceHWConfig
 * Description:	Inputs are the pointer to w5500 packet structure, and the pointers
 * 				to the character arrays containing the default gateway address,
 * 				the subnet mask, the W5500 hardware address and the desired IP address
 * 				of the W5500. This function initializes the aforementioned parameters
 * 				of the W5500.
 */

void w5500SourceHWConfig(w5500packet * pac,char * dGate, char * subMask, char * sHWADD, char * sIP );

/* Name:		w5500UDPSockConfig
 * Description:	Inputs are the pointer to the w5500packet structure, and the pointers
 * 				to the character arrays containing the source port, destination
 * 				hardware address, destination IP address, and
 * 				destination port. This function initializes the aforementioned
 * 				parameters and returns a character which represents the socket.
 */
char w5500UDPSockConfig(w5500packet * pac, char * sPort, char * dHWAdd, char * dIP, char * dPort);

/* Name:		w5500UDPServerSockConfig
 * Description:	Inputs are the pointer to the w5500packet structure, and the pointer
 * 				to the character array containing the source port. This function initializes
 * 				the aforementioned parameters and returns a character which represents
 * 				the socket.
 */
char w5500UDPServerSockConfig(w5500packet * pac, char * sPort);

/* Name:		w5500UDPsend
 * Description:	Takes the w5500 packet structure, the socket ID, the data to be sent,
 * 				and the length of the data in bytes. This function sends 'data' to the
 * 				destination configured when calling w5500UDPSockConfig
 */
void w5500UDPsend(w5500packet * pac, char sockID, char * data, int len);

/* Name: 		w5500UDPrecv
 * Description: Takes the w5500 packet structure, the socket ID, and the buffer the data
 * is to be stored in. Returns the amount of data received in bytes.
 */
int w5500UDPrecv(w5500packet * pac, char sockID, char * recvbuf);

#endif
