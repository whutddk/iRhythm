/**
 * API CODE for 2 List 
 * DDK
 * 2018 03 10
 */

#include "embARC.h"

//#define DBG_MORE
#include "embARC_debug.h"

#include "stdlib.h"

#include "include.h"


// struct filelist *Songid_HEAD = NULL ;			//Head Pointer For Songid List
// struct filelist *Songid_END = NULL;				//Tail Pointer to Protect Songid List

struct filelist *Playlist_HEAD = NULL ;			//Head Pointer For Playlist List
struct filelist *Playlist_END = NULL;			//Tail Pointer to Protect Playlist List

// /**
//  * \brief       Initialize Song ID list and Add Default Song ID
//  *
//  */
// void filelist_init()
// {
// 	struct filelist *lists = NULL;

// 	lists = (struct filelist *)malloc(sizeof(struct filelist));
// 	memset(lists, 0, sizeof(struct filelist));

// 	if ( NULL == lists ) {
// 		dbg_printf(DBG_LESS_INFO,"\r\nSong Id List Init Error!\r\n");
// 	}

// 	strcat(lists -> data, "15368282"); //14950804//244200//1271193
// 	lists -> next = NULL;
// 	Songid_HEAD = lists;
// 	Songid_END = lists;

// }


/**
 * \brief       Add data in List
 *
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
void filelist_add(char *id_data, int32_t lenth, uint8_t location)
{
	struct filelist *linklists = NULL;
	linklists = (struct filelist *)malloc(sizeof(struct filelist));
	if ( NULL == linklists ) {
		dbg_printf(DBG_LESS_INFO,"\r\nPlay List Malloc Error!\r\n");
	}
	memset(linklists, 0, sizeof(struct filelist));
	strcat(linklists -> data, id_data);

	linklists->lenth = lenth;
	linklists->location = location;
	
	if ( location == IN_NET ) {
		linklists -> next = Playlist_HEAD -> next; 	//Add as the Second Node
		Playlist_HEAD -> next = linklists ; 		//Head of List No Change
	} else {
		linklists -> next = Playlist_HEAD ; 		//Add as the First Node
		Playlist_HEAD = linklists;					//Head of List Point here
	}

}


/**
 * \brief       Delete Head of a list
 *
 * \param[in]   list_id                (NET_LIST) 0 :Delete Song Id List
 *                                     (FILE_LIST)1 :Delete Playlist List
 *
 */
void filelist_delete()
{
	struct filelist *linklists = NULL;

	linklists = Playlist_HEAD;
	Playlist_HEAD = Playlist_HEAD -> next;
	free (linklists);

}
