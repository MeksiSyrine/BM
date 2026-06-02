#include "mcp_init.h"
#include "spi.h"
#include "stm32f4xx.h"
#include <stdint.h>

/*
** Resets internal registers to default state, set Configuration mode.
*/
void MCP2515_Reset(void)
{
    cs_enable();
    uint8_t cmd = MCP_RESET;
    spi1_transmit(&cmd, 1);
    cs_disable();
}

/*
** Read data from register beginning at selected address.
*/
uint8_t MCP2515_Read(uint8_t addr)
{
    uint8_t cmd[2];
    uint8_t data;

    cmd[0] = MCP_READ;
    cmd[1] = addr;

    cs_enable();
    spi1_transmit(cmd, 2); 
    spi1_receive(&data, 1);
    cs_disable();

    return data;
}

void MCP2515_Write(uint8_t addr, uint8_t data)
{
    uint8_t buf[3];
    buf[0] = MCP_WRITE;
    buf[1] = addr;
    buf[2] = data;

    cs_enable();
    spi1_transmit(buf, 3); // send data + size
    cs_disable();
}


/*
**  Allows the user to set or clear individual bits in a particular register. 
*/
void MCP2515_BitModify(uint8_t addr, uint8_t mask, uint8_t data)
{
    uint8_t buf[4];
    buf[0] = MCP_BITMOD;
    buf[1] = addr;
    buf[2] = mask;
    buf[3] = data;

    cs_enable();
    spi1_transmit(buf, 4);
    cs_disable();
}

/*
***  Quick polling command that reads several status bits for transmit and receive functions.
*/
uint8_t MCP2515_ReadStatus(void)
{
    uint8_t cmd = MCP_READ_STATUS;
    uint8_t status;

    cs_enable();
    spi1_transmit(&cmd, 1);
    spi1_receive(&status, 1);
    cs_disable();

    return status;
}


uint8_t MCP2515_Init(void)
{
    /* Hardware Reset */
    MCP2515_Reset();

    /* Delay (~10ms) */
    for (volatile int i = 0; i < 50000; i++);

    /* --- Ensure device is in CONFIG mode --- */
    uint8_t mode = MCP2515_Read(MCP_CANSTAT) & MODE_MASK;
    if (mode != MODE_CONFIG)
        return 1;  // failed

    /* --- Configure Bit Timing (500 kbps, 16 MHz crystal) --- */
    MCP2515_Write(MCP_CNF1, 0x00); // SJW=1, BRP=0
    MCP2515_Write(MCP_CNF2, 0x90); // BTLMODE=1, PHSEG1=3, PRSEG=2
    MCP2515_Write(MCP_CNF3, 0x02); // PHSEG2=3

    /* --- Allow all messages (disable filters for now) --- */
    MCP2515_Write(0x60, 0x00); // RXB0CTRL
    MCP2515_Write(0x70, 0x00); // RXB1CTRL

    /* --- Enable Receive Interrupts --- */
    MCP2515_Write(MCP_CANINTE, 0x03); // RX0IE + RX1IE

    /* --- Enter NORMAL mode --- */
    MCP2515_BitModify(MCP_CANCTRL, MODE_MASK, MODE_NORMAL);

    /* Confirm mode */
    mode = MCP2515_Read(MCP_CANSTAT) & MODE_MASK;
    if (mode != MODE_NORMAL)
        return 2;  // failed

    return 0; // success
}




uint8_t MCP2515_SendMessage(CAN_Frame *frame)
{
    /* --------- Load TXB0 ID (Standard) --------- */
    uint8_t id_high = (frame->id >> 3) & 0xFF;
    uint8_t id_low  = (frame->id << 5) & 0xE0;

    MCP2515_Write(0x31, id_high);  // TXB0SIDH
    MCP2515_Write(0x32, id_low);   // TXB0SIDL

    /* --------- Set Data Length --------- */
    uint8_t dlc = frame->dlc & 0x0F;
    MCP2515_Write(0x35, dlc);

    /* --------- Load Data Bytes --------- */
    for (uint8_t i = 0; i < frame->dlc; i++)
        MCP2515_Write(0x36 + i, frame->data[i]);

    /* --------- Request Transmission on TXB0 --------- */
    cs_enable();
    uint8_t cmd = MCP_RTS_TX0; // 0x81
    spi1_transmit(&cmd, 1);
    cs_disable();

    /* --------- Verify Transmission --------- */
    uint8_t status = MCP2515_ReadStatus();

    // bit2 ? TXB0IF (message sent)
    if (status & (1 << 2))
        return 0;  // success

    return 1;      // fail
}



uint8_t MCP2515_ReceiveMessage(CAN_Frame *frame)
{
    uint8_t status = MCP2515_ReadStatus();

    /* ------------------------
       Check RX Buffer 0
       ------------------------ */
    if (status & 0x01)  // bit0 = RX0IF
    {
        /* Read ID */
        uint8_t idh = MCP2515_Read(0x61);  // RXB0SIDH
        uint8_t idl = MCP2515_Read(0x62);  // RXB0SIDL

        frame->id = (idh << 3) | (idl >> 5);

        /* Read DLC */
        frame->dlc = MCP2515_Read(0x65) & 0x0F;

        /* Read Data */
        for (uint8_t i = 0; i < frame->dlc; i++)
            frame->data[i] = MCP2515_Read(0x66 + i);

        /* Clear RX0IF */
        MCP2515_BitModify(MCP_CANINTF, 0x01, 0x00);

        return 0; // success
    }

    /* ------------------------
       Check RX Buffer 1
       ------------------------ */
    if (status & 0x02)  // bit1 = RX1IF
    {
        uint8_t idh = MCP2515_Read(0x71);  // RXB1SIDH
        uint8_t idl = MCP2515_Read(0x72);  // RXB1SIDL

        frame->id = (idh << 3) | (idl >> 5);

        frame->dlc = MCP2515_Read(0x75) & 0x0F;

        for (uint8_t i = 0; i < frame->dlc; i++)
            frame->data[i] = MCP2515_Read(0x76 + i);

        /* Clear RX1IF */
        MCP2515_BitModify(MCP_CANINTF, 0x02, 0x00);

        return 0;  // success
    }

    return 1; // no message available
}



uint8_t MCP2515_SetLoopbackMode(void)
{
    /* Request CONFIG mode first */
    MCP2515_BitModify(MCP_CANCTRL, MODE_MASK, MODE_CONFIG);

    /* Wait */
    for (volatile int i = 0; i < 50000; i++);

    /* Verify CONFIG mode */
    uint8_t mode = MCP2515_Read(MCP_CANSTAT) & MODE_MASK;
    if (mode != MODE_CONFIG)
        return 1; // failed to enter config mode

    /* Set BIT TIMING */
    MCP2515_Write(MCP_CNF1, 0x00);
    MCP2515_Write(MCP_CNF2, 0x90);
    MCP2515_Write(MCP_CNF3, 0x02);

    /* Enable Loopback Mode (REQOP = 010) */
    MCP2515_BitModify(MCP_CANCTRL, MODE_MASK, 0x40);

    /* Small delay */
    for (volatile int i = 0; i < 50000; i++);

    /* Confirm Loopback Mode */
    mode = MCP2515_Read(MCP_CANSTAT) & MODE_MASK;
    if (mode != 0x40)
        return 2; // failed to enter loopback mode

    return 0; // success
}
