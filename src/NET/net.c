//net control
//DDK
//20180227

#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

#include "esp8266.h"


#define WIFI_SSID   "\"Andrew_Sun\""
#define WIFI_PWD    "\"smartcar\""


// static char http_get[] = "GET /";
// static char http_IDP[] = "+IPD,";
// static char http_html_header[] = "HTTP/1.x 200 OK\r\nContent-type: text/html\r\n\r\n";
// static char http_html_body_1[] = "<html><head><title>ESP8266_AT_HttpServer</title></head><body><h1>Welcome to this Website</h1>";
// static char http_html_body_2[] = "<p>This Website is used to test the AT command about HttpServer of ESP8266.</p></body></html>";

// static char cmd_getlist = "GET http://fm.baidu.com/dev/api/?tn=playlist&id=public_tuijian_rege&hashcode=&_=1519727783752 HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\n\r\n";


uint8_t flag_netpoll = 0;
char *net_buff;
uint16_t bypass_cnt = 0;
ESP8266_DEF __ESP8266_A;
ESP8266_DEF_PTR ESP8266_A = &__ESP8266_A;

static int get_songid(const char *jsonstr)
{
	char *string = (char*)jsonstr;
	char songid[12] = "";

 /*************Get Id Here only 10 ,Can Change to Get More Song once Request***************************************/
	uint8_t i = 10;
	while(i--)
	{
		string = strstr(string,"\"id\":");
		if ( string == NULL )
		{
			EMBARC_PRINTF("String End\r\n");
			break;
		}
		string += 5;

		for (i = 0 ; (*string) != ',' ;i++,string ++ )
		{
			songid[i] = *(string);
		}
		songid[i] = '\0';
		EMBARC_PRINTF("\r\n%s\r\n",songid);

		/***建议两张表分开写******/
		filelist_add(NET_LIST,songid,0);

	}
	if ( Songid_HEAD == Songid_END )
	{
		return -1;
	}
	return 0;
}



// int get_songinfo(const char *jsonstr)
// {
// 	JSON_Value *json_1 ;
// 	JSON_Value *json_2 ;
// 	JSON_Value *json_3 ;
// 	JSON_Value *json_music ;
// 	json_1 = json_parse_string(jsonstr);


// 	if ( json_1 == NULL )
// 	{
// 		return -1;
// 	}
// 	// if( json->type != 4 || (*(json->value.object->values))->type != 3)
// 	// {
// 	// 	printf("It's not a vaild id");
// 	// 	return -1;
// 	// }

// 	// music_data.dl_url = (*(json->value.object->values+1))->value.string;
// 	// music_data.album_title = (*(json->value.object->values+19))->value.string;
// 	// music_data.title = (*(json->value.object->values+5))->value.string;

// 	// printf ("download_url:%s\n",music_data.dl_url);
// 	// printf ("album:%s\n",music_data.album_title);
// 	// printf ("title:%s\n",music_data.title);

// 	uartpc.printf("%d\n\r",json_1->type); //type == 4 是json类型

// 	uartpc.printf("%d\n\r",(*(json_1->value.object->values+1))->type);//第1个josn里的第二项的类型

// 	uartpc.printf("%s\n\r",(*(json_1->value.object->names+1)));//第1个josn里的第二项名称


// 	json_2 = *(json_1->value.object->values+1); //json ->data(json)
// 	json_3 = *(json_2->value.object->values+2); //json ->data(json) -> songlist(json)

// 	//uartpc.printf("%s\n\r",(*(json_3->value.object->names+1)));//第3个josn（songlist）里的第二项名称（2）

// //此处需要一个遍历(所有的歌)

// 	char i = 0;

// 	char music_cnt = json_3->value.array->count;
// 	//for (i)
// 	{
// 		json_music = *(json_3->value.array->items + i);


// 		const char* songid = json_object_get_string (json_music->value.object, "queryId");
// 		const char* SongName = json_object_get_string (json_music->value.object, "songName");
// 		const char* ArtistName = json_object_get_string (json_music->value.object, "artistName");
// 		const char* AlbumName = json_object_get_string (json_music->value.object, "albumName");
// 		const char* LrcLink = json_object_get_string (json_music->value.object, "lrcLink");
// 		const char* SongLink = json_object_get_string (json_music->value.object, "songLink");

// 		uartpc.printf("cnt：\n\r%d\n\r",music_cnt);//歌曲数组里有多少项
// 		// uartpc.printf("SongId:%s\n\r",((*(json_music->value.object->values))->value.string));//第1个歌曲id
// 		uartpc.printf("SongId:%s\n\r",songid);//第1个歌曲id

// 		uartpc.printf("SongName:%s\n\r",SongName);//第1个歌曲id
// 		uartpc.printf("ArtistName:%s\n\r",ArtistName);//第1个歌曲id
// 		uartpc.printf("AlbumName:%s\n\r",AlbumName);//第1个歌曲id
// 		uartpc.printf("LrcLink:%s\n\r",LrcLink);//第1个歌曲id
// 		uartpc.printf("SongLink:%s\n\r",SongLink);//第1个歌曲id

// 		memset(dllink, 0, sizeof(char) * 500);
// 		memset(songpoint, 0, sizeof(char) * 50);
// 		strcat(dllink,SongLink);
// 		strcat(songpoint,SongName);
// 		uartpc.printf("\n\rdownlink:%s\n\r" ,dllink);
// 	}

// 	return 0;
// }






void net_init()
{
	EMBARC_PRINTF("============================ Init ============================\n");
	
	net_buff = malloc(sizeof(char) * 10 * 1024 * 1024);
	if ( net_buff == NULL )
	{
		EMBARC_PRINTF("Malloc Net Buff Fail!\r\nstop!\r\n");
		while(1);
	}
	else
	{
		memset( net_buff, 0, sizeof(char) * 10 * 1024 * 1024 );
		EMBARC_PRINTF("Malloc Net Buff Pass!\r\n");
	}


    esp8266_init(ESP8266_A, UART_BAUDRATE_115200);
    at_test(ESP8266_A->p_at);
    //vTaskDelay( 1 );

    // //Set Mode
    EMBARC_PRINTF("============================ Set Mode ============================\n");
    esp8266_wifi_mode_get(ESP8266_A, false);
    //vTaskDelay( 1 );
    esp8266_wifi_mode_set(ESP8266_A, 3, false);
    //vTaskDelay( 1 );

    //Connect WiFi
    EMBARC_PRINTF("============================ Connect WiFi ============================\n");

    // do
    // {
    //     esp8266_wifi_scan(esp8266, scan_result);
    //     EMBARC_PRINTF("Searching for WIFI %s ......\n", WIFI_SSID);
    //     vTaskDelay( 1 );
    //     // board_delay_ms(100, 1);
    // }
    // while (strstr(scan_result, WIFI_SSID)==NULL);

    //EMBARC_PRINTF("WIFI %s found! Try to connect\n", WIFI_SSID);
    while(esp8266_wifi_connect(ESP8266_A, WIFI_SSID, WIFI_PWD, false)!=AT_OK)
    {
        EMBARC_PRINTF("WIFI %s connect failed\n", WIFI_SSID);
        //vTaskDelay( 1 );
    }
    EMBARC_PRINTF("WIFI %s connect succeed\n", WIFI_SSID);

    //Show IP
    EMBARC_PRINTF("============================ Show IP ============================\n");
    esp8266_address_get(ESP8266_A);
    
    // vTaskDelay( 1 );
	
}

/*************获取下载地址*******/

#define REC_BUFF_SIZE (4096)
#define REC_FIFO_SIZE (100)
int socket_request(unsigned char option)
{
	int http_cnt;
	char *http_cmd;
	char id_char[10] = {0};
	int idlen_int;
	char idlen_char[3] = "";
	char *rec_buf;
	char *response;
	int flag = 0;
	int i;
	uint16_t rec_sum = 0;
	uint32_t cur_time ;
    // socket.connect("180.76.141.217", 80);

    EMBARC_PRINTF("============================ connect socket ============================\n\r");
	esp8266_tcp_connect(ESP8266_A,"180.76.141.217", 80);

    http_cmd = (char *)malloc(sizeof(char) * 500);
    memset(http_cmd, 0, sizeof(char) * 500);


    EMBARC_PRINTF("============================ create http command ============================\r\n");
    switch (option)
    {
    	case 0:
			strcat (http_cmd,"GET http://fm.baidu.com/dev/api/?tn=playlist&id=public_tuijian_rege&hashcode=&_=1519727783752 HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\n\r\n");
			break;
    	case 1:
			// strcat (http_cmd,"POST http://fm.baidu.com/data/music/songlink HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\nContent-Length: ");
			
			// idlen_int = strlen(Songid_HEAD->data);
			// idlen_int += 8;
			
			// itoa(idlen_int,idlen_char,10);
			// strcat(http_cmd,idlen_char);
			// strcat(http_cmd,"\r\n\r\nsongIds=");

			// strcat (http_cmd,Songid_HEAD->data);
			// list_delete(0);
			break;
    }
    
    EMBARC_PRINTF("\r\n%s\r\n",http_cmd);
    // http_cnt = socket.send(http_cmd, strlen(http_cmd));
	esp8266_normal_write( ESP8266_A, http_cmd,strlen(http_cmd) );
	flag_netpoll = 1;//start to poll
    free(http_cmd);

    
    // EMBARC_PRINTF("OSP_GET_CUR_MS()= =======%d====================\r\n",OSP_GET_CUR_MS());
	EMBARC_PRINTF("============================ Find header ============================\r\n");
	rec_buf = (char *)malloc(sizeof(char) * REC_FIFO_SIZE);
	response = (char *)malloc(sizeof(char) * REC_BUFF_SIZE);
	memset(response, 0, sizeof(char) * REC_BUFF_SIZE);
	clear_recbuf(ESP8266_A);
    
	/************NEED USE un-Block delay here Here***********************/
	cur_time = OSP_GET_CUR_MS();
	while( OSP_GET_CUR_MS() - cur_time < 5000 );

	// do

    	// memset(rec_buf, 0, sizeof(char) * REC_FIFO_SIZE);

    EMBARC_PRINTF("%s\r\n",(net_buff));
	
	flag_netpoll = 0;//end to poll
	bypass_cnt = 0;
	/*******************Fail to Get Header*********************************/


	/*******************END Fail to Get Header*********************************/


	EMBARC_PRINTF("============================ Get Response ============================\r\n");
	memset(response, 0, sizeof(char) * REC_BUFF_SIZE);





	//skip error head until "{" appear

	while(*rec_buf != '{' /*&& 8266 protect*/ );

	strcat(response,"{\0");

	EMBARC_PRINTF("============================ Extract Information ============================\r\n");

	switch(option)
	{
		case SONG_ID :
			if (-1 == get_songid(response))
			{
				return -1;
			}
			break;
		case SONG_INFO:
			// if (-1 == get_songinfo(response))
			// {
			// 	return -1;
			// }
			break;
	}

	while(1);
	free(rec_buf);
	free(response);
	EMBARC_PRINTF("Recv Done.\r\n");
	// socket.close();
	esp8266_CIPCLOSE(ESP8266_A, "\0");

	return 0;
}

// void download_mp3()
// {
// 	TCPSocket socket;
// 	int rcount;

// 	socket.open(&net);
//     socket.connect("211.91.125.36", 80);

//     char *http_cmd = (char *)malloc(sizeof(char) * 500);
//     memset(http_cmd, 0, sizeof(char) * 500);

//     strcat (http_cmd,"GET ");
//     strcat (http_cmd,dllink);
//     strcat (http_cmd," HTTP/1.1\r\nHost: zhangmenshiting.qianqian.com\r\nConnection: keep-alive\r\n\r\n");

//     int scount = socket.send(http_cmd, strlen(http_cmd));
//     free(http_cmd);

//     char *rec_buf = (char *)malloc(sizeof(char) * 8);
// 	char *response = (char *)malloc(sizeof(char) * REC_BUFF_SIZE);
// 	memset(response, 0, sizeof(char) * REC_BUFF_SIZE);
// 	while(1)
// 	{
//     	memset(rec_buf, 0, sizeof(char) * 8);
//     	rcount = socket.recv(rec_buf, 1);
//     	if ( rcount <= 0 )
// 		{
// 			// free(rec_buf);
// 			uartpc.printf("error in header");
// 			break;
//     	}
//     	rec_buf[1]='\0';
//     	strcat(response,rec_buf);

// 		//找到响应头的头部信息, 两个"\n\r"为分割点
// 		int flag = 0;
// 		for (int i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
// 		if (flag == 4)
// 		{
// 			uartpc.printf("final header is:\n\r %s\r\n",response);
			
// 			break;
// 		}	
// 	}

// 	memset(response, 0, sizeof(char) * REC_BUFF_SIZE);
// 	char filename[50] = "/fs/";
// 	strcat(filename,songpoint); 
// 	strcat(filename,".mp3");
// 	FILE* fd = fopen(filename, "w");
// 	if (fd == NULL)
// 	{
// 		uartpc.printf(" Failure. %d \r\n", errno);
// 		return;
// 	}
// 	else
// 	{
// 		uartpc.printf(" done.\r\n");
// 	}

// 	int rec_cnt = 0;
// 	while(1)
// 	{
//     	memset(response, 0, sizeof(char) * 8);
//     	rcount = socket.recv(response, 1000);
//     	if ( rcount <= 0 )
// 		{			
// 			uartpc.printf("download end\n\r");
// 			break;
//     	}
//     	fwrite(response,1,rcount,fd); 
//     	rec_cnt +=  rcount ;	
//     	uartpc.printf("received : %d KB\r",rec_cnt/1024);
// 	}
// 	uartpc.printf("\r\nreceive end \r\n");
// 	fclose(fd);
// 	strcat(songpoint,".mp3");
// 	list_add(1,songpoint);

// 	free(rec_buf);
// 	free(response);
// 	uartpc.printf("recv done.\r\n");
// 	socket.close();
// }




