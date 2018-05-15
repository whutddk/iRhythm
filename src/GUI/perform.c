#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

uint32_t cost_cyc;
static volatile uint32_t start = 0;
static uint32_t perf_id = 0xFF;

/** performance timer initialization */
void perf_init(uint32_t id)
{
	if (timer_start(id, TIMER_CTRL_NH, 0xFFFFFFFF) < 0) {
		EMBARC_PRINTF("perf timer init failed\r\n");
		while(1);
	}
	perf_id = id;
}

/** performance timer start */
void perf_start(void)
{
	if (timer_current(perf_id, (void *)(&start)) < 0) {
		start = 0;
	}
}

/** performance timer end, and return the time passed */
uint32_t perf_end(void)
{
	uint32_t end = 0;

	if (timer_current(perf_id, (void *)(&end)) < 0) {
		return 0;
	}

	if (start < end) {
		return (end - start);
	} else {
		return (0xFFFFFFFF - start + end);
	}
}