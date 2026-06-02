#ifndef mcp_init_H_
#define mcp_init_H_
#include "stdint.h"

/* SPI Commands */
#define MCP_RESET        0xC0
#define MCP_READ         0x03
#define MCP_WRITE        0x02
#define MCP_BITMOD       0x05
#define MCP_READ_STATUS  0xA0
#define MCP_RTS_TX0      0x81

/* Important Registers */
#define MCP_CANSTAT  0x0E
#define MCP_CANCTRL  0x0F

#define MCP_CNF1     0x2A
#define MCP_CNF2     0x29
#define MCP_CNF3     0x28

#define MCP_CANINTE  0x2B
#define MCP_CANINTF  0x2C

/* Modes */
#define MODE_NORMAL  0x00
#define MODE_CONFIG  0x80   // REQOP = 100
#define MODE_MASK    0xE0



/* Functions */
void MCP2515_Reset(void);
uint8_t MCP2515_Read(uint8_t addr);
void MCP2515_Write(uint8_t addr, uint8_t data);
void MCP2515_BitModify(uint8_t addr, uint8_t mask, uint8_t data);
uint8_t MCP2515_ReadStatus(void);

uint8_t MCP2515_Init(void);


uint8_t MCP2515_SetLoopbackMode(void);


typedef struct {
    uint16_t id;          // 11-bit standard ID
    uint8_t dlc;          // Data length (0–8)
    uint8_t data[8];      // Data bytes
} CAN_Frame;

/* Send & Receive */
uint8_t MCP2515_SendMessage(CAN_Frame *frame);
uint8_t MCP2515_ReceiveMessage(CAN_Frame *frame);


#endif 