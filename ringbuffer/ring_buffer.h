
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <stddef.h>
#define  MAXRINGBUFFERSIZE 64


typedef struct {
	uint8_t * buffer;
	int head;
	int tail;
	int max_size; // max size of RB
	int max_length; // max size of you want hold before emptying
	volatile int buffer_count;

} ring_buffer_t;


int ring_buffer_get(ring_buffer_t *c, uint8_t *data);
int ring_buffer_put(ring_buffer_t *c, uint8_t data);
int ring_buffer_full(ring_buffer_t *c);
int ring_buffer_has_data(ring_buffer_t *c);

#endif /* __RING_BUFFER_H__ */