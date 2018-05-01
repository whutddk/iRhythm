#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"





void net_task()
{
	EMBARC_PRINTF("NET_TASK START\r\n");


	EMBARC_PRINTF("============================ while ============================\n");

	while (1)
	{
		if ( BUFF_EMPTY == flag_netbuff )
		{
			if ( Songid_HEAD == Songid_END )
			{
				EMBARC_PRINTF("\r\nrequest songid\r\n");

				if( -1 == socket_request(SONG_ID) )
				{
					EMBARC_PRINTF("\r\nSongid Error\r\nContinue!\r\n");
					continue;
				}
			}

			EMBARC_PRINTF("\r\nRequest Songurl\r\n");
			if ( -1 == socket_request(SONG_INFO))
			{
				EMBARC_PRINTF("\r\nSongurl Error\r\nContinue!\r\n");
				continue;
			}

			if ( *dllink == '\0'  )
			{
				EMBARC_PRINTF("\r\nNo Url,Reset\r\n");
				continue;
			}

			EMBARC_PRINTF("\r\nDownload Song\r\n");
			download_mp3();
		}
		else
		{
			_Rtos_Delay(5000);
		}
	}

	return ;
}


