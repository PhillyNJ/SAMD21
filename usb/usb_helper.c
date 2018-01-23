/*
 * usb_helper.c
 *
 * Created: 12/27/2017 7:34:07 AM
 *  Author: pvallone
 */ 
 #include "usb_helper.h"

 void sam_display_bmp(int x1, int y1, char *bitmap, int page){

	 struct bitmap_t bmp;
	 bmp.name = bitmap;
	 int8_t header_buff[60];
	 unsigned int byte_read;
	 volatile uint8_t lun = LUN_ID_USB;
	 int position = 0;
	 uint8_t padding = 0;
	 bool flip = false;
	 /* Mount drive */
	 memset(&fs, 0, sizeof(FATFS));
	 FRESULT res = f_mount(lun, &fs);
	 if (FR_INVALID_DRIVE == res) {
		 printf("Mount Failed!\n\r");
		 return;
	 }
	 printf("Loading \t%s\n\r", (char *)bitmap);
	 res = f_open(&file_object,(char *)bitmap, FA_OPEN_EXISTING | FA_READ);
	 if (res == FR_NOT_READY || res != FR_OK) {
		 /* LUN not ready or LUN test error */
		 printf("File open failed!\n\r");
		 f_close(&file_object);
		 return;
	 }

	 // Read the bmp header
	 res = f_read(&file_object, header_buff, sizeof header_buff, &byte_read);
	 
	 if(res != FR_OK){
		 print_fs_result(res);
		 return;
	 }
	 bmp.signiture = (header_buff[1] << 8) | header_buff[0];
	 if(bmp.signiture != 0x4D42){
		 printf("Invalid File. Not a Bitmap!\n\r");
		 return; // not a bmp
	 }
	 uint16_t height = (header_buff[25] << 24) | (header_buff[24] << 16) | (header_buff[23] << 8) | header_buff[22]; // will tell us if negative of positive
	 
	 memcpy(&bmp.offset, &header_buff[6], sizeof(bmp.offset));
	 memcpy(&bmp.height, &header_buff[22], sizeof(bmp.height));
	 memcpy(&bmp.width, &header_buff[18], sizeof(bmp.width));
	 memcpy(&bmp.offset, &header_buff[10], sizeof(bmp.offset));
	 memcpy(&bmp.numberOfPixels, &header_buff[28], sizeof(bmp.numberOfPixels));	
	 memcpy(&bmp.compression, &header_buff[30], sizeof(bmp.compression));
	 memcpy(&bmp.imageSize, &header_buff[34], sizeof(bmp.imageSize));
	 bmp.rowSize = ((bmp.width * 3 + 3) & ~3);
	 
	 if(height > -1){
		 flip = false;
		 position = f_size(&file_object) - bmp.rowSize;// calculate last row to start at (bottom-up) if height is positive
	 } else {
		 flip = true;
		 position = bmp.offset; // read from top down
	 }
	 //print_bitmap_header(&bmp);

	 if(tft_conf.orient == LANDSCAPE){
		 swap(uint32_t, bmp.width, bmp.height);
	 }
	 uint8_t val = (bmp.height / bmp.numberOfPixels) % 4; // swaps in Landscape
	 
	 if(val != 0){
		 padding = 4 - val;
	 }
	 sam_clearCs();
	
	 sam_setXY(x1, y1 + page,(bmp.width + x1) - 1, (bmp.height + y1 + page)-1);
	 	
	// printf("page: %d Calcs: x1: %d y1:%d x2: %ld y2:%ld\n", page, x1, y1 + page,(bmp.width + x1) - 1, (bmp.height + y1 + page)-1);
	 res = f_lseek(&file_object, position); // start at position - set by if height is negative or positive
	 uint8_t row_buff[bmp.rowSize]; // only read 1 row at a time

	 for(uint32_t row = 0; row < (bmp.imageSize / bmp.rowSize); row++){ // read 1 row at a time
		 
		 res = f_read(&file_object, row_buff, sizeof row_buff, &byte_read); // read 1 row
		 
		 if(res != FR_OK){
			 print_fs_result(res);
			 return;
		 }
		 if(flip){
			 position = position + bmp.rowSize; // calculate next position on disk to read
		 }else{
			 position = position - bmp.rowSize; // calculate next position on disk to read
		 }
		 
		 res = f_lseek(&file_object, position); // set next position
		 
		 if(res != FR_OK){
			 print_fs_result(res);
			 return;
		 }
		 // read the buffer and send to TFT
		 if(!flip){

			 for (uint16_t x = 0; x < (sizeof row_buff)-padding ; x += 3) {
				 
				 sam_writeData(((row_buff[x + 2] & 248) | row_buff[x + 1] >> 5), ((row_buff[x + 1] & 28) << 3 | row_buff[x] >> 3));
			 }

			 } else {
			 for (uint16_t x = (sizeof row_buff)-padding; x > 0 ; x -= 3) {
				 
				 sam_writeData(((row_buff[x + 2] & 248) | row_buff[x + 1] >> 5), ((row_buff[x + 1] & 28) << 3 | row_buff[x] >> 3));
			 }
		 }
		 
	 } // end while
	 
	 sam_setCs();
	 sam_clrXY();
	 f_close(&file_object);
	 print_bitmap_header(&bmp);
	 
 }

 void sam_display_raw(int x1, int y1, int width, int height, char *bitmap, int page){
	 
	 volatile uint8_t lun = LUN_ID_USB;
	 unsigned int byte_read;
	 
	 /* Mount drive */
	 memset(&fs, 0, sizeof(FATFS));
	 FRESULT res = f_mount(lun, &fs);
	 if (FR_INVALID_DRIVE == res) {
		 printf("Mount Failed!\n\r");
		 return;
	 }
	 printf("Loading \t%s\n\r", (char *)bitmap);
	 res = f_open(&file_object,(char *)bitmap, FA_OPEN_EXISTING | FA_READ);
	 if (res == FR_NOT_READY || res != FR_OK) {
		 /* LUN not ready or LUN test error */
		 printf("File open failed!\n\r");
		 f_close(&file_object);
		 return;
	 }
	 
	 if(tft_conf.orient == LANDSCAPE){
		 swap(uint32_t, width, height);
	 }
	 DWORD totalBytes = f_size(&file_object);
	 sam_clearCs();
	 sam_setXY(x1, y1 + page,(width + x1) - 1, (height + y1 + page)-1);
	 
	 int bytesPerRow = totalBytes / height;

	 uint8_t row_buff[bytesPerRow]; // only read 1 row at a time

	 for(int row = 0; row < height; row++){ // read 1 row at a time
		 
		 res = f_read(&file_object, row_buff, sizeof row_buff, &byte_read); // read 1 row
		 
		 if(res != FR_OK){
			 print_fs_result(res);
			 return;
		 }
		 
		 res = f_lseek(&file_object, f_tell(&file_object)); // set next position
		 
		 if(res != FR_OK){
			 print_fs_result(res);
			 return;
		 }
		 // read the buffer and send to TFT
		 
		 for (int x = 0; x < bytesPerRow ; x+=2) {
			 
			 sam_writeData(row_buff[x], row_buff[x + 1]);
			 
		 }
		 
	 } // end while
	 
	 sam_setCs();
	 sam_clrXY();
	 f_close(&file_object);
	 
 }
  
 void print_fs_result(FRESULT res){
	 
	 switch(res){
		 case FR_OK:			/* (0) Succeeded */
		 printf("(0) Succeeded");
		 break;
		 case FR_DISK_ERR:
		 printf("(1) A hard error occurred in the low level disk I/O layer");
		 break;
		 case FR_INT_ERR:
		 printf("(2) Assertion failed");
		 break;
		 case FR_NOT_READY:
		 printf("(3) The physical drive cannot work");
		 break;
		 case FR_NO_FILE:
		 printf("(4) Could not find the file");
		 break;
		 case FR_NO_PATH:
		 printf("(5) Could not find the path");
		 break;
		 case FR_INVALID_NAME:		/*  */
		 printf("(6) The path name format is invalid");
		 break;
		 case FR_DENIED:
		 printf("(7) Access denied due to prohibited access or directory full");
		 break;
		 case FR_EXIST:
		 printf("(8) Access denied due to prohibited access");
		 break;
		 case FR_INVALID_OBJECT:
		 printf("(9) The file/directory object is invalid");
		 break;
		 case FR_WRITE_PROTECTED:
		 printf("(10) The physical drive is write protected");
		 break;
		 case FR_INVALID_DRIVE:
		 printf("(11) The logical drive number is invalid");
		 break;
		 case FR_NOT_ENABLED:
		 printf("(12) The volume has no work area ");
		 break;
		 case FR_NO_FILESYSTEM:
		 printf("(13) There is no valid FAT volume");
		 break;
		 case FR_MKFS_ABORTED:
		 printf("(14) The f_mkfs() aborted due to any parameter error");
		 break;
		 case FR_TIMEOUT:
		 printf("(15) Could not get a grant to access the volume within defined period");
		 break;
		 case FR_LOCKED:
		 printf("(16) The operation is rejected according to the file sharing policy");
		 break;
		 case FR_NOT_ENOUGH_CORE:
		 printf("(17) LFN working buffer could not be allocated");
		 break;
		 case FR_TOO_MANY_OPEN_FILES:
		 printf("(18) Number of open files > _FS_SHARE");
		 break;
		 case FR_INVALID_PARAMETER:
		 printf("(19) Given parameter is invalid");
		 break;
		 default:
		 printf("Unknown Error Code %d", res);
		 break;
	 }
	 printf("\n\r");
 }
