#include <stm32f407xx.h>

uint32_t src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
uint32_t dest[8] = {};
uint32_t i = 0;
	
	int main()
{
	// Initialize clocks
	RCC->AHB1ENR |= (1 << 22); // DMA2 clock enable

	// Desactivate stream 0
	DMA2_Stream0->CR &= ~(1U << 0);
	// Wait until the stream is disabled
	while (DMA2_Stream0->CR & (1 << 0))
		;
	// Configure channel and transfer mode
	// Select channel 0 (bits 27:25) : 000
	DMA2_Stream0->CR &= ~(1U << 25); // Channel 0
	DMA2_Stream0->CR &= ~(1U<< 26); // Channel 0
	DMA2_Stream0->CR &= ~(1U << 27); // Channel 0
	// Select memory-to-memory mode (bit 7:6) : 10
	DMA2_Stream0->CR &= ~(1U << 6); // Memory-to-memory mode
	DMA2_Stream0->CR |= (1 << 7);  // Memory-to-memory mode
	// Auto increment memory pointer of destination (bit 10)
	DMA2_Stream0->CR |= (1 << 10);
	// Auto increment memory pointer of source (bit 9)
	DMA2_Stream0->CR |= (1 << 9);
	// Configure transfer size (with word size) MSIZE[1:0] (bits 14:13) : 10 (Source)
	DMA2_Stream0->CR &= ~(1 << 13);
	DMA2_Stream0->CR |= (1 << 14);
	// Configure transfer size (with word size) PSIZE[1:0] (bits 12:11) : 10 (Destination)
	DMA2_Stream0->CR &= ~(1 << 11);
	DMA2_Stream0->CR |= (1 << 12);
	// High priority level (bits 17:16) : 10
	DMA2_Stream0->CR &= ~(1 << 16);
	DMA2_Stream0->CR |= (1 << 17);
	// Configure source address
	DMA2_Stream0->PAR = (uint32_t)src;
	// Configure destination address
	DMA2_Stream0->M0AR = (uint32_t)dest;
	// Configure number of data items to transfer (bits 15:0)
	DMA2_Stream0->NDTR = 8;
	// Activate interrupts (optional)
	DMA2_Stream0->CR |= (1 << 4); // Transfer complete interrupt enable
	// Start DMA transfer
	DMA2_Stream0->CR |= (1 << 0); // Enable the stream

	while (1)
	{
		// Start copying the data from the source to the destination using dma
		while ((DMA2->LISR & (1 << 5)) == 0)
			;					 // Wait until transfer complete flag is set (bit 5)
		DMA2->LIFCR |= (1 << 5); // Clear transfer complete flag (bit 5)
		DMA2_Stream0->CR &= ~(1U << 0);
	}

	return 0;
}