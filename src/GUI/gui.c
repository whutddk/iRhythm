#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#include "include.h"

u8g_t _u8g;

// struct _gui_info
// {
// 	int16_t network_speed;
// 	int16_t decord_speed;
// 	int16_t main_cycle;

// 	char[15] song_name;
// 	char[15] next_song;

// 	uint8_t KEEP_ID3V2;
// 	uint8_t flag_next;
// };

void gui_init()
{
	u8g_InitComFn(&_u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_COM_SSD_I2C);
	u8g_Begin(&_u8g);
	u8g_SetFont(&_u8g, u8g_font_5x7);
	u8g_FirstPage(&_u8g);
}




void u8g_prepare(void) {
	u8g_SetFont(&_u8g, u8g_font_6x10);		/* set the current font and reset the font reference position to "Baseline" */
	u8g_SetFontRefHeightExtendedText(&_u8g);		/* define the calculation method for the ascent and descent of the current font */
	u8g_SetDefaultForegroundColor(&_u8g);		/* assign one of the default colors as current color index */
	u8g_SetFontPosTop(&_u8g);			/* set the reference position for the character and string draw procedure */
}

// void u8g_ascii_1(void) {
// 	char s[2] = " ";
// 	uint8_t x, y;
// 	u8g_DrawStr(&_u8g, 0, 0, "ASCII page 1");
// 	for( y = 0; y < 6; y++ ) {
// 		for( x = 0; x < 16; x++ ) {
// 			s[0] = y*16 + x + 32;
// 			u8g_DrawStr(&_u8g, x*7, y*10+10, s);
// 		}
// 	}
// }


void reflash_screen()
{

	u8g_FirstPage(&_u8g);
	do 
	{
		u8g_prepare();
		// u8g_ascii_1();
		u8g_DrawStr(&_u8g, 0, 0, "net_speed:");
		u8g_DrawStr(&_u8g, 0, 10, "dec_time:");
		u8g_DrawStr(&_u8g, 0, 20, "main_cycle:");
		u8g_DrawStr(&_u8g, 0, 30, "SONG:");
		u8g_DrawStr(&_u8g, 64, 30, "NEXT SONG:");
	} 
	while( u8g_NextPage(&_u8g) );

}


