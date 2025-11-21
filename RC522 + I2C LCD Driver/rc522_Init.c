#include"stm32f407xx.h"
#include "stdio.h"
#include"RC522_SPI_Init.c"

void rc522_init(void)
{
	/*
	 * STM32 ->RFID
	 * SPI1  -> SPI
	 * PA8  ->RST
	 * PB0  ->CS alternate function
	 * */
	
  RC522_SPI_Init();
	
  GPIOA->MODER|=GPIO_MODER_MODE8_0;
  GPIOA->MODER&=~GPIO_MODER_MODE8_1;

  RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;

  GPIOB->MODER|=GPIO_MODER_MODE0_0;
  GPIOB->MODER&=~GPIO_MODER_MODE0_1;
  GPIOA->BSRR=GPIO_BSRR_BR8;
  for(volatile int i=0;i<100000;i++);
  GPIOA->BSRR=GPIO_BSRR_BS8;
  for(volatile int i=0;i<100000;i++);
  rc522_reset();
	//Met CS bas
  rc522_regWrite8(MFRC522_REG_T_MODE, 0x80);
  rc522_regWrite8(MFRC522_REG_T_PRESCALER, 0xA9);
  rc522_regWrite8(MFRC522_REG_T_RELOAD_L, 0xE8);
  rc522_regWrite8(MFRC522_REG_T_RELOAD_H, 0x03);


  rc522_regWrite8(MFRC522_REG_TX_AUTO, 0x40);
  rc522_regWrite8(MFRC522_REG_MODE, 0x3D);

  rc522_antennaON();   //Open the antenna
}