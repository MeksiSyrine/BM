
#include "stm32f4xx.h"

int main(void);

#define BUF_SIZE 8

 uint32_t src_addr[8] = {0, 1, 2, 3, 4, 5, 6, 7};
 uint32_t dst_addr[8] = {};

void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & (1 << 5)) {
        // clear stream 0 transfer complete interrupt
       DMA2->LIFCR |= (1 << 5);
			DMA2_Stream0->CR &= ~(1 << 0);
    }
}

/*************************************************
* main code starts from here
*************************************************/
int main(void)
{
    // wait a bit
    for(volatile int i=10000000; i>0; i--);

    // Fill src_addr with numbers
    // Zero out dst_addr
    for (int i=0; i<BUF_SIZE; i++){
        src_addr[i] = (uint8_t)i;
        dst_addr[i] = 0;
    }

    /********************************
     * setup DMA
     *******************************/

    // enable DMA2 clock, bit 22 on AHB1ENR
    RCC->AHB1ENR |= (1 << 22);

    // clear DMA Stream configuration register
    // single transfer, M0 is target, single buffer mode, circular buffer disabled
    DMA2_Stream0->CR = 0;
    // wait until dma is disabled
    while(DMA2_Stream0->CR & (1 << 0));

    // set channel CHSEL: bits27:25 to channel0
    DMA2_Stream0->CR |= (0 << 25);

    // set data transfer direction DIR: bits7:6 memory-to-memory
    DMA2_Stream0->CR |= (2 << 6);

    // set channel priority PL bits17:16 to medium
    DMA2_Stream0->CR |= (0x1 << 16);

    // increment memory MINC : bit10
    DMA2_Stream0->CR |= (1 << 10);
    // memory data size MSIZE : bits14:13 to byte
    DMA2_Stream0->CR |= (0 << 13);

    // increment peripheral PINC : bit9
    DMA2_Stream0->CR |= (1 << 9);
    // peripheral data size PSIZE : bits12:11 to byte
    DMA2_Stream0->CR |= (0 << 11);

    // source memory address
    DMA2_Stream0->PAR = (uint32_t)src_addr;
    // destination memory address
    DMA2_Stream0->M0AR = (uint32_t)dst_addr;
    // number of items to be transferred
    DMA2_Stream0->NDTR = BUF_SIZE;

    // enable transfer complete interrupt bit4
    DMA2_Stream0->CR |= (1 << 4);
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    // enable dma bit0
    DMA2_Stream0->CR |= (1 << 0);

    while(1)
    {
    }

    return 0;
}