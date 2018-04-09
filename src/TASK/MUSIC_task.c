#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"
#define NUM_BYTE_READ 4096


FATFS fs_p;
int32_t error_num = 0;
FIL fp;
unsigned char buf_read[NUM_BYTE_READ];
// FATFS_DISKIO *fs;
void music_task()
{
	uint32_t num_read;
	EMBARC_PRINTF("MUSIC_TASK START\r\n");

	uint8_t *file_buff;
	file_buff = malloc(sizeof(uint8_t) * 10*1024*1024);
	// fs = get_fatfs_diskio(EMSK_SDCARD_0_DRVID);
	// if (error_num = disk_initialize (	EMSK_SDCARD_0_DRVID	/* Physical drive nmuber (0..) */)  == STA_NOINIT)
	// {
	// 	EMBARC_PRINTF("SD_card init fail!\r\n");
	// }
	// else
	// {
	// 	EMBARC_PRINTF("SD_card init status:%d !\r\n",error_num);
	// }
	error_num = f_mount(&fs_p,"0:/",1);

	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File f_mount failed!\r\nstop!\r\n");
		while(1);
	}
	error_num = f_open(&fp,"0:/mint_summer.mp3",FA_READ);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File open failed!\r\nstop!\r\n");
		while(1);
	}

	EMBARC_PRINTF("Start To Trace!!!\r\n");
	error_num = f_read(&fp,buf_read,NUM_BYTE_READ,&num_read);

	while( num_read!=0 )
	{
		error_num = f_read(&fp,buf_read,NUM_BYTE_READ,&num_read);
		
	}
	EMBARC_PRINTF("File Read End\r\n");
	f_close(&fp);
	while(1);
}



