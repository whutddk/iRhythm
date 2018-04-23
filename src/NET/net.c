//net control
//DDK
//20180227

#include "embARC.h"
#include "embARC_debug.h"

#include "esp8266.h"

#define WIFI_SSID   "\"Andrew_Sun\""
#define WIFI_PWD    "\"smartcar\""


static char http_get[] = "GET /";
static char http_IDP[] = "+IPD,";
static char http_html_header[] = "HTTP/1.x 200 OK\r\nContent-type: text/html\r\n\r\n";
static char http_html_body_1[] = "<html><head><title>ESP8266_AT_HttpServer</title></head><body><h1>Welcome to this Website</h1>";
static char http_html_body_2[] = "<p>This Website is used to test the AT command about HttpServer of ESP8266.</p></body></html>";


ESP8266_DEF __ESP8266_A;
ESP8266_DEF_PTR ESP8266_A = &__ESP8266_A;

void net_init()
{
	char *conn_buf;
    char scan_result[1024];

	EMBARC_PRINTF("============================ Init ============================\n");
	
    esp8266_init(ESP8266_A, UART_BAUDRATE_115200);
    at_test(ESP8266_A->p_at);
    vTaskDelay( 1 );

    // //Set Mode
    EMBARC_PRINTF("============================ Set Mode ============================\n");
    esp8266_wifi_mode_get(ESP8266_A, false);
    vTaskDelay( 1 );
    esp8266_wifi_mode_set(ESP8266_A, 3, false);
    vTaskDelay( 1 );

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
        vTaskDelay( 1 );
    }
    EMBARC_PRINTF("WIFI %s connect succeed\n", WIFI_SSID);

    //Show IP
    EMBARC_PRINTF("============================ Show IP ============================\n");
    esp8266_address_get(ESP8266_A);
    EMBARC_PRINTF("============================ try to return  ============================\n");
    vTaskDelay( 1 );
	
}

// void net_deinit()
// {
// 	net.disconnect();
// 	uartpc.printf("net disconnect\n\r");
// }

//获取下载地址

// #define REC_BUFF_SIZE (4096)
// int socket_request(unsigned char option)
// {
// 	TCPSocket socket;
// 	int rcount;

// 	socket.open(&net);
//     socket.connect("180.76.141.217", 80);
//     char *http_cmd = (char *)malloc(sizeof(char) * 500);
//     memset(http_cmd, 0, sizeof(char) * 500);
//     switch (option)
//     {
//     	case 0:
// 			strcat (http_cmd,"GET http://fm.baidu.com/dev/api/?tn=playlist&id=public_tuijian_rege&hashcode=&_=1519727783752 HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\n\r\n");
// 			break;
//     	case 1:
// 			strcat (http_cmd, "POST http://fm.baidu.com/data/music/songlink HTTP/1.1\r\nHost: fm.baidu.com\r\nConnection: keep-alive\r\nContent-Length: ");
			
// 			char id_char[10] = {0};

// 			int idlen_int = strlen(Songid_HEAD->data);
// 			idlen_int += 8;
// 			char idlen_char[3] = "";

// 			itoa(idlen_int,idlen_char,10);
// 			strcat(http_cmd,idlen_char);
// 			strcat(http_cmd,"\r\n\r\nsongIds=");

// 			//itoa(songidnum,Songid_HEAD->data,10);
// 			strcat (http_cmd,Songid_HEAD->data);
// 			list_delete(0);

// 			uartpc.printf("\r\n%s\r\n",http_cmd);

// 			break;
//     }
    
//     int scount = socket.send(http_cmd, strlen(http_cmd));
//     free(http_cmd);

// 	char *rec_buf = (char *)malloc(sizeof(char) * 8);
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

// 	//skip until {
// 	do 
// 	{

// 		rcount = socket.recv(rec_buf, 1);
// 	}
// 	while(*rec_buf != '{' && rcount != 0);

// 	strcat(response,"{\0");

// 	while(1)
// 	{
// 		uint16_t rec_sum = 0;
//     	memset(rec_buf, 0, sizeof(char) * 8);
//     	rcount = socket.recv(rec_buf, 1);
//     	rec_sum += rcount;
//     	if ( rcount <= 0 )
// 		{			
// 			uartpc.printf("recv end\n\r");
// 			break;
//     	}
//     	if ( rec_sum >= REC_BUFF_SIZE )
// 		{			
// 			uartpc.printf("too big,break!\n\r");
// 			break;
//     	}
//     	rec_buf[rcount] ='\0';  	
//     	strcat(response,rec_buf);  	
// 	}

// 	uartpc.printf("%s",response);

// 	switch(option)
// 	{
// 		case 0 :
// 			if (-1 == get_songid(response))
// 			{
// 				return -1;
// 			}
// 			break;
// 		case 1:
// 			if (-1 == get_songinfo(response))
// 			{
// 				return -1;
// 			}
// 			break;
// 	}


// 	free(rec_buf);
// 	free(response);
// 	uartpc.printf("recv done.\r\n");
// 	socket.close();

// 	return 0;
// }

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




