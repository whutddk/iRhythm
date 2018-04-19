#include "embARC.h"
#include "embARC_debug.h"

#include "esp8266.h"

#include <stdio.h>
#include <string.h>

#define WIFI_SSID   "\"Andrew_Sun\""
#define WIFI_PWD    "\"smartcar\""

static char http_get[] = "GET /";
static char http_IDP[] = "+IPD,";
static char http_html_header[] = "HTTP/1.x 200 OK\r\nContent-type: text/html\r\n\r\n";
static char http_html_body_1[] = "<html><head><title>ESP8266_AT_HttpServer</title></head><body><h1>Welcome to this Website</h1>";
static char http_html_body_2[] = "<p>This Website is used to test the AT command about HttpServer of ESP8266.</p></body></html>";



void net_task()
{
	char *conn_buf;
    char scan_result[1024];


	EMBARC_PRINTF("NET_TASK START\r\n");
    // //ESP8266 Init
    EMBARC_PRINTF("============================ Init ============================\n");
	
	//vTaskDelay( 1000 );

    

    ESP8266_DEFINE(esp8266);
    esp8266_init(esp8266, UART_BAUDRATE_115200);
    at_test(esp8266->p_at);
    vTaskDelay( 1 );
    //board_delay_ms(100, 1);

    // //Set Mode
    EMBARC_PRINTF("============================ Set Mode ============================\n");

    esp8266_wifi_mode_get(esp8266, false);
    vTaskDelay( 1 );
    // board_delay_ms(100, 1);
    esp8266_wifi_mode_set(esp8266, 3, false);
    vTaskDelay( 1 );
    // board_delay_ms(100, 1);

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

    EMBARC_PRINTF("WIFI %s found! Try to connect\n", WIFI_SSID);

    while(esp8266_wifi_connect(esp8266, WIFI_SSID, WIFI_PWD, false)!=AT_OK)
    {
        EMBARC_PRINTF("WIFI %s connect failed\n", WIFI_SSID);
        vTaskDelay( 1 );
        // board_delay_ms(100, 1);
    }

    EMBARC_PRINTF("WIFI %s connect succeed\n", WIFI_SSID);

    //Creat Server
    EMBARC_PRINTF("============================ Connect Server ============================\n");

    esp8266_tcp_server_open(esp8266, 80);

    //Show IP
    EMBARC_PRINTF("============================ Show IP ============================\n");

    esp8266_address_get(esp8266);
    vTaskDelay( 2 );
    // board_delay_ms(1000, 1);

    EMBARC_PRINTF("============================ while ============================\n");

    while (1)
    {
        memset(scan_result, 0, sizeof(scan_result));
        at_read(esp8266->p_at ,scan_result ,1000);
        vTaskDelay( 1 );
        // board_delay_ms(200, 1);
        //EMBARC_PRINTF("Alive\n");

        if(strstr(scan_result, http_get) != NULL)
        {
            EMBARC_PRINTF("============================ send ============================\n");

            EMBARC_PRINTF("\nThe message is:\n%s\n", scan_result);

            conn_buf = strstr(scan_result, http_IDP) + 5;
            *(conn_buf+1) = 0;

            EMBARC_PRINTF("Send Start\n");
            vTaskDelay( 1 );
            // board_delay_ms(10, 1);

            esp8266_connect_write(esp8266, http_html_header, conn_buf, (sizeof(http_html_header)-1));
            vTaskDelay( 1 );
            // board_delay_ms(100, 1);

            esp8266_connect_write(esp8266, http_html_body_1, conn_buf, (sizeof(http_html_body_1)-1));
            vTaskDelay( 1 );
            // board_delay_ms(300, 1);

            esp8266_connect_write(esp8266, http_html_body_2, conn_buf, (sizeof(http_html_body_2)-1));
            vTaskDelay( 1 );
            // board_delay_ms(300, 1);

            esp8266_CIPCLOSE(esp8266, conn_buf);

            EMBARC_PRINTF("Send Finish\n");
        }
    }

    return E_OK;
}


