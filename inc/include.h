#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "esp8266.h"


#include <string.h>
#include <stdio.h>
#include <malloc.h>
/*************define in net.c****************/
extern uint8_t flag_netpoll ;
extern char *net_buff;
extern ESP8266_DEF_PTR ESP8266_A;
extern void net_init();
extern int socket_request(unsigned char option);


#endif
