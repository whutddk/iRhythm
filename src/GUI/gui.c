/**
 * GUI API CORE CODE
 * DDK
 * 2018 05 10
 */


#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"


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

	/* Display Net Speed */
	//u8g_DrawStr(&_u8g, 0, 0, "Net Speed:");
	u8g_DrawBitmap(&_u8g, 0, 0, 2, 16, bitmap_xia);
	u8g_DrawBitmap(&_u8g, 16, 0, 2, 16, bitmap_zai);
	u8g_DrawBitmap(&_u8g, 32, 0, 2, 16, bitmap_su);
	u8g_DrawBitmap(&_u8g, 48, 0, 2, 16, bitmap_du);

	if ( gui_info.network_speed < 0) {
		//u8g_DrawStr(&_u8g, 80, 0, "Stop!");
		u8g_DrawBitmap(&_u8g, 80, 0, 2, 16, bitmap_ting);
		u8g_DrawBitmap(&_u8g, 96, 0, 2, 16, bitmap_zhi);
	} else {
		u8g_DrawStr(&_u8g, 80, 7, u8g_u16toa((uint16_t) gui_info.network_speed, 3));
		u8g_DrawStr(&_u8g, 100, 7, "KB/s");
	}

	/* Display Decord time */
	//u8g_DrawStr(&_u8g, 0, 15, "Decord:");
	u8g_DrawBitmap(&_u8g, 0, 18, 2, 16, bitmap_jie);
	u8g_DrawBitmap(&_u8g, 16, 18, 2, 16, bitmap_ma);
	u8g_DrawBitmap(&_u8g, 32, 18, 2, 16, bitmap_su);
	u8g_DrawBitmap(&_u8g, 48, 18, 2, 16, bitmap_du);

	if ( gui_info.decord_speed < 0) {
		//u8g_DrawStr(&_u8g, 80, 18, "Stop!");
		u8g_DrawBitmap(&_u8g, 80, 18, 2, 16, bitmap_ting);
		u8g_DrawBitmap(&_u8g, 96, 18, 2, 16, bitmap_zhi);
	} else {
		u8g_DrawStr(&_u8g, 80, 25, u8g_u16toa((uint16_t) gui_info.decord_speed, 2));
		u8g_DrawStr(&_u8g, 100, 25, "ms");
	}

	/* Display Song Name Playing Now */
	//u8g_DrawStr(&_u8g, 0, 45, "SONG:");
	u8g_DrawBitmap(&_u8g, 0, 45, 2, 16, bitmap_ge);
	u8g_DrawBitmap(&_u8g, 16, 45, 2, 16, bitmap_qu);

	u8g_DrawStr(&_u8g, 34, 52, ":");
	u8g_DrawStr(&_u8g, 40, 52, gui_info.song_name);

	/* Display Song Name May be Selected to Play Next*/

	//u8g_DrawStr(&_u8g, 0, 0, "Next Song:");
	u8g_DrawBitmap(&_u8g, 0, 0, 2, 16, bitmap_xia);
	u8g_DrawBitmap(&_u8g, 16, 0, 2, 16, bitmap_yi);
	u8g_DrawBitmap(&_u8g, 32, 0, 2, 16, bitmap_shou);

	u8g_DrawStr(&_u8g, 20, 30, gui_info.next_song);

}

/**
 * \brief       This function used to show song name to select song
 *
 */
static void FFT_screen()
{




}



/**
 * \brief       Main Loop function of GUI task
 *
 */
void reflash_screen()
{
	/* if No Operation Appears in 5 Seconds,then Return to Performance Page */
	if ( gui_info.delay_cnt < ( xTaskGetTickCount () - 5000 ) ) {
		gui_info.screen_point = 0;
	}

	switch (gui_info.screen_point) {
		case (0):
			screen1();
			break;

		default:
			screen2();
			break;
	}

}


