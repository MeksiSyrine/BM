#include "mcp_init.h"
#include "spi.h"
#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>

/*int main()
{
    SPI1_GPIO_Init();
    SPI1_Init();

    MCP2515_Init();

    CAN_Frame tx;
    tx.id = 0x123;
    tx.dlc = 3;
    tx.data[0] = 0x11;
    tx.data[1] = 0x22;
    tx.data[2] = 0x33;

    MCP2515_SendMessage(&tx);

    CAN_Frame rx;

    while (1)
    {
        if (MCP2515_ReceiveMessage(&rx) == 0)
        {
            // message received!
        }
    }
}*/

int main(void)
{
    SPI1_GPIO_Init();
    SPI1_Init();

    // Set loopback mode
    if (MCP2515_SetLoopbackMode() != 0)
    {
        // error
        while(1);
    }

  

		   CAN_Frame tx = {
        .id = 0x123,
        .dlc = 8,
        .data = {10,20,30,40,50,60,70,80}
    };

    CAN_Frame rx;
while (1)
{
   MCP2515_SendMessage(&tx);
    printf("Message sent!\n");

 if (MCP2515_ReceiveMessage(&rx) == 0)
        {
            printf("RX ID=%03X DLC=%d DATA=", rx.id, rx.dlc);
            for (int i = 0; i < rx.dlc; i++)
                printf("%d ", rx.data[i]);
            printf("\n");
        }

    for (volatile int i = 0; i < 500000; i++); // small delay
}
}