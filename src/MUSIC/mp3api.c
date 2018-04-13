#include "embARC.h"
#include "embARC_debug.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"

#include "include.h"

// #define NUM_BYTE_READ 4096


// volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 

short buf_rec1[2304];
short buf_rec2[2304];

void play_mp3()
{

	uint32_t temp = 0;

	int32_t offset;
	uint8_t *read_ptr = file_buff;

	/*这里改文件大小*/
	int byte_left = 4271541;

	uint32_t res_dec;
	MP3DecInfo *mp3_dec;

	mp3_dec = (MP3DecInfo*)MP3InitDecoder();
	if ( mp3_dec == NULL )
	{
		EMBARC_PRINTF("Malloc mp3_dec buff fail!\r\nstop!\r\n");
		while(1);
	}
	else
	{
		EMBARC_PRINTF("Malloc mp3_dec buff Pass!\r\nstop!\r\n");
	}

				// fread(buf_read,1,NUM_BYTE_READ,fd);
	EMBARC_PRINTF("start to trace\r\n");
	int flag_start = 0;

	// isTransferCompleted = true;
	// isFinished = true;

	while(1)
	{
		offset = MP3FindSyncWord(read_ptr, byte_left);

		if (offset < 0 ) 
		{										
			break;
		} 
		else 
		{

			read_ptr += offset;         //data start point
			byte_left -= offset;        //in buffer

			if ( flag_sw == 0 )
			{
				
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec1, 0);
				
			}
			else
			{
				
				res_dec = MP3Decode(mp3_dec, &read_ptr, (int *)&byte_left, buf_rec2, 0);
				
			}

			if (res_dec != ERR_MP3_NONE)
			{
				EMBARC_PRINTF("MP3Decode error:%d!\n\r",res_dec);
				read_ptr += 2;
				byte_left -= 2;
				continue;
			}
			else
			{
				// EMBARC_PRINTF("MP3Decode Pass!\n\r");
			}

			// while( isTransferCompleted == false )//|| isFinished == false )
			// {
			// 	;
			// }

			 //while(!LCRK.read());
   //      	while(!LCRK.read());

			// isFinished = false;
			// isTransferCompleted = false;
			// uartpc.printf("GO2\r\n");
			// while(!LCRK.read());
			// while(LCRK.read());

		    // if ( flag_sw == 0 )
		    // {
		    //     temp = (uint32_t)buf_rec1;
		    //     flag_sw = 1;
		    // }
		    // else
		    // {
		    //     temp = (uint32_t)buf_rec2;
		    //     flag_sw = 0;
		    // }

		    // /*  xfer structure */
		    // xfer.data = (uint8_t *)temp;
		    // xfer.dataSize = 4608;
		    // SAI_TransferSendEDMA(I2S0, &txHandle, &xfer);

			// masterXfer.txData = (uint8_t *)temp;
			// masterXfer.rxData = NULL;
			// masterXfer.dataSize = 4608;
			// masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

		 //    if (kStatus_Success !=
		 //        DSPI_MasterTransferEDMA(SPI0, &g_dspi_edma_m_handle, &masterXfer))
		 //    {
		 //        EMBARC_PRINTF("There is error when start DSPI_MasterTransferEDMA \r\n ");
		 //    }
		 //    else
		 //    {
		 //        // EMBARC_PRINTF(" start DSPI_MasterTransferEDMA DONE\r\n ");
		 //    }


			
				// byte_left = NUM_BYTE_READ;
				// read_ptr = buf_read;
			
		}
	}
	MP3FreeDecoder(mp3_dec);

	EMBARC_PRINTF("MP3 file: decorder is over!\n\r" );
}


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