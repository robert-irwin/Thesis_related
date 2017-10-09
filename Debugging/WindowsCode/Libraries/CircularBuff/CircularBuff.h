/*
 * CircularBuff.h
 *
 *  Created on: Jun 13, 2016
 *  Author: Robert Irwin
 */

#ifndef CIRCULARBUFF_H_
#define CIRCULARBUFF_H_

#include	<stdio.h>
#include	<stdint.h>
typedef struct CircularBuff{
	uint16_t * buffer;
	uint16_t * headptr;
	uint16_t * tailptr;
	uint16_t * buffbegin;
	uint16_t * buffend;
	int capacity;
	int count;
}CircularBuff;

/* Name: 		CircBuffInit
 *
 * Description: Initializes a circular buffer with number of elements
 * 				defined by 'capacity' where each element is type 'int'.
 */
void CircBuffInit(CircularBuff *cb, int capacity);

/* Name: 		CircBuffPut
 * Description: Inserts data pointed to by 'val' to the buffer at the point currently marked by
 * 				the 'headptr' variable.
 */
void CircBuffPut(CircularBuff *cb, uint16_t * val);

/* Name:		CircBuffGet
 * Description: Takes an int pointer, 'tail', and stores
 * 				the current value of the tail of the circular
 * 				buffer 'cb' into the input argument 'tail'.
 */
void CircBuffGet(CircularBuff *cb, uint16_t * tail);

/* Name:		GetOld2New
 * Description:	Takes the pointer to the circular buffer and a uint16_t pointer to an array
 * 				with capacity elements. This function will store the capacity elements
 * 				starting with the element currently pointed to by the tailptr. The tailptr
 * 				will be incremented by one after the function ends.
 */
void GetOld2New(CircularBuff * cb, uint16_t * vec);

#endif /* CIRCULARBUFF_H_ */

