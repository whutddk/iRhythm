#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"





void net_task()
{
	


	EMBARC_PRINTF("NET_TASK START\r\n");
    // //ESP8266 Init
   

    //Creat Server
    // EMBARC_PRINTF("============================ Connect Server ============================\n");

    // esp8266_tcp_server_open(esp8266, 80);

    net_init();

    socket_request(0);


    EMBARC_PRINTF("============================ while ============================\n");

    while (1)
    {
    	;
        // memset(scan_result, 0, sizeof(scan_result));
        // at_read(esp8266->p_at ,scan_result ,1000);
        // vTaskDelay( 1 );
        // // board_delay_ms(200, 1);
        // //EMBARC_PRINTF("Alive\n");

        // if(strstr(scan_result, http_get) != NULL)
        // {
        //     EMBARC_PRINTF("============================ send ============================\n");

        //     EMBARC_PRINTF("\nThe message is:\n%s\n", scan_result);

        //     conn_buf = strstr(scan_result, http_IDP) + 5;
        //     *(conn_buf+1) = 0;

        //     EMBARC_PRINTF("Send Start\n");
        //     vTaskDelay( 1 );
        //     // board_delay_ms(10, 1);

        //     esp8266_connect_write(esp8266, http_html_header, conn_buf, (sizeof(http_html_header)-1));
        //     vTaskDelay( 1 );
        //     // board_delay_ms(100, 1);

        //     esp8266_connect_write(esp8266, http_html_body_1, conn_buf, (sizeof(http_html_body_1)-1));
        //     vTaskDelay( 1 );
        //     // board_delay_ms(300, 1);

        //     esp8266_connect_write(esp8266, http_html_body_2, conn_buf, (sizeof(http_html_body_2)-1));
        //     vTaskDelay( 1 );
        //     // board_delay_ms(300, 1);

        //     esp8266_CIPCLOSE(esp8266, conn_buf);

        //     EMBARC_PRINTF("Send Finish\n");
        // }
    }

    return E_OK;
}


