#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"





void net_task()
{
	EMBARC_PRINTF("NET_TASK START\r\n");


	EMBARC_PRINTF("============================ while ============================\n");

	while (1)
	{

		/***************There is No Song in Buff ********************************************/
		if ( BUFF_EMPTY == flag_netbuff )
		{
			/********Check Song ID List is not Empty*********************/
			if ( Songid_HEAD == Songid_END )
			{
			
			/*********Request Song ID to Request Download Url*******************/
				EMBARC_PRINTF("\r\nrequest songid\r\n");
				if( -1 == socket_request(SONG_ID) )
				{
					EMBARC_PRINTF("\r\nRequest Songid Fail,Continue!\r\n");
					continue;
				}
			}

			/************Request Song Information Including Download Url**********/
			EMBARC_PRINTF("\r\nRequest Songurl\r\n");
			if ( -1 == socket_request(SONG_INFO))
			{
				EMBARC_PRINTF("\r\nRequest Songurl Fail,Continue!\r\n");
				continue;
			}

			/************Check Song  Download Url It's Unnecessary Now**********/
			if ( *dllink == '\0'  )
			{
				EMBARC_PRINTF("\r\nNo Url,Reset\r\n");
				continue;
			}

			/************Download Song *****************************************/
			EMBARC_PRINTF("\r\nDownload Song\r\n");
			download_mp3();
		}
		/***************There is a Song in Buff and has not been Play yet********************************************/
		else
		{
			/**********Just Release RTOS Task*******************************/
			_Rtos_Delay(5000);
		}
	}

	return ;
}


