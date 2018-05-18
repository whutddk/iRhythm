#ifndef _INC_TASK_H_
#define _INC_TASK_H_

inline void _Rtos_Delay(uint32_t ticks)
{
	TickType_t xLastWakeTime;
	TickType_t xFrequency;

	xLastWakeTime = xTaskGetTickCount ();
    xFrequency = ticks;
	vTaskDelayUntil( &xLastWakeTime, xFrequency );
}

// Events
extern EventGroupHandle_t evt1_cb;
extern EventGroupHandle_t GUI_Ev;


extern void music_task();
extern void net_task();
extern void gui_task();


#endif

