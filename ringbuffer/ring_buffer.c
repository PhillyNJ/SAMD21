/*  Adopted from 
	https://codereview.stackexchange.com/questions/16468/circular-ringbuffer

*/
#include "ring_buffer.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

int ring_buffer_put(ring_buffer_t *c, uint8_t data)
{
	// next is where head will point to after this write.
	int next = c->head + 1;
	if (next >= c->max_size) {
		next = 0;
	}
	if (next == c->tail) { // check if circular buffer is full
		return -1;
	} // and return with an error.

	c->buffer[c->head] = data; // Load data and then move
	c->head = next;            // head to next data offset.
	c->buffer_count++;
	return 0;  // return success to indicate successful push.
}

int ring_buffer_get(ring_buffer_t *c, uint8_t *data)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (c->head == c->tail) // check if circular buffer is empty
	return -1;          // and return with an error

	// next is where tail will point to after this read.
	int next = c->tail + 1;
	if (next >= c->max_size)
	next = 0;
	
	*data = c->buffer[c->tail]; // Read data and then move

	c->tail = next;             // tail to next data offset.
	c->buffer_count--;
	return 0;  // return success to indicate successful pop.
}
int ring_buffer_full(ring_buffer_t *c) {
	return c->buffer_count == c->max_size - 1 ? 1 : 0;
}
int ring_buffer_has_data(ring_buffer_t *c) {
	return c->buffer_count  > 0 ? 1 : 0;
}