/******************************************************************************
 *
 *** For RFID-RC522 module:
 * CS        - PB0
 * SCK       - PA5
 * MOSI      - PA7
 * MISO      - PA6
 * RST       - PA8
 *** For LCD:
 * SLK       - PB8
 * SDA       - PB9
 *
 * @File     main.c
 * @Author   Syrrine MEKSI
 *
 *****************************************************************************/



#include"stm32f407xx.h"
#include "lcd.h"
#include "delay.h"
#include "stdio.h"
#include "rc522.h"


uint8_t rfid_id[4];
int main(void)
		{
		RC522_SPI_Init();
		lcd_init();
		setCursor(0,0);
		lcd_send_string("Hello!!");
		delay(2000);
		//lcd_clear();
		while(1)
				{
					lcd_clear();
						setCursor(0,0);
						lcd_send_string("Nobody is here");
					
				if(rc522_checkCard(rfid_id))
							{

							lcd_clear();
							char data[20];
							setCursor(0,0);
								lcd_send_string("Detected! code:");
							setCursor(0,1);
							sprintf(data,"0x%x 0x%x 0x%x 0x%x",rfid_id[0],rfid_id[1],rfid_id[2],rfid_id[3]);
							lcd_send_string(data);
							delay(1000);
							}
				delay(100);
							
				}

		}

