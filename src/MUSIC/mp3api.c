#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"


#include "include.h"

 #define NUM_BYTE_READ 4096


// volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 

char buf_rec1[2304]={1};
char buf_rec2[2304]={1};

uint8_t dec_buff[NUM_BYTE_READ] = {1};

void play_mp3(int filelenth)
{

	uint32_t temp = 0;

	int32_t offset;
	uint8_t *read_ptr = dec_buff;
	uint8_t *raw_ptr = raw_buff;
	uint8_t *file_ptr = file_buff;
	/*这里改文件大小*/
	int file_left = filelenth;
	int byte_left = NUM_BYTE_READ;

	uint32_t res_dec;
	int flag_start = 0;

	MP3DecInfo *mp3_dec;

	EventBits_t uxBits;

	/*code*/
	mp3_dec = (MP3DecInfo*)MP3InitDecoder();
	if ( mp3_dec == NULL )
	{
		EMBARC_PRINTF("Malloc mp3_dec buff fail!\r\nstop!\r\n");
		while(1);
	}
	else
	{
		EMBARC_PRINTF("Malloc mp3_dec buff Pass!\r\n");
	}

				// fread(buf_read,1,NUM_BYTE_READ,fd);
				memmove(dec_buff,file_ptr,NUM_BYTE_READ);
				file_ptr += NUM_BYTE_READ;
				file_left -= NUM_BYTE_READ;

	EMBARC_PRINTF("Start to Trace\r\n");
	
	flag_start = 0;
	// flag_dma_finish = 1;
	xEventGroupSetBits( evt1_cb, BIT_0 | BIT_1 );
	while(1)
	{
		offset = MP3FindSyncWord(read_ptr, byte_left);
 
		if ( offset >= 0 )
		{

			read_ptr += offset;         //data start point
			byte_left -= offset;        //in buffer


			iosignal_ctrl(1,0);
			if ( flag_sw == 0 )
			{
				
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec1, 0);
				
			}
			else
			{
				
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec2, 0);
				
			}
			iosignal_ctrl(0,0);
			if (res_dec == ERR_MP3_NONE)
			{
				// EMBARC_PRINTF("MP3Decode Pass!\n\r");
				// memcpy(raw_ptr,(uint8_t*)(buf_rec1),4608);
				// raw_ptr += 4608;
			}
			else
			{
				EMBARC_PRINTF("MP3Decode error:%d!\n\r",res_dec);
				read_ptr += 2;
				byte_left -= 2;
						memmove(dec_buff,file_ptr,NUM_BYTE_READ);
								file_ptr += NUM_BYTE_READ;
								file_left -= NUM_BYTE_READ;
						if ( file_left <= 0 )
						{
							//这里可能越界，需要保护
							break;
						}
				continue;
				
			}

/********************Shedule Here*****************************/
			xEventGroupWaitBits( 
				evt1_cb, 
				BIT_0 | BIT_1 , 	//regard BIT0 as dma finish,regard BIT1 as buff full
				pdFALSE, 		//BIT_0 and BIT_1 should Not be cleared before returning.
				pdTRUE, 		// Wait for both bits
				portMAX_DELAY );
			xEventGroupClearBits( evt1_cb, BIT_0 );
			// while(flag_dma_finish==0);
			// flag_dma_finish = 0;

			// EMBARC_PRINTF("GPIO BIT1 %d\r\n",iosignal_read(0));
			if ( iosignal_read(0) )
			{
				uxBits = 0;
			}
			else if (( uxBits & BIT_1 ) != 0 )
			{
				EMBARC_PRINTF("uxBits & BIT_1  != 0\r\n");
			}
			else
			{
				EMBARC_PRINTF("GPIO Clear BIT1\r\n");
				uxBits = xEventGroupClearBits( evt1_cb, BIT_1 );
			}
			// while(!iosignal_read(0))
			// 	{
			// 		EMBARC_PRINTF("!GPIO BIT1 %d\r\n",!iosignal_read(0));
			// 	}
			// 	EMBARC_PRINTF("!GPIO BIT1 %d\r\n",!iosignal_read(0));
/********************Shedule End Here*****************************/

			if ( flag_sw == 0 )
			{
				spi_writeraw((uint8_t*)buf_rec1);
				flag_sw = 1;
			}
			else
			{
				spi_writeraw((uint8_t*)buf_rec2);
		 		flag_sw = 0;
		    }


			if (byte_left < NUM_BYTE_READ) 
			{
				memmove(dec_buff,read_ptr,byte_left);

							//num_read = fread(buf_read + byte_left,1,NUM_BYTE_READ - byte_left,fd);
							memmove(dec_buff + byte_left,file_ptr,NUM_BYTE_READ - byte_left);
								file_ptr += NUM_BYTE_READ - byte_left;
								file_left -= NUM_BYTE_READ - byte_left;
						if ( file_left <= 0 )
						{
							//这里可能越界，需要保护
							break;
						}
				
				byte_left = NUM_BYTE_READ;
				read_ptr = dec_buff;
			}
			
		}
		else
		{
			if( flag_start == 0 )
			{
					//fread(buf_read,1,NUM_BYTE_READ,fd);
						memmove(dec_buff,file_ptr,NUM_BYTE_READ);
						file_ptr += NUM_BYTE_READ;
						file_left -= NUM_BYTE_READ;
				if ( file_left <= 0 )
				{
					//这里可能越界，需要保护
					EMBARC_PRINTF("decorder never start and file end!\n\r" );
					break;
				}
				continue;
			}
			else
			{
				EMBARC_PRINTF("decorder start and complete!\n\r" );
				break;
			}
		}
	}
	EMBARC_PRINTF("Free mp3_dec!\n\r" );
	MP3FreeDecoder(mp3_dec);

	EMBARC_PRINTF("MP3 file: decorder is over!\n\r" );
}

void playlist_init()
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
		if ( fileinfo.fattrib == 32 )
		{
			filelist_add(FILE_LIST,&(fileinfo.fname[0]),fileinfo.fsize);
			EMBARC_PRINTF("File name: %s  File size:%d   \r\n",fileinfo.fname,fileinfo.fsize);
		}
	}
	while( 1 );



	// struct dirent* de;

	// while((de = readdir(dir)) != NULL)
	// {
		
	// 	//uartpc.printf("Format %d \r\n", de->d_type);
	// 	if ( de->d_type == 5 )
	// 	{
	// 		list_add(1,&(de->d_name)[0]);
	// 		EMBARC_PRINTF("Add %s into Play List\r\n", &(de->d_name)[0]);
	// 	}
	// }

	EMBARC_PRINTF("\r\nCloseing root directory. \r\n");
	f_closedir(&dir);
	// return_error(error);

}

