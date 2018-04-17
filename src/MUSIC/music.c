#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"

volatile uint8_t flag_dma_finish = 0;
DEV_SPI_PTR spi;

void spi_dma_test()
{
	uint8_t spi_buff[2] ={0xaa,0xab}; 


	

	

	

	EMBARC_PRINTF("Start TO dma_pull\r\n");
	EMBARC_PRINTF("spi freq:%d\r\n",spi->spi_info.freq);

	while(1)
	{
		// spi->spi_write(spi_buff,2);

	}
	
}