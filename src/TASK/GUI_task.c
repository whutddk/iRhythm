/**
 * GUI TASK HERE
 * DDK
 * 2018 05 10
 */
#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#include "include.h"


/**
 * \brief       Main function of Gui Task
 */
void gui_task()
{
	while (1) {
		reflash_screen();

		xEventGroupWaitBits(
			GUI_Ev,
			BIT_0 , 		//Wait BIT0 to reflash
			pdTRUE, 		//BIT_0 Should be cleared after Returning.
			pdFALSE, 		
			portMAX_DELAY );
	}

	return ;
}
