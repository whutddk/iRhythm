#include "embARC.h"
#include "embARC_debug.h"

void music_task()
{
	
	while(1)
	{
		EMBARC_PRINTF("MUSIC_TASK START\r\n");
		vTaskDelay( 1000 );
	}
}



