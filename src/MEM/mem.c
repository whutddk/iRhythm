#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"



#include "include.h"

#define NUM_BYTE_READ 4096


FIL fp;

uint8_t buf_read[NUM_BYTE_READ];

void readout_file()
{
	uint32_t num_read;

	error_num = f_open(&fp,"0:/mint_summer.mp3",FA_READ);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File open failed!\r\nstop!\r\n");
		while(1);
	}

//try to check file size here

//try to check file size here
	EMBARC_PRINTF("Start To Read!!!\r\n");
	memset( file_buff, 0, sizeof(uint8_t) * 10 * 1024 * 1024 );
	num_read = 1;

	while( num_read != 0 )
	{
		error_num = f_read(&fp,buf_read,NUM_BYTE_READ,&num_read);
		strcat(file_buff,buf_read);

	}
	EMBARC_PRINTF("File Read End\r\n");
	f_close(&fp);
}


