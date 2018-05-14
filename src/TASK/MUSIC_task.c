#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"



void music_task()
{

	EMBARC_PRINTF("MUSIC_TASK START\r\n");
	
	play_init();
	
	while(1)
	{
		Start_playing();
	}
}



