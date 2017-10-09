/*
 * CircularBuff.c
 *
 *  Created on: Jun 13, 2016
 *      Author: robertirwin
 */

#include	"CircularBuff.h"
#include	<stdlib.h>
#include	<stdint.h>
/* Name: 		CircBuffInit
 *
 * Description: Initializes a circular buffer with number of elements
 * 				defined by 'capacity + 1' where each element is type 'int'.
 */
void CircBuffInit(CircularBuff *cb, int capacity)
{
	cb->buffer = malloc((capacity + 1) * sizeof(uint16_t));
	cb->buffend = cb->buffer + (capacity);
	cb->buffbegin = cb->buffer;
	cb->headptr = cb->buffer;
	cb->tailptr = cb->buffer;
	cb->capacity = capacity;
	cb->count = 0;
}

/* Name: 		CircBuffPut
 * Description: Inserts data pointed to 'val' to the buffer at the point currently
 * 				marked by the 'headptr' variable.
 */
void CircBuffPut(CircularBuff *cb, uint16_t * val)
{
	*(cb->headptr) = *val;
	if (cb->headptr == cb->buffend)
	{
		cb->headptr = cb->buffbegin;
	}
	else (cb->headptr)++;

	if (cb->count < cb->capacity)
	{
		(cb->count)++;
	}
}


/* Name:		CircBuffGet
 * Description: Takes an int pointer, 'tail', and stores
 * 				the current value of the tail of the circular
 * 				buffer 'cb' into the input argument 'tail'.
 */
void CircBuffGet(CircularBuff *cb, uint16_t * tail)
{
	*tail = *(cb->tailptr);
	if (cb->tailptr == cb->buffend)
	{
		cb->tailptr = cb->buffbegin;
	}
	else (cb->tailptr)++;
}

/* Name:		GetOld2New
 * Description:	Takes the pointer to the circular buffer and a uint16_t pointer to an array
 * 				with capacity elements. This function will store the capacity elements
 * 				starting with the element currently pointed to by the tailptr. The tailptr
 * 				will be incremented by one after the function ends.
 */
void GetOld2New(CircularBuff * cb, uint16_t * vec)
{
	// first find where cb->tailptr will point after this function executes;
	//
	uint16_t * newtail;
	if (cb->tailptr == cb->buffend)
		newtail = cb->buffbegin;
	else newtail = cb->tailptr+1;

	// fill the vector
	//
	for (int i = 0; i < cb->capacity; i++)
	{
		CircBuffGet(cb, vec+i);
	}

	// assign the tailptr to its proper place in the buffer
	//
	cb->tailptr = newtail;
}
