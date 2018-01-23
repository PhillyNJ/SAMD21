/*
 * usb_helper.h
 *
 * Created: 12/27/2017 7:33:55 AM
 *  Author: pvallone
 */ 


#ifndef USB_HELPER_H_
#define USB_HELPER_H_
#include <asf.h>
#include "sam_tft.h"
#include "bitmap.h"
#define MAX_DRIVE _VOLUMES

/* FATFS variables */
void print_fs_result(FRESULT res);
FATFS fs;
FIL file_object;
char buff[256];
void sam_display_bmp(int x1, int y1, char *bitmap, int page);
void sam_display_raw(int x1, int y1, int width, int height, char *bitmap, int page);

#endif /* USB_HELPER_H_ */