#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#include "include.h"

void gui_task()
{		
	while(1)
	{	
		reflash_screen();	

		xEventGroupWaitBits( 
				GUI_Ev, 
				BIT_0 , 	//Wait BIT0 to reflash
				pdTRUE, 		//BIT_0 should be cleared before returning.
				pdFALSE, 		// Wait for both bits
				portMAX_DELAY );
	}
	 return ;
}
