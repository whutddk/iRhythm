#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"


FATFS fs_p;



uint8_t *file_buff;
int file_lenth;
void music_task()
{
	char * music_filename = NULL;


	EMBARC_PRINTF("MUSIC_TASK START\r\n");

	spi_dma_prepare();
	iosignal_init();

/** malloc 10MB space from DDR2 for reading mp3 file from SD card **/
	file_buff = malloc(sizeof(uint8_t) * 10 * 1024 * 1024);
	if ( file_buff == NULL )
	{
		EMBARC_PRINTF("Malloc file buff fail!\r\nstop!\r\n");
		while(1);
	}
	else
	{
		EMBARC_PRINTF("Malloc file buff pass!\r\n");
	}
	
/**mount Fatfs**/
	error_num = f_mount(&fs_p,"0:/",1);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File f_mount failed!\r\nstop!\r\n");
		while(1);
	}

/*checkout directory and init song list to play*/
	playlist_init();

/**Malloc file name string space**/
	music_filename = malloc(sizeof(char) * 50);

	



	// spi_dma_test();
	// play_mp3();
	
	while(1)
	{
		file_lenth = Playlist_HEAD -> lenth;
		memset( music_filename, 0, sizeof(char) * 50 );
		strcat(music_filename,Playlist_HEAD -> data); 
		//file_lenth = Playlist_HEAD -> lenth;
		//不是最后一首
		if ( Playlist_HEAD -> next != NULL )
		{
			filelist_delete(FILE_LIST);
		}
		else
		{
			EMBARC_PRINTF("\r\nNo Song Left!!!\r\n");
		}

		EMBARC_PRINTF("\r\nplay %s\r\n",music_filename);

		if ( file_lenth  > 10 * 1024* 1024 )
		{
		 	EMBARC_PRINTF("\r\nfile too big,play fail!\r\n");
		 	continue;
		}
		EMBARC_PRINTF("\r\nfile lenth = %d \r\n",file_lenth);

/**read out file to DDR2 from SD card ,can product by another task**/
		iosignal_ctrl(1,0);
		readout_file(music_filename);
		iosignal_ctrl(0,0);	

		spi =  spi_get_dev(DW_SPI_0_ID);
		spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));

		EMBARC_PRINTF("\r\nfile lenth %d \r\n",file_lenth);
		play_mp3(file_lenth);

		EMBARC_PRINTF("\r\nplay complete!!!\r\n");
	}
}



