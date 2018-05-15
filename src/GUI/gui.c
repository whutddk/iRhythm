#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#include "include.h"

u8g_t _u8g;
struct _gui_info gui_info;






static void u8g_prepare(void) {
	u8g_SetFont(&_u8g, u8g_font_8x13);		/* set the current font and reset the font reference position to "Baseline" */
	u8g_SetFontRefHeightExtendedText(&_u8g);		/* define the calculation method for the ascent and descent of the current font */
	u8g_SetDefaultForegroundColor(&_u8g);		/* assign one of the default colors as current color index */
	u8g_SetFontPosTop(&_u8g);			/* set the reference position for the character and string draw procedure */
}

static void screen1()
{
		u8g_DrawStr(&_u8g, 0, 0, "Net Speed:");

		if ( gui_info.network_speed < 0)
		{
			u8g_DrawStr(&_u8g, 80, 0, "None!");
		}
		else
		{
			u8g_DrawStr(&_u8g, 80, 0,u8g_u16toa((uint16_t) gui_info.network_speed, 3));
			u8g_DrawStr(&_u8g, 100, 0,"KB/s");
		}

		u8g_DrawStr(&_u8g, 0, 15, "Decord:");

		if ( gui_info.decord_speed < 0)
			{
				u8g_DrawStr(&_u8g, 80, 15, "None!");
			}
			else
			{
				u8g_DrawStr(&_u8g, 80, 15,u8g_u16toa((uint16_t) gui_info.decord_speed, 2));
				u8g_DrawStr(&_u8g, 100, 15,"ms");
			}

		u8g_DrawStr(&_u8g, 0, 30, "Cycle:");

		if ( gui_info.main_cycle < 0)
			{
				u8g_DrawStr(&_u8g, 80, 30, "None!");
			}
			else
			{
				u8g_DrawStr(&_u8g, 80, 30,u8g_u16toa((uint16_t) gui_info.main_cycle, 3));
				u8g_DrawStr(&_u8g, 100, 30,"KB/s");
			}

		u8g_DrawStr(&_u8g, 0, 45, "SONG:");

}

void screen2()
{

}


void gui_init()
{
	gui_info.screen_point = 0;
	gui_info.network_speed = -1;
	gui_info.decord_speed = -1;
	gui_info.main_cycle = -1;
	gui_info.flag_next = 0;
	u8g_InitComFn(&_u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_COM_SSD_I2C);
	u8g_Begin(&_u8g);
	u8g_SetFont(&_u8g, u8g_font_5x7);
}

void reflash_screen()
{

	u8g_FirstPage(&_u8g);
	do 
	{
		u8g_prepare();
		switch(gui_info.screen_point)
		{
			case(0):
			screen1();
			break;
		}

	} 
	while( u8g_NextPage(&_u8g) );

}


