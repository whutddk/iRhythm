#include "embARC.h"
#include "embARC_debug.h"
#include "u8g.h"

u8g_t u8g;
char *song[10]={"Attention","Baby","burning","Calibaby","Cool","Cry","Faded","Giants","I Got You"};
char *menu[2]={"Song List","Volume"};
int picture = 0;
int button_cnt1 = 0;
int ys = 0;
int song_cnt=0;
int songlist_cnt=0;
int song_cnt_tmp;
int ym = 0;
int vl=0;
int w=0;
int playx=0;
int button_cnt2 = 0;
uint32_t mask = 0x7;


void drawsonglist(int y2,int song_cntlist){
	u8g_FirstPage(&u8g);
		do{
			 u8g_DrawStr(&u8g,0, 20, song[song_cntlist]);
			 u8g_DrawStr(&u8g,0, 40, song[song_cntlist+1]);
			 u8g_DrawStr(&u8g,0, 60, song[song_cntlist+2]);
			 u8g_DrawFrame(&u8g,0,y2,120,22);
			}while(u8g_NextPage(&u8g));
		 //board_delay_ms(100, 1);
 }

void drawSonglist(){

	if(button_read(mask) == 0x04){
		button_cnt2++;
		ys+=22;
	}

	if(button_cnt2 > 9)
		button_cnt2 = 0;
	
	if(ys>44){
	ys=44;
	song_cnt++;
	if(song_cnt>7){
			song_cnt=0;
			ys=0;
	}
	}
	drawsonglist(ys,song_cnt);
}

void drawmenu(int y1){
	u8g_FirstPage(&u8g);
	do{
		u8g_DrawStr(&u8g,0, 20, menu[0]);
		u8g_DrawStr(&u8g,0, 40, menu[1]);
		u8g_DrawFrame(&u8g,0,y1,120,22);
	}while(u8g_NextPage(&u8g));
}

void drawMenu(){
	if(button_read(mask) == 0x04)
		button_cnt1++;
	if(button_cnt1 % 2 == 0)
		ym = 0;
	else
		ym = 22;  
	drawmenu(ym);
		//board_delay_ms(100, 1);
}

void drawVolume(){
	if(button_read(mask) == 0x04)
		vl+=10;

	if(vl>120)
		vl=0;

	u8g_FirstPage(&u8g);
	do{
		u8g_DrawBox(&u8g,0,22,vl,22);
		u8g_DrawStr(&u8g,0,20,"Vol");
		}while(u8g_NextPage(&u8g));
	//board_delay_ms(100, 1);
 }

void drawPlay(){
	w=u8g_GetStrWidth(&u8g,song[song_cnt]);
	playx=(120-w)/2;
	song_cnt = button_cnt2;
	if(button_read(mask) == 0x04)
		song_cnt++;
		button_cnt2 = song_cnt;
	if(song_cnt>9)
		song_cnt=0;
 
	u8g_FirstPage(&u8g);
		do{
			u8g_DrawTriangle(&u8g,0,32,20,22,20,44);
			u8g_DrawTriangle(&u8g,128,32,108,22,108,44);
			u8g_DrawCircle(&u8g,64,32,14, U8G_DRAW_ALL);
			u8g_DrawLine(&u8g,57,27,57,37);
			u8g_DrawLine(&u8g,71,27,71,37); 
			u8g_DrawStr(&u8g,playx, 20, song[song_cnt]);
			}while(u8g_NextPage(&u8g));
			 //board_delay_ms(100, 1);
}

int gui_task()
{		
	u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_COM_SSD_I2C);
	u8g_Begin(&u8g);
	while(1){
		u8g_SetFont(&u8g, u8g_font_osb18);
		switch(picture){
			case 0:
			drawMenu();
			if(button_read(mask) == 0x01){
				if(ym == 0)
					picture = 1;
				else
					picture = 3;
			}
			break;
			case 1:
			drawSonglist();
			if(button_read(mask) == 0x01){
				song_cnt_tmp=song_cnt;
				picture=2;
			}
			if(button_read(mask) == 0x02)
				picture = 0;
			break;
			case 2:drawPlay();
			if(button_read(mask) == 0x02){
				picture = 1;
				song_cnt = song_cnt_tmp;
			}
			break;
			case 3:drawVolume();
			if(button_read(mask) == 0x02)
				picture = 0;
				break;
		}

	}
	 return E_SYS;
}
