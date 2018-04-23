#include "embARC.h"
#include "embARC_debug.h"

void gui_task()
{
	
	while(1)
	{
		EMBARC_PRINTF("GUI_TASK START\r\n");
		vTaskDelay( 1000 );
	}
}