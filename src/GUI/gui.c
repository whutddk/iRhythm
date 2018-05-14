#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

#include "include.h"


struct _gui_info
{
	int16_t network_speed;
	int16_t decord_speed;
	int16_t main_cycle;

	char[15] song_name;
	char[15] next_song;

	uint8_t KEEP_ID3V2;
	uint8_t flag_next;
};
