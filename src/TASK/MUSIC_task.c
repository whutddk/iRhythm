#include "embARC.h"
#include "embARC_debug.h"

// FATFS_DISKIO *fs;
void music_task()
{
	int32_t error_num = 0;
	EMBARC_PRINTF("MUSIC_TASK START\r\n");

	// fs = get_fatfs_diskio(EMSK_SDCARD_0_DRVID);
	if (error_num = disk_initialize (	EMSK_SDCARD_0_DRVID	/* Physical drive nmuber (0..) */)  == STA_NOINIT)
	{
		EMBARC_PRINTF("SD_card init fail!\r\n");
	}
	else
	{
		EMBARC_PRINTF("SD_card init status:%d !\r\n",error_num);
	}



	while(1)
	{
		
		
	}
}



