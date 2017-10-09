/*
 * AtMega328P_SPI.h
 *
 *  Created on: Jun 27, 2016
 *      Author: robertirwin
 */

#ifndef ATMEGA328P_SPI_H_
#define ATMEGA328P_SPI_H_

/* Name:		SPI328P_MasterInit
 * Description:	Initializes the the data direction register of port B (SPI interface),
 * 				such that the MOSI and SCK pins are set to outputs. This function also
 * 				enables he SPI interface, and sets the SCK frequency to 1 MHz.
 */
void SPI328P_MasterInit(void);

/* Name:		SPI328P_Master_Transaction
 * Description:	Sends 'numbytes' number of bytes over the SPI interface configured as master.
 * 				This function writes then reads data. The data that is read is stored in the input
 * 				buffer 'recvbuf'.
 * 				SPI328P_MasterInit must be run prior to this function
 */
void SPI328P_MasterTransaction(char * data, int numbytes, char * recvbuf);

#endif /* ATMEGA328P_SPI_H_ */
