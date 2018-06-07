/**
 * GUI API CORE CODE
 * DDK
 * 2018 05 10
 */


#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

#include "oled.h"

struct _gui_info gui_info;	//GUI Control Infomation

/**
 *  \brief      Chinese pixel data
 *
 *
 *
 */


/**
 * \brief       Gui Screen 1 Reflash Function to Show Debug Preformace Message
 *
 */
static void perform_screen()
{
	
	if ( gui_info.network_speed < 0) {
		//u8g_DrawStr(&_u8g, 80, 0, "Stop!");
		// Draw_BMP( 80, 0, 96, 1, bitmap_ting);
		// Draw_BMP( 97, 0, 103, 1, bitmap_zhi);
		OLED_P6x8Str( 70, 0, "Stop!   ");
	} else {
		OLED_PrintInt16( 70, 0, gui_info.network_speed );
		OLED_P6x8Str( 88, 0, "KB/s");
	}

	if ( gui_info.decord_speed < 0) {
		//u8g_DrawStr(&_u8g, 80, 18, "Stop!");
		// Draw_BMP( 80, 18, 2, 16, bitmap_ting);
		// Draw_BMP( 96, 18, 2, 16, bitmap_zhi);
		OLED_P6x8Str( 70, 3, "Stop!   ");
	} else {
		OLED_PrintInt16( 70, 3,gui_info.decord_speed );
		OLED_P6x8Str( 88, 3, "ms");
	}


	if ( 0 == gui_info.perf_update ) {
		/* Display Net Speed */
		OLED_CLS();
		OLED_P6x8Str(0, 0, "Net Speed:");
		// Draw_BMP( 0, 0, 16, 1, bitmap_xia);
		// Draw_BMP( 17, 0, 32, 1, bitmap_zai);
		// Draw_BMP( 33, 0, 48, 1, bitmap_su);
		// Draw_BMP( 49, 0, 64, 1, bitmap_du);

		/* Display Decord time */
		OLED_P6x8Str( 0, 1, "Decord:");
		// Draw_BMP( 0, 18, 2, 16, bitmap_jie);
		// Draw_BMP( 16, 18, 2, 16, bitmap_ma);
		// Draw_BMP( 32, 18, 2, 16, bitmap_su);
		// Draw_BMP( 48, 18, 2, 16, bitmap_du);

		/* Display Song Name Playing Now */
		// Draw_BMP( 0, 45, 2, 16, bitmap_ge);
		// Draw_BMP( 16, 45, 2, 16, bitmap_qu);
		OLED_P6x8Str(0, 5, "SONG:");
		OLED_P6x8Str( 40, 5, gui_info.song_name);

		/* Display Song Name May be Selected to Play Next*/
		// Draw_BMP( 0, 0, 2, 16, bitmap_xia);
		// Draw_BMP( 16, 0, 2, 16, bitmap_yi);
		// Draw_BMP( 32, 0, 2, 16, bitmap_shou);
		OLED_P6x8Str(0, 6, "Next Song:");
		OLED_P6x8Str( 20, 6, gui_info.next_song);
		gui_info.perf_update = 1;
	}

	

}

/**
 * \brief       This function used to show song name to select song
 *
 */

static void FFT_screen()
{
	gui_info.perf_update = 0;

	draw_fft();

}



/**
 * \brief       Main Loop function of GUI task
 *
 */
void reflash_screen()
{
	/* if No Operation Appears in 5 Seconds,then Return to Performance Page */
	if ( (gui_info.delay_cnt < ( xTaskGetTickCount () - 5000 )) 
		&& ( 0 != gui_info.screen_point ) ) {
			gui_info.screen_point = 0;
			gui_info.fft_show[0] = 0;
			gui_info.fft_show[1] = 0;
			gui_info.fft_show[2] = 0;
			gui_info.fft_show[3] = 0;
			OLED_CLS();
	}

	switch (gui_info.screen_point) {
		case (0):
			FFT_screen();
			
			break;

		default:
			perform_screen();
			break;
	}

}


