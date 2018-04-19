#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"


FATFS fs_p;



uint8_t *file_buff;
void music_task()
{
	char * music_file = NULL;

	EMBARC_PRINTF("MUSIC_TASK START\r\n");

	spi_dma_prepare();
	iosignal_init();

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
	
	error_num = f_mount(&fs_p,"0:/",1);
	if( error_num != FR_OK)
	{
		EMBARC_PRINTF("File f_mount failed!\r\nstop!\r\n");
		while(1);
	}

	/*checkout directory and init song list to play*/
	playlist_init();

	

	iosignal_ctrl(1,0);
	readout_file();
	iosignal_ctrl(0,0);	


	spi =  spi_get_dev(DW_SPI_0_ID);
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));
	// spi_dma_test();
	play_mp3();
	
	while(1)
	{
		
		// strcat(filename,Playlist_HEAD -> data); 

		// //不是最后一首
		// if ( Playlist_HEAD -> next != NULL )
		// {
		// 	list_delete(FILE_LIST);
		// }
		// else
		// {
		// 	uartpc.printf("\r\nNo Song Left!!!\r\n");
		// }

		// uartpc.printf("\r\nplay %s\r\n",filename);
		// play_mp3(filename);

		// uartpc.printf("\r\nplay complete!!!\r\n");
	}
}



