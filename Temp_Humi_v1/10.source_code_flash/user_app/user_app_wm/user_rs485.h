
#ifndef _USER_RS485_
#define _USER_RS485_    _USER_RS485_H

#include "user_util.h"
#include "main.h"
#include "usart.h"

/*======================== Define ======================*/
/*
    * Uart danh cho 485 so 1
        + Baurate
        + Port uart
        ...
*/
extern DMA_HandleTypeDef            hdma_usart1_rx;

#define uart_rs485	                huart1
#define uart_dma_rs485              hdma_usart1_rx

#define MX_UART_RS485_Init          MX_USART1_UART_Init            //Func init Uart

/* Definition for USARTx's NVIC */
#define USART_RS485_IRQn            USART1_IRQn
#define USART_RS485_Priority        3

/*-- Rx uart: DMA with IDLE line | Interupt line  ----*/
#define UART_RS485_DMA              0
#define UART_RS485_IT               1

#define UART_RS485_MODE             UART_RS485_IT

#define DE_GPIO_PORT                RS485_TXDE_GPIO_Port  
#define DE_GPIO_PIN                 RS485_TXDE_Pin     

/*-------------------------------*/
#define RS485_POWER_PORT    RS485_ON_OFF_GPIO_Port  
#define RS485_POWER_PIN     RS485_ON_OFF_Pin    

#define RS485_ON        HAL_GPIO_WritePin(RS485_POWER_PORT, RS485_POWER_PIN, GPIO_PIN_RESET);
#define RS485_OFF       HAL_GPIO_WritePin(RS485_POWER_PORT, RS485_POWER_PIN, GPIO_PIN_SET);


extern uint8_t Rs485Status_u8;

extern sData sUart485;
extern void (*pRS485_Rx_Done) (void);
/*======================== Funcion ======================*/
void RS485_Init_Data (void);
void RS485_Init_Uart (void);
void RS485_Init_RX_Mode(void);
void RS485_Stop_RX_Mode (void);
void RS485_Rx_Callback (uint16_t Size);
void RS485_Modbus_Send (uint8_t SlaveID, uint8_t Func, uint16_t Addr, uint16_t nRegis);

void RS485_1_Trans(uint8_t *Data, uint16_t Length);
void RS485_2_Trans(uint8_t *Data, uint16_t Length);

#endif






