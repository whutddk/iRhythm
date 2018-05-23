/**
 * API CODE for 2 List 
 * DDK
 * 2018 03 10
 */

#include "embARC.h"
#include "embARC_debug.h"
#include "stdlib.h"

#include "include.h"

struct filelist *Songid_HEAD = NULL ;			//Head Pointer For Songid List
struct filelist *Songid_END = NULL;				//Tail Pointer to Protect Songid List

struct filelist *Playlist_HEAD = NULL ;			//Head Pointer For Playlist List
struct filelist *Playlist_END = NULL;			//Tail Pointer to Protect Playlist List

/**
 * \brief       Initialize Song ID list and Add Default Song ID
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
 * \param[in]   list_id                (NET_LIST) 0: Add Data to Song Id LIST; 
 *                                     (FILE_LIST)1: Add Data to Playlist LIST
 *                                     
 * \param[in]   id_data                ID Number to Song Id List 
 *                                     or File Name to Playlist List
 *                                     
 * \param[in]   lenth                  File Lenth Add to Playlist List 
 *                                     No Usage in Song Id List
 *                                     
 * \param[in]   location               File Location Add to Playlist List 
 *                                     IN_BUFF/IN_FILE
 *                                     No Usage in Song Id List
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
			lists -> next = NULL;						//Add into the End of List 
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
				lists -> next = Playlist_HEAD -> next; 	//Add as the Second Node
				Playlist_HEAD -> next = lists ; 		//Head of List No Change
			} else {
				lists -> next = Playlist_HEAD ; 		//Add as the First Node
				Playlist_HEAD = lists;					//Head of List Point here
			}

			break;
	}
}


/**
 * \brief       Delete Head of a list
 *
 * \param[in]   list_id                (NET_LIST) 0 :Delete Song Id List
 *                                     (FILE_LIST)1 :Delete Playlist List
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
