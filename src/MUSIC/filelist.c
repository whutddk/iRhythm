#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"

struct filelist *Songid_HEAD = NULL ;
struct filelist *Songid_END = NULL;

struct filelist *Playlist_HEAD = NULL ;
struct filelist *Playlist_END = NULL;

/**
 * \brief       Init Song ID list and Add Default Song ID
 *
 */
void filelist_init()
{
	struct filelist *lists = NULL;

	lists = (struct filelist *)malloc(sizeof(struct filelist));
	memset(lists, 0, sizeof(struct filelist));

	if ( NULL == lists ) {
		EMBARC_PRINTF("\r\nSong Id List Init Error!\r\n");
	}

	strcat(lists -> data, "15368282"); //14950804//244200//1271193
	lists -> next = NULL;
	Songid_HEAD = lists;
	Songid_END = lists;

}


/**
 * \brief       Add data in List
 *
 *
 * \param[in]   list_id                0: add data to Song id LIST; 
 *                                     1: add data to Playlist LIST
 *                                     
 * \param[in]   id_data                ID Number to Song id list 
 *                                     or File Name to playlist list
 *                                     
 * \param[in]   lenth                  File lenth add to playlist list 
 *                                     no usage in Song id list
 *                                     
 * \param[in]   location               File location add to playlist list 
 *                                     IN_BUFF/IN_FILE
 *                                     no usage in Song id list
 *
 */
void filelist_add(uint8_t list_id, char *id_data, int32_t lenth, uint8_t location)
{
	struct filelist *lists = NULL;

	switch (list_id) {
		case (NET_LIST):
			lists = (struct filelist *)malloc(sizeof(struct filelist));
			memset(lists, 0, sizeof(struct filelist));

			if ( NULL == lists ) {
				EMBARC_PRINTF("\r\nSong Id List Malloc Error!\r\n");
			}

			strcat(lists -> data, id_data);
			lists -> next = NULL;//插入到表尾

			Songid_END -> next = lists;
			Songid_END = Songid_END -> next;
			break;

		case (FILE_LIST):
			lists = (struct filelist *)malloc(sizeof(struct filelist));
			memset(lists, 0, sizeof(struct filelist));

			if ( NULL == lists ) {
				EMBARC_PRINTF("\r\nPlay List Malloc Error!\r\n");
			}

			strcat(lists -> data, id_data);
			lists->lenth = lenth;
			lists->location = location;

			if ( location == IN_BUFF ) {
				lists -> next = Playlist_HEAD -> next; //插入到第二节点上
				Playlist_HEAD -> next = lists ; //链表头不变
			} else {
				lists -> next = Playlist_HEAD ; //插入到第一节点上
				Playlist_HEAD = lists;			//成为链表头
			}

			break;
	}
}


/**
 * \brief       Delete Head of a list
 *
 * \param[in]   list_id                0 :Delete Song id List
 *                                     1 :Delete Playlist list
 *
 */
void filelist_delete(uint8_t list_id)
{
	struct filelist *lists = NULL;

	switch (list_id) {
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