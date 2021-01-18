#ifndef __24L01_H
#define __24L01_H
#include "sys.h"

#define NRF_READ_REG    0x00
#define NRF_WRITE_REG   0x20
#define RD_RX_PLOAD     0x61
#define WR_TX_PLOAD     0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3
#define NOP             0xFF

#define CONFIG          0x00

#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07

#define MAX_TX  		0x10
#define TX_OK   		0x20
#define RX_OK   		0x40

#define OBSERVE_TX      0x08
#define CD              0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define NRF_FIFO_STATUS 0x17


#define NRF24L01_CE     PAout(4)
#define NRF24L01_CSN    PCout(4)
#define NRF24L01_IRQ    PAin(1)


#define TX_ADR_WIDTH    5
#define RX_ADR_WIDTH    5
#define TX_PLOAD_WIDTH  32
#define RX_PLOAD_WIDTH  32


void NRF24L01_Init(void);
void NRF24L01_RX_Mode(void);
void NRF24L01_TX_Mode(void);
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 u8s);
u8 NRF24L01_Read_Buf(u8 reg, u8 *pBuf, u8 u8s);
u8 NRF24L01_Read_Reg(u8 reg);
u8 NRF24L01_Write_Reg(u8 reg, u8 value);
u8 NRF24L01_Check(void);
u8 NRF24L01_TxPacket(u8 *txbuf);
u8 NRF24L01_RxPacket(u8 *rxbuf);

#endif
