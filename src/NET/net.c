//net control
//DDK
//20180227

#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

#include "esp8266.h"


// #define WIFI_SSID   "\"Andrew_Sun\""
// #define WIFI_PWD    "\"smartcar\""
#define WIFI_SSID   "\"WUT-test\""
#define WIFI_PWD    "\"DDK123456\""


uint8_t flag_netpoll = 0;				//Big Data Receive Flag
uint8_t flag_netbuff = BUFF_EMPTY;		//Net Buff FULL Flag
char *net_buff;							//10MB Net Buff
uint32_t bypass_cnt = 0;				//Big Data Receive Count

char dllink[500] = { 0 };				//Store Song Download Url
char songpoint[50] = { 0 };				//Store Song Name Download Form Net ,Unnecessart Now

ESP8266_DEF __ESP8266_A;				//Define ESP8266 Control Struct
ESP8266_DEF_PTR ESP8266_A = &__ESP8266_A;


/***
**	Start to Receive File
**
*/
inline static void START_REC()
{	
	bypass_cnt = 0;
	flag_netpoll = 1;
}

/***
**	End Receive File
**
**/
inline static void END_REC()
{

	bypass_cnt = 0;
	flag_netpoll = 0;
}

/**
** Deal with Receive Buff and extract Song ID 
**
*/

static int get_songid(char *jsonstr)
{
	char *string = (char*)jsonstr;
	char songid[12] = "";

 /*************Get Id Here only 10 ,Can Change to Get More Song once Request***************************************/
	uint8_t i = 10,j = 0;
	while(i--)
	{
		string = strstr(string,"\"id\":");
		if ( string == NULL )
		{
			EMBARC_PRINTF("String End\r\n");
			break;
		}
		string += 5;

/******************Caution "+IPD" May Appear in the string"*****************************************/
		for (j = 0 ; (*string) != ',' ;j++,string ++ )
		{
			songid[j] = *(string);
		}
		songid[j] = '\0';
		EMBARC_PRINTF("\r\n%s\r\n",songid);

		/***建议两张表分开写******/
		filelist_add(NET_LIST,songid,0,0);

	}
	if ( Songid_HEAD == Songid_END )
	{
		return -1;
	}
	return 0;
}

/**
**	Deal with Receive Buff and exract Song Information
**	Only Download Url useful
**/

static int get_songinfo(char *jsonstr)
{
	char *string = (char*)jsonstr;
	char *string_p1; 
	char *string_p2 ;

	char queryId[20] = { 0 };
	// char songName[50] = {0};
	char artistName[50] = { 0 };
	char albumName[50] = { 0 };
	char lrcLink[500] = { 0 };
	char songLink[500] = { 0 };

	uint16_t i = 0,j = 0;;

	memset(dllink, 0, sizeof(char) * 500);
	memset(songpoint, 0, sizeof(char) * 50);

	/***********queryId************/
	string_p2 = strstr(string,"\"queryId\":");
	string_p1 = string_p2 + 11;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(queryId,string_p1,(uint8_t)(string_p2 - string_p1));
	EMBARC_PRINTF("\r\n%s\r\n",queryId);

	/***********songName****Need Protect********/
	string_p2 = strstr(string,"\"songName\":");
	string_p1 = string_p2 + 12;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(songpoint,string_p1,(uint8_t)(string_p2 - string_p1));
	EMBARC_PRINTF("\r\n%s\r\n",songpoint);

	/***********artistName************/
	string_p2 = strstr(string,"\"artistName\":");
	string_p1 = string_p2 + 14;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(artistName,string_p1,(uint8_t)(string_p2 - string_p1));
	EMBARC_PRINTF("\r\n%s\r\n",artistName);


	/***********albumName************/
	string_p2 = strstr(string,"\"albumName\":");
	string_p1 = string_p2 + 13;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(albumName,string_p1,(uint8_t)(string_p2 - string_p1));
	EMBARC_PRINTF("\r\n%s\r\n",albumName);

	/***********lrcLink*****unformat*******/
	string_p2 = strstr(string,"\"lrcLink\":");
	string_p1 = string_p2 + 11;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(lrcLink,string_p1,(uint8_t)(string_p2 - string_p1));
	EMBARC_PRINTF("\r\n%s\r\n",lrcLink);

	/***********songLink****Need proterct********/
	string_p2 = strstr(string,"\"songLink\":");
	string_p1 = string_p2 + 12;
	string_p2 = strstr(string_p1,"\",\"");
	strncpy(songLink,string_p1,(uint8_t)(string_p2 - string_p1));

	for ( i = 0 , j = 0; songLink[i] != '\0'; i++ )
	{
		if ( songLink[i] != '\\' )
		{
			dllink[j] = songLink[i];
			j++;
		}
	}
	EMBARC_PRINTF("\r\n%s\r\n",dllink);

	return 0;
}

/***
** 302 Redirect dure to hust school Network Mirror
**	do not need to use anymore
**/


// static int get_302(char *buff)
// {
// 	char *string_p1; 
// 	char *string_p2 ;

// 	string_p1 = strstr(buff,"http://");
// 	if ( string_p1 == NULL )
// 	{
// 		EMBARC_PRINTF("No http\r\n");
// 		while(1);
// 		return -1;
// 	}
// 	string_p2 = strstr(string_p1,"\r\n");
// 	if ( string_p1 == NULL )
// 	{
// 		EMBARC_PRINTF("No rn\r\n");
// 		while(1);
// 		return -1;
// 	}
// 	memset(dllink,0,sizeof(char) * 500);
// 	strncpy(dllink,string_p1,(uint8_t)(string_p2 - string_p1));
// 	return 0;
// }
// static uint32_t fix_netbuff(char *net_buff,uint32_t length)
// {
// 	char *buff_p1;
// 	char *buff_p2;

// 	uint32_t i = 0;
// 	uint8_t j = 0;
// 	uint32_t len = length;

// 	for ( i = 0;i < len; i++ )
// 	{
// 		if ( ( *(net_buff + i ) == '+' ) 
// 			&& ( *(net_buff + i + 1 ) == 'I' )
// 			&& ( *(net_buff + i + 2 ) == 'P' )
// 			&& ( *(net_buff + i + 3 ) == 'D' ) )
// 		{
// 		/*********Record Start Point***************/
// 			buff_p1 = net_buff + i;	

// 			j = 4;

// 			/*****Just a Protection*************/
// 			while( ( i+j ) < len )
// 			{
// 				***':' only one****
// 				if ( *(net_buff + i + j ) != ':' )
// 				{
// 					j++;
// 				}
// 				/*************(net_buff + i + j ) == ':'**********************/
// 				else
// 				{
// 					/*****next char***********/
// 					j++;
// 					buff_p2 = net_buff + i + j;

// 					/***********i is correct length***********************/
// 					memmove(buff_p1 ,buff_p2,len - i - j);
// 					/***********j is cut lenth******************************/
// 					len -= j;
// 					EMBARC_PRINTF("\r\ncut %d \r\n",j);
// 					break;
// 				}
// 			}

// 		}
// 	}

// 	return len;
// }
/**
**	 Init ESP8266 and Malloc 10MB net buff and Connect to WIfi
**
**/

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


    esp8266_init(ESP8266_A, UART_BAUDRATE_230400);
    at_test(ESP8266_A->p_at);


	_Rtos_Delay(100);

    // //Set Mode
    EMBARC_PRINTF("============================ Set Mode ============================\n");
    esp8266_wifi_mode_get(ESP8266_A, false);

    _Rtos_Delay(100);

    esp8266_wifi_mode_set(ESP8266_A, 3, false);

	_Rtos_Delay(100);

    //Connect WiFi
    EMBARC_PRINTF("============================ Connect WiFi ============================\n");

    while(esp8266_wifi_connect(ESP8266_A, WIFI_SSID, WIFI_PWD, false)!=AT_OK)
    {
        EMBARC_PRINTF("WIFI %s connect failed\n", WIFI_SSID);
        _Rtos_Delay(1000);
    }
    EMBARC_PRINTF("WIFI %s connect succeed\n", WIFI_SSID);

    //Show IP
    EMBARC_PRINTF("============================ Show IP ============================\n");
    esp8266_address_get(ESP8266_A);
    
    // _Rtos_Delay(100);
	
}

/***********
**获取Song ID 或者 下载地址**
**option：0 SONGID ;1 DOWNLOAD LINK
***/

int socket_request(unsigned char option)
{
	char *http_cmd;
	uint32_t idlen_int;
	char idlen_char[3] = "";

	TickType_t xLastWakeTime;

    EMBARC_PRINTF("============================ connect socket ============================\n\r");
	esp8266_tcp_connect(ESP8266_A,"180.76.141.217", 80);

    http_cmd = (char *)malloc(sizeof(char) * 500);
    memset(http_cmd, 0, sizeof(char) * 500);
	memset(net_buff, 0, sizeof(char) * 10 * 1024 * 1024);

    EMBARC_PRINTF("============================ create http command ============================\r\n");
    switch (option)
    {
    	case SONG_ID:
			strcat (http_cmd,"GET http://fm.baidu.com/dev/api/?tn=playlist&id=public_tuijian_rege&hashcode=&_=1519727783752 HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\n\r\n");
			break;
    	case SONG_INFO:
			strcat (http_cmd,"POST http://fm.baidu.com/data/music/songlink HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\nContent-Length: ");
			
			idlen_int = strlen(Songid_HEAD->data);
			idlen_int += 8;
			
			itoa(idlen_int,idlen_char,10);
			strcat(http_cmd,idlen_char);
			strcat(http_cmd,"\r\n\r\nsongIds=");

			strcat (http_cmd,Songid_HEAD->data);
			if ( Songid_HEAD->next != NULL )
			{
				filelist_delete(NET_LIST);
			}
			else
			{
				EMBARC_PRINTF("Net List Empty\r\n");
			}
			break;
    }
    
    EMBARC_PRINTF("\r\n%s\r\n",http_cmd);

    vTaskSuspendAll();

    /*****Enable passthrough to Deal with +IPD flag********/
    esp8266_passthr_start(ESP8266_A);
	esp8266_passthr_write( ESP8266_A, http_cmd,strlen(http_cmd) );
	// esp8266_normal_write( ESP8266_A, http_cmd,strlen(http_cmd) );
	START_REC();				//Set Flag for isr to Receive Buff Directly
	xTaskResumeAll();

    free(http_cmd);


	EMBARC_PRINTF("======================== Get all Data Driectly===================\r\n");
	_Rtos_Delay(1000);			//Wait Data to Arrive

    EMBARC_PRINTF("%s\r\n",(net_buff));

	/*********Receive Complete , Reset Flag and Disable Passthrough***************/
	END_REC();
	esp8266_passthr_end(ESP8266_A);
	_Rtos_Delay(100);
	esp8266_transmission_mode(ESP8266_A,ESP8266_NORMALSEND);

	/*********Exract Data From Buff******************************/
	switch(option)
	{
		case SONG_ID :
			if (-1 == get_songid(net_buff))
			{
				return -1;
			}
			break;
		case SONG_INFO:
			if (-1 == get_songinfo(net_buff))
			{
				return -1;
			}
			break;
	}	

	EMBARC_PRINTF("Recv Done.\r\n");
	esp8266_CIPCLOSE(ESP8266_A);

	return 0;
}


DEV_UART_PTR uart_obj;

/***
**Download MP3 use Download link
**
**/


void download_mp3()
{
	uint8_t http_cnt = 0;
	uint32_t http_sum = 0;
	// uint32_t cur_time;
	char *http_cmd;
	uint8_t timeout_cnt = 0;

	uint32_t net_time = 0;
	uint32_t net_time_pre = 0;
	


	DEV_BUFFER Rxintbuf;

	DEV_BUFFER_INIT(&Rxintbuf, net_buff, sizeof(char) * 10 * 1024 * 1024);
	uart_obj = uart_get_dev(ESP8266_UART_ID);

	EMBARC_PRINTF("============================ connect socket ============================\n\r");
	esp8266_tcp_connect(ESP8266_A,"211.91.125.36", 80);

    http_cmd = (char *)malloc(sizeof(char) * 500);
    memset(http_cmd, 0, sizeof(char) * 500);
	memset(net_buff, 0, sizeof(char) * 10 * 1024 * 1024);

    strcat (http_cmd,"GET ");
    strcat (http_cmd,dllink);
    strcat (http_cmd," HTTP/1.1\r\nHost: zhangmenshiting.qianqian.com\r\nConnection: keep-alive\r\n\r\n");

    EMBARC_PRINTF("\r\n%s\r\n",http_cmd);


	/*****Enable passthrough to Deal with +IPD flag********/
	vTaskSuspendAll();
    esp8266_passthr_start(ESP8266_A);
	esp8266_passthr_write( ESP8266_A, http_cmd,strlen(http_cmd) );	

	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, (void*)(&Rxintbuf));

	START_REC();
	xTaskResumeAll();

    free(http_cmd);

	net_time_pre = xTaskGetTickCount ();
	while(1)
	{
		_Rtos_Delay(1000);

		net_time = xTaskGetTickCount ();

		bypass_cnt = uart_obj -> uart_info.rx_buf.ofs;//read the buff offset has received
		
    	if ( http_sum != bypass_cnt  )
    	{
    		EMBARC_PRINTF("received : %d KB\r",bypass_cnt / 1024 );
			EMBARC_PRINTF("received : %d KB/s\r",( bypass_cnt - http_sum ) / 1024 / ( net_time - net_time_pre ) );
			http_sum = bypass_cnt;
			timeout_cnt = 0;
    	}
    	else
    	{
    		timeout_cnt ++;
    		EMBARC_PRINTF("\r\nTime out\r\n");
    		if ( timeout_cnt > 3 )
    		{
				EMBARC_PRINTF("\r\nreceive end , %d B\r\n",bypass_cnt  );
				EMBARC_PRINTF("\r\n%s \r\n",net_buff);
	    		break;
    		}
    	}
    	net_time_pre = net_time;
	}

	/*********Receive Complete , Reset Flag and Disable Passthrough***************/
	esp8266_passthr_end(ESP8266_A);
	_Rtos_Delay(100);
	esp8266_transmission_mode(ESP8266_A,ESP8266_NORMALSEND);
	END_REC();

	http_sum = 10*1024*1024;
	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, NULL);
	_Rtos_Delay(100);
	filelist_add(FILE_LIST,songpoint,http_sum,IN_BUFF);
	flag_netbuff = BUFF_FULL;

	EMBARC_PRINTF("Socket Close.\r\n");

	/**********Connect will Close Automatic*********************/
	esp8266_CIPCLOSE(ESP8266_A);
	
}




