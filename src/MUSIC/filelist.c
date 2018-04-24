#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"

struct filelist *Songid_HEAD = NULL ;
struct filelist *Songid_END = NULL;

struct filelist *Playlist_HEAD = NULL ;
struct filelist *Playlist_END = NULL;

void filelist_init()
{
	struct filelist *lists = NULL;

	lists = (struct filelist *)malloc(sizeof(struct filelist));
	memset(lists, 0, sizeof(struct filelist));
	if ( NULL == lists )
	{
		EMBARC_PRINTF("\r\nSong Id List Init Error!\r\n");
	}

	strcat(lists -> data,"1271193");//14950804//244200//1271193
	lists -> next = NULL;
	Songid_HEAD = lists;
	Songid_END = lists;

}

//每获得一个歌曲id则后面加一个链
void filelist_add(uint8_t list_id,char* id_data,int lenth)
{
	struct filelist *lists = NULL;

	switch (list_id)
	{
		case (NET_LIST):
			lists =(struct filelist *)malloc(sizeof(struct filelist));
			memset(lists, 0, sizeof(struct filelist));
			if ( NULL == lists )
			{
				EMBARC_PRINTF("\r\nSong Id List Malloc Error!\r\n");
			}

			strcat(lists -> data,id_data);
			lists -> next = NULL;

			Songid_END -> next = lists;
			Songid_END = Songid_END -> next;
			break;

		case (FILE_LIST):
			lists =(struct filelist *)malloc(sizeof(struct filelist));
			memset(lists, 0, sizeof(struct filelist));
			if ( NULL == lists )
			{
				EMBARC_PRINTF("\r\nPlay List Malloc Error!\r\n");
			}

			strcat(lists -> data,id_data);
			lists->lenth = lenth;
			lists -> next = Playlist_HEAD;

			// struct filelist *temp = Playlist_HEAD;
			Playlist_HEAD = lists;
			// Playlist_END = Playlist_END -> next;
			break;
	}
}

//每获得一首歌的信息，删掉一个头节点
void filelist_delete(uint8_t list_id)
{
	struct filelist *lists = NULL;
	switch (list_id)
	{
		case (0):
			lists = Songid_HEAD;
			Songid_HEAD = Songid_HEAD -> next;
			free (lists);
			break;
		case (1):
			lists = Playlist_HEAD;
			Playlist_HEAD = Playlist_HEAD -> next;
			free (lists);
			break;
	}
}