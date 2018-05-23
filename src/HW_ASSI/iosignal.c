#include "embARC.h"
#include "embARC_debug.h"


#include "../../board/emsk/drivers/mux/mux_hal.h"
#include "../../board/emsk/drivers/mux/mux.h"


#include "include.h"

/*****A[31:28]****PMOD6[10:7]*************/
#define BOARD_SIGNOUT_MASK 0x30000100
#define BOARD_SIGNIN_MASK 0xC0000007

static DEV_GPIO *io_signal;

/**
 * \brief       IO interrupt that trigger music event to switch back music task
 *              
 */
void empty_isr()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xEventGroupSetBitsFromISR(
		evt1_cb,	// The event group being updated.
		BIT_1,   // The bits being set.
		&xHigherPriorityTaskWoken );
	//EMBARC_PRINTF("GPIO INTERRUPT!\r\n");
}


/**
 * \brief       IO port initalization 
 *              
 */
void iosignal_init()
{
	DEV_GPIO_INT_CFG gpio_int_cfg;
	DEV_GPIO_BIT_ISR gpio_bit_isr;


	uint32_t val = get_pmod_mux();
	val &= ~(PM1_LR_SPI_S);			//change LOW ROW of PM1 to GPIOA
	set_pmod_mux(val);

	gpio_int_cfg.int_bit_mask = BOARD_SIGNIN_MASK;
	gpio_int_cfg.int_bit_type = BOARD_SIGNIN_MASK;
	gpio_int_cfg.int_bit_polarity = GPIO_INT_RISING_EDGE_ALL;
	gpio_int_cfg.int_bit_debounce = 0;

	gpio_bit_isr.int_bit_ofs = 30;
	gpio_bit_isr.int_bit_handler = empty_isr;

	io_signal = gpio_get_dev(DEV_GPIO_PORT_A);

	if (io_signal->gpio_open(0xf0000000) == E_OPNED) {
		io_signal->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)(BOARD_SIGNOUT_MASK));
		io_signal->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(BOARD_SIGNIN_MASK));
		io_signal->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(BOARD_SIGNOUT_MASK));

		/*************Interrupt enable********************/
		io_signal->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(BOARD_SIGNIN_MASK));

		gpio_bit_isr.int_bit_ofs = 30;
		gpio_bit_isr.int_bit_handler = empty_isr;
		io_signal->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&gpio_bit_isr));


		gpio_bit_isr.int_bit_ofs = 0;
		gpio_bit_isr.int_bit_handler = key1_isr;
		io_signal->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&gpio_bit_isr));

		gpio_bit_isr.int_bit_ofs = 1;
		gpio_bit_isr.int_bit_handler = key2_isr;
		io_signal->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&gpio_bit_isr));

		gpio_bit_isr.int_bit_ofs = 2;
		gpio_bit_isr.int_bit_handler = key3_isr;
		io_signal->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&gpio_bit_isr));

		io_signal->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&gpio_int_cfg));
		io_signal->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)(BOARD_SIGNIN_MASK));
	}

	iosignal_ctrl(0, 0);
	iosignal_ctrl(0, 1);

	io_signal->gpio_write(0x00000100, 0x00000100);
	// iosignal_ctrl(0,2);
	// iosignal_ctrl(0,3);
error_exit:
	return;
}

/**
 * \brief       IO output control
 * 
 * \param[in]   val                    1:IO LOGIC HIGH 0:IO LOGIC LOW
 *
 * \param[in]   num                    0 or 1 for two special IO for debug popuse
 *
 */
void iosignal_ctrl(uint8_t val, uint8_t num)
{
	uint32_t temp = 0;

	temp = val ? 0x10000000 : 0;

	switch (num) {
		case 0:
			io_signal->gpio_write(temp << 0, 0x10000000);
			break;

		case 1:
			io_signal->gpio_write(temp << 1, 0x20000000);
			break;

		default:
			break;
	}

	return;
}

/**
 * \brief       Read IO logical lever 
 *
 * \param[in]   num                    0 or 1 for two special IO
 *
 * \retval      0                      Logical LOW
 *
 * \retval      1                      Logical HIGH
 *
 */
uint8_t iosignal_read(uint8_t num)
{
	uint32_t value = 0, mask;

	if ( num == 0 ) {
		mask = 0x40000000;
		io_signal->gpio_read(&value, mask);
		value = (~value) & mask;
	} else {
		mask = 0x80000000;
		io_signal->gpio_read(&value, mask);
		value = (~value) & mask;
	}

	return (value ? 0 : 1);
}

/**
 * \brief       PULL logical LOW for a period for ESP8266 reset
 *
 */
void net_rst()
{
	uint32_t cur_time;
	/***************RST = 0;*****************/
	io_signal->gpio_write(0x00000000, 0x00000100);

	// _Rtos_Delay(100);
	cur_time = OSP_GET_CUR_MS();

	while ((OSP_GET_CUR_MS() - cur_time) < 1000);

	/***************RST = 1;*****************/
	io_signal->gpio_write(0x00000100, 0x00000100);
}

