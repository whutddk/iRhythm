#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

/*****A[31:28]****PMOD6[10:7]*************/
#define BOARD_SIGNOUT_MASK 0x30000100
#define BOARD_SIGNIN_MASK 0xC0000007

static DEV_GPIO *io_signal;

void empty_isr()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	

	/*******No a Suggestion Used Here**************************/
	// xEventGroupSetBits( evt1_cb, BIT_1 );

	xEventGroupSetBitsFromISR(
		evt1_cb,	// The event group being updated.
		BIT_1,   // The bits being set.
		&xHigherPriorityTaskWoken );
	//EMBARC_PRINTF("GPIO INTERRUPT!\r\n");
}

void key1_isr()//确定键
{
	//SWITCH 254 SONG most 
	uint8_t i;
	struct filelist *file_pointer = Playlist_HEAD;

	gui_info.delay_cnt = xTaskGetTickCount ();

	if ( gui_info.screen_point == 0 )
	{
		gui_info.screen_point ++;
		gui_info.next_song = file_pointer -> data;
	}
	else
	{
		gui_info.flag_next = 1;

		//how to stay in the same song?
		for ( i = gui_info.screen_point - 1; i > 0; i -- )
		{
				/***********If it is the last Song in Play List,Play it again and again and Never Delete*******************/
			if ( Playlist_HEAD -> next != NULL )
			{
				if ( Playlist_HEAD -> location != IN_BUFF )
				{
					;
				}
				else
				{
					flag_netbuff = BUFF_EMPTY;
				}


				filelist_delete(FILE_LIST);				//Once Play a Song, delete it from Playlist
			}
			else
			{
				EMBARC_PRINTF("\r\nNo Song Left!!!\r\n");
			}
		}
		gui_info.screen_point = 0;
		gui_info.network_speed = -1;
		gui_info.decord_speed = -1;
		gui_info.main_cycle = -1;
	}
	//EMBARC_PRINTF("key1_isr!\r\n");


	//Now Reflash the Srceen
	xEventGroupSetBits( GUI_Ev, BIT_0 );
}

void key2_isr()//右键
{
	uint8_t i;
	struct filelist *file_pointer = Playlist_HEAD;

	gui_info.delay_cnt = xTaskGetTickCount ();

	for ( i = gui_info.screen_point ; i > 0 ;i -- )
	{
		file_pointer = file_pointer -> next;
		if ( file_pointer != NULL )
		{
			
		}
		else
		{
			return;	//超出了没必要刷新画面，刷新了时间即可，如果是0界面，不会出现这种情况
		}
	}

	//EMBARC_PRINTF("key2_isr!\r\n");

	gui_info.next_song = file_pointer -> data;
	gui_info.screen_point ++;

	//Now Reflash the Srceen
	xEventGroupSetBits( GUI_Ev, BIT_0 );
}

void key3_isr()
{
	//EMBARC_PRINTF("key3_isr!\r\n");
}

void iosignal_init()
{
	DEV_GPIO_INT_CFG gpio_int_cfg;
	DEV_GPIO_BIT_ISR gpio_bit_isr;

	gpio_int_cfg.int_bit_mask = BOARD_SIGNIN_MASK;
	gpio_int_cfg.int_bit_type = BOARD_SIGNIN_MASK;
	gpio_int_cfg.int_bit_polarity = GPIO_INT_RISING_EDGE_ALL;
	gpio_int_cfg.int_bit_debounce = 0;

	gpio_bit_isr.int_bit_ofs = 30;
	gpio_bit_isr.int_bit_handler = empty_isr;

	io_signal = gpio_get_dev(DEV_GPIO_PORT_A);

	if (io_signal->gpio_open(0xf0000000) == E_OPNED) 
	{
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

	iosignal_ctrl(0,0);
	iosignal_ctrl(0,1);

	io_signal->gpio_write(0x00000100, 0x00000100);
	// iosignal_ctrl(0,2);
	// iosignal_ctrl(0,3);
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
		// case 2:
		// io_signal->gpio_write(temp<<2, 0x40000000);
		// break;
		// case 3:
		// io_signal->gpio_write(temp<<3, 0x80000000);
		// break;
		default:
		break;
	}

	// led_val = (~led_val) & mask;
	// led_val = led_val << EMSK_LED_OFFSET;
	// mask = (mask << EMSK_LED_OFFSET) & BOARD_SIGN_MASK;

	return;
}

uint8_t iosignal_read(uint8_t num)
{
	uint32_t value = 0,mask;

	if ( num == 0 )
	{
		mask = 0x40000000;
		io_signal->gpio_read(&value, mask);
		value = (~value) & mask;
	}
	else
	{	
		mask =0x80000000;
		io_signal->gpio_read(&value, mask);
		value = (~value) & mask;
	}

	return (value?0:1);
}

void net_rst()
{
	uint32_t cur_time;
	/***************RST = 0;*****************/
	io_signal->gpio_write(0x00000000, 0x00000100);

	// _Rtos_Delay(100);
	cur_time = OSP_GET_CUR_MS();
	while((OSP_GET_CUR_MS()-cur_time) < 1000);

	/***************RST = 1;*****************/
	io_signal->gpio_write(0x00000100, 0x00000100);


} 

