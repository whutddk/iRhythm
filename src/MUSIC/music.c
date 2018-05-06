#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

volatile uint8_t flag_dma_finish = 0;
DEV_SPI_PTR spi;

FATFS fs_p;

uint8_t *file_buff;			//10MB File Buff to Read out from SD Card

/**********************
**	Read out the Information of  file in SD Card and store in file list
**
***/
static void playlist_init()
{
	DIR dir;
	FILINFO fileinfo;

	EMBARC_PRINTF("\r\nCreate Play List\r\n");

	struct filelist *lists = NULL;
	lists = (struct filelist *)malloc(sizeof(struct filelist));
	if ( NULL == lists )
	{
		EMBARC_PRINTF("\r\nPlay List Init Error!\r\n");
	}

/*open and checkout the directory*/
	// DIR* dir = opendir("/fs");
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
		if ( fileinfo.fattrib == 32 )			//Check File Style
		{
			filelist_add(FILE_LIST,&(fileinfo.fname[0]),fileinfo.fsize,IN_FILE);
			EMBARC_PRINTF("File name: %s  File size:%d   \r\n",fileinfo.fname,fileinfo.fsize);
		}
	}
	while( 1 );

	EMBARC_PRINTF("\r\nCloseing root directory. \r\n");
	f_closedir(&dir);
	// return_error(error);

}

/***********
**  Initialization 
**	Malloc 10MB buff to read out File from SD card
**	Create play list
*/
void play_init()
{
	

	spi =  spi_get_dev(DW_SPI_0_ID);
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

/********checkout directory and init song list to play****************/
	playlist_init();
}

/************
**	Read Out One Song Information from Play List
**	if Song is in SD Card ,Read Out File
**	Play Song
*/



int32_t Start_playing()
{
	char * music_filename = NULL;
	int file_lenth;
	uint8_t file_location;

	/**Malloc file name string space**/
	music_filename = malloc(sizeof(char) * 50);

	file_lenth = Playlist_HEAD -> lenth;
	file_location = Playlist_HEAD -> location;
	memset( music_filename, 0, sizeof(char) * 50 );
	strcat( music_filename,Playlist_HEAD -> data ); 

	/***********If it is the last Song in Play List,Play it again and again and Never Delete*******************/
	if ( Playlist_HEAD -> next != NULL )
	{
		filelist_delete(FILE_LIST);				//Once Play a Song, delete it from Playlist
	}
	else
	{
		EMBARC_PRINTF("\r\nNo Song Left!!!\r\n");
	}

	EMBARC_PRINTF("\r\nplay %s\r\n",music_filename);

	/********If the Song File is Bigger than 10MB Buff,Play Next one*****************************/
	if ( file_lenth  > 10 * 1024* 1024 )
	{
	 	EMBARC_PRINTF("\r\nfile too big,play fail!\r\n");
	 	return -1;
	}
	EMBARC_PRINTF("\r\nfile lenth = %d \r\n",file_lenth);

	/**read out file to DDR2 from SD card ,if Net Buff is EMPTY**/

	if ( file_location == IN_FILE )
	{
		spi->spi_control(SPI_CMD_MST_SET_FREQ,CONV2VOID(1000000));
		// iosignal_ctrl(1,0);
		readout_file(music_filename);
		// iosignal_ctrl(0,0);	
	}
	else
	{
		;
	}
	play_mp3(file_lenth,file_location);
	EMBARC_PRINTF("\r\nplay complete!!!\r\n");
	return 0;
}