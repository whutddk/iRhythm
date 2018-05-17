#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"



#include "include.h"

#define NUM_BYTE_READ 4096


FIL fp;

// uint8_t buf_read[NUM_BYTE_READ];

/***
*** Read out File Form SD Card to DDR2 10MB buff
**
**/

void readout_file(char* music_name)
{
	char filename[50] = "0:/music/";
	uint32_t num_read;
	uint8_t *fbuff_p = file_buff;
	uint32_t read_sum = 0;

	strcat(filename,music_name);
	error_num = f_open(&fp,filename,FA_READ);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File %s open failed!\r\nstop!\r\n",filename);
		while(1);
	}

	EMBARC_PRINTF("Start To Read file %s !!!\r\n",filename);
	memset( file_buff, 0, sizeof(int8_t) * 15 * 1024 * 1024 );
	num_read = 1;

	while( num_read != 0 )
	{
		cpu_lock();
		error_num = f_read(&fp,fbuff_p,NUM_BYTE_READ,&num_read);
		cpu_unlock();
		fbuff_p += num_read;
		read_sum += num_read;
		//EMBARC_PRINTF("readout %d!!!\r\n",read_sum);

		if ( gui_info.flag_next != 1)
		{
			;
		}
		else//play next song?
		{
			break;
		}
	}
	EMBARC_PRINTF("readout %d!!!\r\n",read_sum);
	EMBARC_PRINTF("File %s Read End\r\n",filename);
	f_close(&fp);
}


