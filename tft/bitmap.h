/*
 * bitmap.h
 *
 * Created: 11/30/2017 7:22:52 AM
 *  Author: pvallone
 */ 


#ifndef BITMAP_H_
#define BITMAP_H_
#include <asf.h>
struct bitmap_t  {
	
	int32_t width;
	int32_t height;	
	uint16_t offset;
	uint16_t rowSize;
	uint8_t numberOfPixels;
	uint32_t compression;
	uint32_t imageSize;
	uint16_t signiture;
	char *name;
};

void print_bitmap_header(struct bitmap_t *bmp);

#endif /* BITMAP_H_ */