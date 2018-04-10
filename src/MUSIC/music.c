#include "embARC.h"
#include "embARC_debug.h"

#include "include.h"




void spi_poll()
{
	uint8_t spi_buff[2] ={0xaa,0xab}; 
	DEV_SPI_PTR spi;

	spi =  spi_get_dev(DW_SPI_0_ID);

	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)EMSK_SPI_LINE_0));
	EMBARC_PRINTF("Start TO Pull\r\n");
	while(1)
	{
		spi->spi_write(spi_buff,2);
	}
	
}