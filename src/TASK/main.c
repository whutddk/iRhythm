/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */



/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_BLINKY
 * \brief	main source file for blinky example
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_BLINKY
 * @{
 */
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"
#include "include.h"
#include "inc_task.h"

static TaskHandle_t MUSIC_task_handle = NULL;
static TaskHandle_t GUI_task_handle = NULL;
static TaskHandle_t NET_task_handle = NULL;
static TaskHandle_t IDLE_task_handle = NULL;

// Events
EventGroupHandle_t evt1_cb;

void idle_task()
{
	while(1);
}


/**
 * \brief	Test hardware board without any peripheral
 */


int32_t error_num = 0;

int main(void)
{
	// board_init();
	io_mux_init();
	emsk_gpio_init();

	EMBARC_PRINTF("START to TEST FREERTOS\r\n");
	EMBARC_PRINTF("Benchmark CPU Frequency: %d Hz\r\n", BOARD_CPU_CLOCK);

	vTaskSuspendAll();

/**********MP3 Decord Assist IO Init***************************/
	iosignal_init();

/********IO reset ESP8266************************/
	net_rst();
/*********init Songid List*****/
    filelist_init();
/*******Init Esp8266 and Connect to Wifi***************/
    net_init();
    spi_dma_prepare();
/********************** Create Tasks**************************/



	// if (xTaskCreate(music_task, "music_task", 512, (void *)NULL, configMAX_PRIORITIES-1, &MUSIC_task_handle)
	//     != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
	// 	EMBARC_PRINTF("create music_task error\r\n");
	// 	return -1;
	// }	
	if (xTaskCreate(net_task, "net_task", 512, (void *)NULL, configMAX_PRIORITIES-2, &NET_task_handle)
	    != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create NET_task error\r\n");
		return -1;
	}    
	// if (xTaskCreate(gui_task, "gui_task", 256, (void *)NULL, configMAX_PRIORITIES-3, &GUI_task_handle)
	//     != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
	// 	EMBARC_PRINTF("create GUI_task error\r\n");
	// 	return -1;
	// }
	if (xTaskCreate(idle_task, "idle_task", 128, (void *)NULL, configMAX_PRIORITIES-4, &IDLE_task_handle)
	    != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create IDLE_task error\r\n");
		return -1;
	}


	//other task


	//other task end//
	
	// Create Events

	evt1_cb = xEventGroupCreate();


	xTaskResumeAll();
	while(1);
	return E_SYS;
}





/** @} */