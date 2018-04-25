#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "esp8266.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

/********define in playlist.c******/
struct filelist
{
	char data[50];
	int lenth;
	struct filelist* next;
};

#define NET_LIST 0
#define FILE_LIST 1


extern struct filelist *Songid_HEAD ;
extern struct filelist *Songid_END ;

extern struct filelist *Playlist_HEAD;
extern struct filelist *Playlist_END;

extern void filelist_init();
extern void filelist_add(uint8_t list_id,char* id_data,int lenth);
extern void filelist_delete(uint8_t list_id);

/*************define in net.c****************/

#define SONG_ID 0
#define SONG_INFO 1
extern uint8_t flag_netpoll ;
extern char *net_buff;
extern uint16_t bypass_cnt;
extern ESP8266_DEF_PTR ESP8266_A;
extern void net_init();
extern int socket_request(unsigned char option);


#endif
