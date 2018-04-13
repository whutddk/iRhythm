#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

/*****A[31:28]****PMOD6[10:7]*************/
#define BOARD_SIGN_MASK 0xf0000000

static DEV_GPIO *io_signal;

void iosignal_init()
{
	io_signal = gpio_get_dev(DEV_GPIO_PORT_A);

	if (io_signal->gpio_open(0xf0000000) == E_OPNED) {
		io_signal->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)(BOARD_SIGN_MASK));
		io_signal->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(BOARD_SIGN_MASK));
	}

	iosignal_ctrl(0,0);
	iosignal_ctrl(0,1);
	iosignal_ctrl(0,2);
	iosignal_ctrl(0,3);
error_exit:
	return;
}


void iosignal_ctrl(uint8_t val,uint8_t num)
{
	/** write 1 to light on led bit, else light off led */

	uint32_t temp = 0;

	temp = val?0x10000000:0;

	switch(num)
	{
		case 0:
		io_signal->gpio_write(temp<<0, 0x10000000);
		break;
		case 1:
		io_signal->gpio_write(temp<<1, 0x20000000);
		break;
		case 2:
		io_signal->gpio_write(temp<<2, 0x40000000);
		break;
		case 3:
		io_signal->gpio_write(temp<<3, 0x80000000);
		break;
		default:
		break;
	}

	// led_val = (~led_val) & mask;
	// led_val = led_val << EMSK_LED_OFFSET;
	// mask = (mask << EMSK_LED_OFFSET) & BOARD_SIGN_MASK;

	return;
}



