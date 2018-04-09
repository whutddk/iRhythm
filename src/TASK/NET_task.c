#include "embARC.h"
#include "embARC_debug.h"

void net_task()
{
	while(1)
	{
		EMBARC_PRINTF("NET_TASK START\r\n");
		vTaskDelay( 1000 );
	}
}