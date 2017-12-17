/*
 * bitmap.c
 *
 * Created: 11/30/2017 7:29:00 AM
 *  Author: pvallone
 */ 
 #include "bitmap.h"

 void print_bitmap_header(struct bitmap_t *bmp){

	printf("Offset:\t%d\n\r", bmp->offset);
	printf("Height:\t%d\n\r", bmp->height);
	printf("Width:\t\t%d\n\r", bmp->width);
	printf("Row Size:\t%d\n\r", bmp->rowSize);
	printf("Offset:\t%d\n\r", bmp->offset);
	printf("# of Pixels:\t%d\n\r", bmp->numberOfPixels);
	printf("Compression:\t%ld\n\r", bmp->compression);
	printf("Image Size:\t%ld\n\r", bmp->imageSize);
	printf("Name:\t\t%s\n\r", bmp->name);

 }