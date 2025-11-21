#include"stm32f407xx.h"
#include "millis.h"

int8_t SPI_Receive(uint8_t *data, uint32_t size)
{
while(size)
		{
	uint32_t start=millis();
		SPI1->DR=0;
		while(!(SPI1->SR&SPI_SR_RXNE)){if(millis()-start>200){return -1;}}
		*data++=(SPI1->DR);
			size--;
		}
return 0;
}