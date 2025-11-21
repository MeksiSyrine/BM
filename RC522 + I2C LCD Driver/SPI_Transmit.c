#include"stm32f407xx.h"
#include "stdio.h"
int8_t SPI_Transmit(uint8_t *data, uint32_t size)
{


	uint32_t i		=0;
	uint8_t  temp	=0;
	uint32_t start=millis();
	temp =SPI1->DR;
	temp=SPI1->SR;
	while(i<size)
		{
		while(!((SPI1->SR)&SPI_SR_TXE)){if(millis()-start>1000){
			printf("TXE timed out\r\n");
			return -1;}} // wait to transmision buffer to be emplty
		SPI1->DR= data[i];
		while(!(SPI1->SR&SPI_SR_BSY)){if(millis()-start>1000){printf("BSY timed out\r\n");return -1;}}
		i++;
		}
while(!((SPI1->SR)&SPI_SR_TXE)){if(millis()-start>1000){printf("TXE2 time dout\r\n");return -1;}}
while((SPI1->SR)&SPI_SR_BSY){if(millis()-start>1000){printf("BSY2 timed out\r\n"); return -1;}}
temp =SPI1->DR;
temp=SPI1->SR;
return 0;
}