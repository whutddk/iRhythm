//net control
//DDK
//20180227

#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

#include "esp8266.h"


#define WIFI_SSID   "\"Andrew_Sun\""
#define WIFI_PWD    "\"smartcar\""
// #define WIFI_SSID   "\"WUT-test\""
// #define WIFI_PWD    "\"DDK123456\""


uint8_t flag_netbuff = BUFF_EMPTY;		//Net Buff FULL Flag
int8_t net_buff[BUFF_SPACE];			//10MB Net Buff


char dllink[500] = { 0 };				//Store Song Download Url
char songpoint[50] = { 0 };				//Store Song Name Download Form Net ,Unnecessart Now

ESP8266_DEF __ESP8266_A;				//Define ESP8266 Control Struct
ESP8266_DEF_PTR ESP8266_A = &__ESP8266_A;

DEV_UART_PTR uart_obj;

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


/**
**	 Init ESP8266 and Malloc 10MB net buff and Connect to WIfi
**
**/

void net_init()
{
	EMBARC_PRINTF("============================ Init ============================\n");
	
	
	memset( net_buff, 0, sizeof(int8_t) * BUFF_SPACE );



    esp8266_init(ESP8266_A, 3125000);
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
    
    uart_obj = uart_get_dev(ESP8266_UART_ID);
    // _Rtos_Delay(100);
	
}

/***********
**获取Song ID 或者 下载地址**
**option：0 SONGID ;1 DOWNLOAD LINK
***/

int socket_request(uint8_t option)
{
	char http_cmd[500] = {0};
	uint32_t idlen_int;
	char idlen_char[3] = "";

	DEV_BUFFER Rxintbuf;

	DEV_BUFFER_INIT(&Rxintbuf, net_buff, sizeof(int8_t) * BUFF_SPACE);

    EMBARC_PRINTF("============================ connect socket ============================\n\r");
	esp8266_tcp_connect(ESP8266_A,"180.76.141.217", 80);

    memset(http_cmd, 0, sizeof(char) * 500);
	memset(net_buff, 0, sizeof(int8_t) * BUFF_SPACE);

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
    _Rtos_Delay(100);
	esp8266_passthr_write( ESP8266_A, http_cmd,strlen(http_cmd) );
	_Rtos_Delay(100);

	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, (void*)(&Rxintbuf));
	xTaskResumeAll();

	EMBARC_PRINTF("======================== Get all Data Driectly===================\r\n");
	_Rtos_Delay(1000);			//Wait Data to Arrive

    EMBARC_PRINTF("%s\r\n",(net_buff));

	
	/*********Receive Complete , Reset Flag and Disable Passthrough***************/
	esp8266_passthr_end(ESP8266_A);
	_Rtos_Delay(100);
	esp8266_transmission_mode(ESP8266_A,ESP8266_NORMALSEND);
	_Rtos_Delay(100);

	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, NULL);
	_Rtos_Delay(100);
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




/***
**Download MP3 use Download link
**
**/


void download_mp3()
{
	uint32_t http_sum = 0;
	char http_cmd[500] = {0};
	uint8_t timeout_cnt = 0;
	uint32_t bypass_cnt = 0;	

	uint32_t net_time = 0;
	uint32_t net_time_pre = 0;
	
	DEV_BUFFER Rxintbuf;



	DEV_BUFFER_INIT(&Rxintbuf, net_buff, sizeof(int8_t) * BUFF_SPACE);
	

	EMBARC_PRINTF("============================ connect socket ============================\n\r");
	esp8266_tcp_connect(ESP8266_A,"211.91.125.36", 80);

    memset(http_cmd, 0, sizeof(char) * 500);
	memset(net_buff, 0, sizeof(int8_t) * BUFF_SPACE);

    strcat (http_cmd,"GET ");
    strcat (http_cmd,dllink);
    strcat (http_cmd," HTTP/1.1\r\nHost: zhangmenshiting.qianqian.com\r\nConnection: keep-alive\r\n\r\n");

    EMBARC_PRINTF("\r\n%s\r\n",http_cmd);


	/*****Enable passthrough to Deal with +IPD flag********/
	vTaskSuspendAll();
    esp8266_passthr_start(ESP8266_A);
    _Rtos_Delay(100);
	esp8266_passthr_write( ESP8266_A, http_cmd,strlen(http_cmd) );	
	_Rtos_Delay(100);
	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, (void*)(&Rxintbuf));

	xTaskResumeAll();

	net_time_pre = xTaskGetTickCount ();
	while(1)
	{
		_Rtos_Delay(1000);

		net_time = xTaskGetTickCount ();

		bypass_cnt = uart_obj -> uart_info.rx_buf.ofs;//read the buff offset has received
		
    	if ( http_sum != bypass_cnt  )
    	{
			gui_info.network_speed = ( bypass_cnt - http_sum ) * 1000 / 1024 / ( net_time - net_time_pre );
    		xEventGroupSetBits( GUI_Ev, BIT_0 );

    		EMBARC_PRINTF("received : %d KB\r",bypass_cnt / 1024 );
			EMBARC_PRINTF("received : %d KB/s\r", gui_info.network_speed);
			http_sum = bypass_cnt;
			timeout_cnt = 0;

			if ( http_sum > 14*1024*1024 )//protect
			{
				EMBARC_PRINTF("received More than 14MB! Break！\r\n");
				break;
			}
    	}
    	else
    	{
    		gui_info.network_speed = -1;
    		xEventGroupSetBits( GUI_Ev, BIT_0 );
    		
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
	
	uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, NULL);
	_Rtos_Delay(100);

	if ( http_sum >1024 )
	{
		filelist_add(FILE_LIST,songpoint,http_sum,IN_BUFF);
		flag_netbuff = BUFF_FULL;
	}
	else
	{ 
		EMBARC_PRINTF("Receive Fail!\r\n");
	}
	EMBARC_PRINTF("Socket Close.\r\n");

	/**********Connect will Close Automatic*********************/
	esp8266_CIPCLOSE(ESP8266_A);
	
}




