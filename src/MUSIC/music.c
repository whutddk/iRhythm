#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

uint8_t buff_A[4608] ={0xaa,0xab,0xaa,0xab};
uint8_t flag_dma_finish = 0;

void spi_dma_test()
{
	uint8_t spi_buff[2] ={0xaa,0xab}; 

	DEV_SPI_PTR spi;

	 spi_dma_prepare();

	spi =  spi_get_dev(DW_SPI_0_ID);

	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));

	EMBARC_PRINTF("Start TO dma_pull\r\n");
	EMBARC_PRINTF("spi freq:%d\r\n",spi->spi_info.freq);

	while(1)
	{
		// spi->spi_write(spi_buff,2);
		spi_writeraw(buff_A);
		while(flag_dma_finish == 0);
		flag_dma_finish = 0;
	}
	
}