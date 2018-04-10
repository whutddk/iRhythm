#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"


FATFS fs_p;



uint8_t *file_buff;
void music_task()
{
	DIR dir;
	FILINFO fileinfo;
	EMBARC_PRINTF("MUSIC_TASK START\r\n");

	
	file_buff = malloc(sizeof(uint8_t) * 10 * 1024 * 1024);
	error_num = f_mount(&fs_p,"0:/",1);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File f_mount failed!\r\nstop!\r\n");
		while(1);
	}

	//checkout directory

	error_num = f_opendir (&dir, "0:/");
	if ( error_num != FR_OK )
	{
		;
	}

	do
	{
		error_num = f_readdir (&dir, &fileinfo);
		if ( dir.sect == 0 ) //end of directory
		{
			break;
		}
		EMBARC_PRINTF("File name: %s  File size:%d\r\n",fileinfo.fname,fileinfo.fsize);
	}
	while( 1 );
	EMBARC_PRINTF("Close Directory\r\n");
	f_closedir(&dir);


	//readout_file();
	spi_poll();
	while(1);
}



