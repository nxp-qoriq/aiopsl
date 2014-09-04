
#ifndef __FSL_DMA_H
#define __FSL_DMA_H

int dma_get_icontext(uint16_t icid, void **icontext);
int dma_read(void *icontext, uint16_t size, uint64_t src, void *dest);
int dma_write(void *icontext, uint16_t size, void *src, uint64_t dest);

#endif /* __FSL_DMA_H */
