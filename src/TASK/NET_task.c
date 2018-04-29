#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"





void net_task()
{

	EMBARC_PRINTF("NET_TASK START\r\n");
    //ESP8266 Init
    

    socket_request(SONG_ID);
    socket_request(SONG_INFO);
    // socket_request(SONG_INFO);
    download_mp3();
    EMBARC_PRINTF("============================ while ============================\n");

    while (1)
    {
        // socket_request(SONG_INFO);
    }

    return ;
}


