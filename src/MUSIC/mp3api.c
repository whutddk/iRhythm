#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"

#include "perform.h"
#include "include.h"


// volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 					//Ping-pong Buff switching Flag

int8_t buf_rec1[2304];						//Ping-pong Buff for DMA Transfer
int8_t buf_rec2[2304];


/***
**	MP3 Decord Core Function 
**  filelenth : BUFF length should be Decord
**	loction :IF the Data in 10MB FILE BUFF or IN 10MB NET BUFF
**/
int32_t play_mp3(int32_t filelenth,uint8_t location)
{
	int32_t offset;
	uint8_t *read_ptr;
	uint8_t *file_ptr;
	
	int32_t byte_left = filelenth;

	uint32_t res_dec;

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

	read_ptr = file_ptr;


	/***Prepare to transfer by SPI DMA *****/
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));
	spi->spi_control(SPI_CMD_MST_SET_FREQ,CONV2VOID(12000000));

	mp3_dec = (MP3DecInfo*)MP3InitDecoder();
	if ( mp3_dec != NULL )
	{
		//EMBARC_PRINTF("Malloc mp3_dec buff Pass!\r\n");
	}
	else
	{
		EMBARC_PRINTF("Malloc mp3_dec buff fail!\r\nstop!\r\n");
		while(1);
	}

	EMBARC_PRINTF("Start to Trace\r\n");
	

	xEventGroupSetBits( evt1_cb, BIT_0 | BIT_1 );

	/*************Start to Decord MP3******************************/
	perf_start();
	while(1)
	{

		if ( gui_info.flag_next != 1)
		{
			;
		}
		else//play next song?
		{
			gui_info.decord_speed = -1;
			gui_info.main_cycle = -1;
			return 1;
		}


		offset = MP3FindSyncWord(read_ptr, byte_left);
 
		if ( offset >= 0 )
		{
			read_ptr += offset;         //data start point
			byte_left -= offset;        //in buffer

			// iosignal_ctrl(1,0);			
			gui_info.main_cycle = perf_end();
			
			perf_start();
			
			if ( flag_sw == 0 )
			{				
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec1, 0);				
			}
			else
			{			
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec2, 0);			
			}
			gui_info.decord_speed = perf_end();
			//iosignal_ctrl(0,0);
			if (res_dec == ERR_MP3_NONE)
			{
				//EMBARC_PRINTF("MP3Decode Time :%dus!\n\r",cost_cyc);
			}
			else
			{
				EMBARC_PRINTF("MP3Decode error:%d!\n\r",res_dec);
				read_ptr += 2;
				byte_left -= 2;
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

			if ( iosignal_read(0) )
			{
				uxBits = 0;
			}
			else
			{
				xEventGroupSetBits( GUI_Ev, BIT_0 );
				//EMBARC_PRINTF("GPIO Clear BIT1\r\n");
				uxBits = xEventGroupClearBits( evt1_cb, BIT_1 );
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
			
		}
		else
		{
/*****scan whole buff,no start is end **********/
			EMBARC_PRINTF("Decorder Complete!\n\r" );
			break;

		}
	}

	gui_info.decord_speed = -1;
	gui_info.main_cycle = -1;
	
	/********Play Song in NET Buff,should Reset Flag*****************/
	if ( location == IN_BUFF )
	{
		flag_netbuff = BUFF_EMPTY;
	}

	EMBARC_PRINTF("MP3 file: decorder is over!\n\r" );

	return 0;
}



