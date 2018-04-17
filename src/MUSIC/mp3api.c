#include "embARC.h"
#include "embARC_debug.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"

#include "include.h"

 #define NUM_BYTE_READ 4096


// volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 

short buf_rec1[2304]={1};
short buf_rec2[2304]={1};

uint8_t dec_buff[NUM_BYTE_READ] = {1};

void play_mp3()
{

	uint32_t temp = 0;

	int32_t offset;
	uint8_t *read_ptr = dec_buff;
	uint8_t *raw_ptr = raw_buff;
	uint8_t *file_ptr = file_buff;
	/*这里改文件大小*/
	int file_left = 4271541;
	int byte_left = NUM_BYTE_READ;

	uint32_t res_dec;
	MP3DecInfo *mp3_dec;


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

	EMBARC_PRINTF("start to trace\r\n");
	int flag_start = 0;

	flag_dma_finish = 1;

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

			while(flag_dma_finish==0);
			flag_dma_finish = 0;

			while(iosignal_read(0));

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
					break;
				}
				continue;
			}
			else
			{
				break;
			}
		}
	}
	EMBARC_PRINTF("Free mp3_dec!\n\r" );
	MP3FreeDecoder(mp3_dec);

	EMBARC_PRINTF("MP3 file: decorder is over!\n\r" );
}

/*第二方案——异步解码播放
void send2spi()
{
	uint8_t *raw_ptr = raw_buff;

	spi_writeraw(raw_ptr);
	while(flag_dma_finish == 0);
	flag_dma_finish = 0;
	raw_ptr += 4096;

	// iosignal_ctrl(iosignal_read(0),0);
	// while(!iosignal_read(0));
	// iosignal_ctrl(iosignal_read(0),0);
}
第二方案——异步解码播放*/




// void playlist_init()
// {
// 	EMBARC_PRINTF("\r\nCreate Play List\r\n");

// 	struct list *lists = NULL;
// 	lists = (struct list *)malloc(sizeof(struct list));
// 	if ( NULL == lists )
// 	{
// 		uartpc.printf("\r\nPlay List Init Error!\r\n");
// 	}


// 	DIR* dir = opendir("/fs");
// 	errno_error(dir);

// 	struct dirent* de;
// 	// uartpc.printf("Printing all filenames:\r\n");

// 	// if ( ( de = readdir(dir)) != NULL )
// 	// {
// 	// 	strcat(lists -> data, &(de->d_name)[0]);
// 	// 	lists -> next = NULL;
// 	// 	Playlist_HEAD = lists;
// 	// 	Playlist_END = lists;
// 	// 	uartpc.printf("Add %s into Play List\r\n", &(de->d_name)[0]);
// 	// }


// 	while((de = readdir(dir)) != NULL)
// 	{
		
// 		//uartpc.printf("Format %d \r\n", de->d_type);
// 		if ( de->d_type == 5 )
// 		{
// 			list_add(1,&(de->d_name)[0]);
// 			uartpc.printf("Add %s into Play List\r\n", &(de->d_name)[0]);
// 		}
// 	}

// 	uartpc.printf("\r\nCloseing root directory. \r\n");
// 	closedir(dir);
// 	// return_error(error);

// }