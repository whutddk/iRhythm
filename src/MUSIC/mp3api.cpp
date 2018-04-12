#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"


volatile bool isFinished = true;
volatile uint8_t flag_sw = 0; 

short buf_rec1[2304];
short buf_rec2[2304];

unsigned char buf_read[NUM_BYTE_READ];
	

void play_mp3(char* filename)
{

	uint32_t temp = 0;

	int32_t offset;
	uint8_t *read_ptr;
	int byte_left = NUM_BYTE_READ;
	uint32_t res_dec;
	unsigned int num_read;
	MP3DecInfo *mp3_dec;

	read_ptr = buf_read;
	FILE* fd;

	mp3_dec = (MP3DecInfo*)MP3InitDecoder();

	char mp3file[50] = "/fs/";
	strcat(mp3file,filename);
	EMBARC_PRINTF("open %s!\r\n",mp3file);
	fd = fopen(mp3file,"rb");

	if (fd == NULL)
	{
		EMBARC_PRINTF(" Failure. %d \r\n", errno);
		return;
	}
	else
	{
		EMBARC_PRINTF(" done.\r\n");
	}

	fread(buf_read,1,NUM_BYTE_READ,fd);
	EMBARC_PRINTF("start to trace\r\n");
	int flag_start = 0;

	isTransferCompleted = true;
	isFinished = true;

	while(1)
	{
		offset = MP3FindSyncWord(read_ptr, byte_left);
		if (offset < 0 ) 
		{
			//uartpc.printf("offset:%d\r\n",offset);
			if( flag_start == 0 )
			{
				fread(buf_read,1,NUM_BYTE_READ,fd);
				if ( num_read == 0 )
				{
					break;
				}
				continue;
			}
			else
			{
				break;
			}
		} 
		else 
		{

			flag_start = 1;
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
				flag_start = 0;
				num_read = fread(buf_read,1,NUM_BYTE_READ,fd);
				if ( num_read == 0 )
				{
					break;
				}
				continue;
			}
		// uartpc.printf("GO1\r\n");
			// led1 = 1;



			while( isTransferCompleted == false )//|| isFinished == false )
			{
				;
			}

			 while(!LCRK.read());
   //      	while(!LCRK.read());
			// led1 = 0;
			// isFinished = false;
			isTransferCompleted = false;
			// uartpc.printf("GO2\r\n");
			// while(!LCRK.read());
			// while(LCRK.read());

		    if ( flag_sw == 0 )
		    {
		        temp = (uint32_t)buf_rec1;
		        flag_sw = 1;
		    }
		    else
		    {
		        temp = (uint32_t)buf_rec2;
		        flag_sw = 0;
		    }

		    // /*  xfer structure */
		    // xfer.data = (uint8_t *)temp;
		    // xfer.dataSize = 4608;
		    // SAI_TransferSendEDMA(I2S0, &txHandle, &xfer);

			masterXfer.txData = (uint8_t *)temp;
			masterXfer.rxData = NULL;
			masterXfer.dataSize = 4608;
			masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

		    if (kStatus_Success !=
		        DSPI_MasterTransferEDMA(SPI0, &g_dspi_edma_m_handle, &masterXfer))
		    {
		        EMBARC_PRINTF("There is error when start DSPI_MasterTransferEDMA \r\n ");
		    }
		    else
		    {
		        // EMBARC_PRINTF(" start DSPI_MasterTransferEDMA DONE\r\n ");
		    }


			if (byte_left < NUM_BYTE_READ) 
			{
				memmove(buf_read,read_ptr,byte_left);

				num_read = fread(buf_read + byte_left,1,NUM_BYTE_READ - byte_left,fd);

				if(num_read == 0) 
				{
					EMBARC_PRINTF("num_read:%d\r\n",num_read);
					break;
				}
				if (num_read < NUM_BYTE_READ - byte_left)
				{
					memset(buf_read + byte_left + num_read, 0, NUM_BYTE_READ - byte_left - num_read);
				}
				byte_left = NUM_BYTE_READ;
				read_ptr = buf_read;
			}
		}
	}
	MP3FreeDecoder(mp3_dec);
	fclose(fd);

	EMBARC_PRINTF("MP3 file:%s decorder is over!\n\r" ,mp3file);
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