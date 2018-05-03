#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"


#include "include.h"

 #define NUM_BYTE_READ 4096


// volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 					//Ping-pong Buff switching Flag

char buf_rec1[2304]={1};						//Ping-pong Buff for DMA Transfer
char buf_rec2[2304]={1};

uint8_t dec_buff[NUM_BYTE_READ] = {1};			//Decord buff in DCCM,much Smaller,Read Data From 10MB File BUFF


/***
**	MP3 Decord Core Function 
**  filelenth : BUFF length should be Decord
**	loction :IF the Data in 10MB FILE BUFF or IN 10MB NET BUFF
**/
void play_mp3(int filelenth,uint8_t location)
{
	uint32_t temp = 0;

	int32_t offset;
	uint8_t *read_ptr = dec_buff;
	uint8_t *file_ptr;
	
	/*这里改文件大小*/
	int file_left = filelenth;
	int byte_left = NUM_BYTE_READ;

	uint32_t res_dec;
	int flag_start = 0;

	MP3DecInfo *mp3_dec;

	EventBits_t uxBits;

	/*code*/

	if ( location == IN_FILE )
	{
		file_ptr = file_buff;
	}
	else
	{
		file_ptr = net_buff;
	}

	/***Prepare to transfer by SPI DMA *****/
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));
	spi->spi_control(SPI_CMD_MST_SET_FREQ,CONV2VOID(3000000));

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

	/*************Start to Decord MP3******************************/
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

			// if ( iosignal_read(0) )
			// {
			// 	uxBits = 0;
			// }
			// else if (( uxBits & BIT_1 ) != 0 )
			// {
			// 	EMBARC_PRINTF("uxBits & BIT_1  != 0\r\n");
			// }
			// else
			// {
			// 	EMBARC_PRINTF("GPIO Clear BIT1\r\n");
			// 	uxBits = xEventGroupClearBits( evt1_cb, BIT_1 );
			// }
			/******Can Replace by IO interrupt to Set Event****************/
			while(!iosignal_read(0))
			{
				;
			}
				
/********************Shedule End Here*****************************/

			/**********Enable DMA to Transfer**********************/
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

	    	/**************Read out data in 10MB buff to DCCM BUFF************/
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
	/********Play Song in NET Buff,should Reset Flag*****************/
	if ( location == IN_BUFF )
	{
		flag_netbuff = BUFF_EMPTY;
	}
	EMBARC_PRINTF("Free mp3_dec!\n\r" );
	MP3FreeDecoder(mp3_dec);

	EMBARC_PRINTF("MP3 file: decorder is over!\n\r" );
}



