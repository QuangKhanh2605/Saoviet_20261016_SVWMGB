/*
 * user_app.h
 *
 *  Created on: Dec 14, 2021
 *      Author: Chien
 */

#ifndef INC_USER_APP_COMM_H_
#define INC_USER_APP_COMM_H_


#include "user_util.h"
#include "user_define.h"
#include "event_driven.h"


#define USING_APP_COMM
         
//#define USB_CDC_DEBUG
#ifdef USB_CDC_DEBUG
    #include "usbd_cdc_if.h"
#endif

#define HARD_DET     false// ( (HAL_GPIO_ReadPin(HARD_DETECT_GPIO_Port, HARD_DETECT_Pin) == GPIO_PIN_SET) ? false : true )

#define MAX_TIME_ALARM           2 *3600   //s
#define MAX_RETRY_SET_LSE        3

typedef enum
{
	_EVENT_TIMMER_IRQ = 0,       //0
	_EVENT_UART_RX_DEBUG,        //1
    _EVENT_UART_ERROR,
    _EVENT_SET_RTC,              //2
	_EVENT_IDLE,                 //3
    _EVENT_TX_TIMER,             //4
    _EVENT_SAVE_BOX,             //6
	_EVENT_END_COMM,             //7
}eKindEventAppCommon;


extern char sFirmVersion[];
extern sEvent_struct    sEventAppComm[];

/*=============Function=======================*/
void    SysApp_Init (void);
void    SysApp_Setting (void);
void    Main_Task (void);

uint8_t AppComm_Task (void);
void    AppComm_Init (void);

void    AppComm_IRQ_Timer_CallBack (void);
uint32_t AppComm_Get_Random_Follow_ID (void);

uint8_t AppComm_Set_Next_TxTimer (void);
uint8_t AppComm_Get_Reset_Source (void);

void    AppComm_DBG_Init_Uart (void);
void    AppComm_DBG_Init_RX_Mode(void);
void    AppComm_DBG_Stop_RX_Mode (void);
void    AppComm_DBG_Rx_Callback (uint16_t Size);

//UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *));
//UTIL_ADV_TRACE_Status_t vcom_DeInit(void);
//void                    vcom_Trace(uint8_t *p_data, uint16_t size);
//UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size);
//void                    vcom_Resume(void);

uint8_t AppComm_Save_Log (uint8_t *pData, uint16_t Length);
uint8_t AppComm_Check_RTC_Alarm (void) ;
uint8_t AppComm_Check_Tx_Timer (void) ;
uint8_t AppComm_Is_Init_LSE(void);
uint8_t AppComm_Save_Mark_LSE (uint8_t val);

void AppComm_Start_Timer (uint32_t sec);
void AppComm_Stop_Timer(void);

#endif /* INC_USER_APP_H_ */
