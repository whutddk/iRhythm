#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"


void key1_isr()//确定键
{
	//SWITCH 254 SONG most 
	uint8_t i;
	struct filelist *file_pointer = Playlist_HEAD;

	gui_info.delay_cnt = xTaskGetTickCount ();

	if ( gui_info.screen_point == 0 )
	{
		gui_info.screen_point ++;
		gui_info.next_song = file_pointer -> data;
	}
	else
	{
		gui_info.flag_next = 1;

		//how to stay in the same song?
		for ( i = gui_info.screen_point - 1; i > 0; i -- )
		{
				/***********If it is the last Song in Play List,Play it again and again and Never Delete*******************/
			if ( Playlist_HEAD -> next != NULL )
			{
				if ( Playlist_HEAD -> location != IN_BUFF )
				{
					;
				}
				else
				{
					flag_netbuff = BUFF_EMPTY;
				}


				filelist_delete(FILE_LIST);				//Once Play a Song, delete it from Playlist
			}
			else
			{
				EMBARC_PRINTF("\r\nNo Song Left!!!\r\n");
			}
		}
		gui_info.screen_point = 0;
		gui_info.network_speed = -1;
		gui_info.decord_speed = -1;
		gui_info.main_cycle = -1;
	}
	//EMBARC_PRINTF("key1_isr!\r\n");


	//Now Reflash the Srceen
	xEventGroupSetBits( GUI_Ev, BIT_0 );
}

void key2_isr()//右键
{
	uint8_t i;
	struct filelist *file_pointer = Playlist_HEAD;

	gui_info.delay_cnt = xTaskGetTickCount ();

	for ( i = gui_info.screen_point ; i > 0 ;i -- )
	{
		file_pointer = file_pointer -> next;
		if ( file_pointer != NULL )
		{
			
		}
		else
		{
			return;	//超出了没必要刷新画面，刷新了时间即可，如果是0界面，不会出现这种情况
		}
	}

	//EMBARC_PRINTF("key2_isr!\r\n");

	gui_info.next_song = file_pointer -> data;
	gui_info.screen_point ++;

	//Now Reflash the Srceen
	xEventGroupSetBits( GUI_Ev, BIT_0 );
}

void key3_isr()
{
	//EMBARC_PRINTF("key3_isr!\r\n");
}