#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"

struct filelist *Songid_HEAD = NULL ;
struct filelist *Songid_END = NULL;

struct filelist *Playlist_HEAD = NULL ;
struct filelist *Playlist_END = NULL;


/**
** Init Song ID list and Add Default Song ID 
**
*/
void filelist_init()
{
	struct filelist *lists = NULL;

	lists = (struct filelist *)malloc(sizeof(struct filelist));
	memset(lists, 0, sizeof(struct filelist));
	if ( NULL == lists )
	{
		EMBARC_PRINTF("\r\nSong Id List Init Error!\r\n");
	}

	strcat(lists -> data,"15368282");//14950804//244200//1271193
	lists -> next = NULL;
	Songid_HEAD = lists;
	Songid_END = lists;

}

//每获得一个歌曲id则后面加一个链
/****
**	Add data in List
** LIST_ID :0 =  Song id LIST; 1=Playlist LIST
** ID_DATA :ID Number or File Name 
** lenth : NO use;File Length
** location: no use ;File in SD card or Net BUFF
**/
void filelist_add(uint8_t list_id,char* id_data,int lenth,uint8_t location)
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
			lists->location = location;
			lists -> next = Playlist_HEAD;

			// struct filelist *temp = Playlist_HEAD;
			Playlist_HEAD = lists;
			// Playlist_END = Playlist_END -> next;
			break;
	}
}

//每获得一首歌的信息，删掉一个头节点
/**
** Delete Head of a list
**list_id： 0 =  Delete Song id List ;1 = Delete Playlist list 
*/
void filelist_delete(uint8_t list_id)
{
	struct filelist *lists = NULL;
	switch (list_id)
	{
		case (NET_LIST):
			lists = Songid_HEAD;
			Songid_HEAD = Songid_HEAD -> next;
			free (lists);
			break;
		case (FILE_LIST):
			lists = Playlist_HEAD;
			Playlist_HEAD = Playlist_HEAD -> next;
			free (lists);
			break;
	}
}