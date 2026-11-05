/*
    8/2021
    Thu vien Quan ly Modem
*/

#include "user_modem.h"

#include "user_rtc.h"
#include "onchipflash.h"
#include "user_adc.h"
//#include "dma.h"
//#include "spi.h"

/*===================Struct, Var=========================*/
ST_TIME_FORMAT          sRTCSet = {0};
/*================ struct Feature=====================*/
sModemVariable	sModemVar = 
{
    .ModePower_u8   = _POWER_MODE_SAVE,             // _POWER_MODE_ONLINE,    //  _POWER_MODE_SAVE,  // 
    .ModeConnNow_u8 = MODE_CONNECT_DATA_MAIN,
};

sModemInformation sModemInfor = 
{
    .Type_u8        = _CONTROL_OXY,
    .aID            = {"SVWMGB26009999"},
    
    .sFrequence.nTSVH_u8        = DEFAULT_NUM_TSVH,
    .sFrequence.Duration_u32    = DEFAULT_FREQ_WAKEUP,
     
    .sFrequence.DurAlarm_u32    = DEFAULT_DURA_ALARM,
        
    .sFrequence.DurOnline_u32   = DEFAULT_FREQ_SEND_ONLINE,
    .sFrequence.UnitMin_u16     = 2,
    
    //Server Infor
    .sServerMain.aIP        = {"124.158.5.135"},        //124.158.5.154   port 1883   117.6.163.177 
    .sServerMain.aPORT      = {"1883"},
    .sServerMain.aUSER      = {"syswmsv"},
    .sServerMain.aPASS      = {"sv@wmsv568"},
    
    .sServerMain.IPnum      = {124, 158, 5, 154},       // {117, 6, 163, 177},
    .sServerMain.Port_u16   = 1883,
    
    .sServerMain.KeepAlive_u32 = 60,
    .sServerMain.DomainOrIp_u8 = __SERVER_IP,
    
    //Server Infor
    .sServerBack.aIP        = {"124.158.5.135"},  
    .sServerBack.aPORT      = {"1883"},
    .sServerBack.aUSER      = {"syswmsv"},
    .sServerBack.aPASS      = {"sv@wmsv568"},
    
    .sServerBack.IPnum = {124, 158, 5, 135},
    .sServerBack.Port_u16 = 7525,
    
    .sServerBack.KeepAlive_u32 = 60,
    .sServerBack.DomainOrIp_u8 = __SERVER_IP, 
};


StructpFuncLPM sModemLPMHandle = 
{
    .LPM_Deinit_Peripheral          = Modem_Deinit_Peripheral,  
    .LPM_Init_Peripheral            = Modem_Init_Peripheral,
    .LPM_Deinit_Before_IRQ_Handle   = Modem_Deinit_Before_IRQ_Handle,
    .LPM_Init_Before_IRQ_Handle     = Modem_Init_Before_IRQ_Handle,
};
    


const sKindModem sSV_Modem[] =
{
	{ _MODEM_WM_NBIOT,              "DCU/WMSV/WME1/"        },
	{ _MODEM_WM_GSM,	            "DCU/WMSV/WMP2/"        },
	{ _MODEM_COS_PHI,		        "DCU/EMET/COSP/"        },
	{ _MODEM_EM_NBIOT,		        "DCU/EMSV/SVE1/"        },
    { _MODEM_LEVEL,		            "DCU/WMSV/WML1/"        },
    { _MODEM_WM_GSM_LORA,		    "DCU/WMSV/WMLG/"        },
    { _MODEM_TEMH_GSM,		        "DCU/THSV/THGS/"        },
    { _MODEM_CONV_GSM,		        "DCU/WMSV/WMPI/"        },
    { _CONTROL_OXY,                 "DCU/WMSV/WMCO/"        },
    { _MODEM_WM_GSM_ETH,		    "DCU/WMSV/WMGE/"        },
};


/*===================Function=========================*/    


void Modem_Init(void)
{
    Modem_Init_Var();

#ifdef USING_AT_CONFIG
    /* regis cb serial */
    sATCmdList[_SET_DEV_SERIAL].CallBack = Modem_SER_Set_ID;
    sATCmdList[_QUERY_DEV_SERIAL].CallBack = Modem_SER_Get_ID;
    
    sATCmdList[_SET_DUTY_READ_DATA].CallBack = Modem_SER_Set_Duty_Cycle;
    sATCmdList[_QUERY_DUTY_READ_DATA].CallBack = Modem_SER_Get_Duty_Cycle;
    
    sATCmdList[_SET_FREQ_ONLINE].CallBack = Modem_SER_Set_Dura_Online;
    sATCmdList[_QUERY_FREQ_ONLINE].CallBack = Modem_SER_Get_Dura_Online;
    
    sATCmdList[_SET_NUM_WAKEUP].CallBack = Modem_SER_Set_Numwakeup;
    
    sATCmdList[_SET_RTC].CallBack = Modem_SER_Set_sTime;
    sATCmdList[_QUERY_RTC].CallBack = Modem_SER_Get_sTime;  
    
    sATCmdList[_SET_LEVEL_DEBUG].CallBack = Modem_SER_Set_Level_Debug;
    sATCmdList[_QUERY_LEVEL_DEBUG].CallBack = Modem_SER_Get_Level_Debug; 
    
    sATCmdList[_RESET_MODEM].CallBack = Modem_SER_Req_Reset;
    sATCmdList[_QUERY_FIRM_VER].CallBack = Modem_SER_Get_Firm_Version;
    
    sATCmdList[_SET_SERVER_MAIN].CallBack = Modem_SER_Set_Server_Main;
    sATCmdList[_QUERY_SERVER_MAIN].CallBack = Modem_SER_Get_Server_Main;
    
    sATCmdList[_SET_SERVER_BACKUP].CallBack = Modem_SER_Set_Server_Back;
    sATCmdList[_QUERY_SERVER_BACKUP].CallBack = Modem_SER_Get_Server_Back;
 
    sATCmdList[_REQ_SWITCH_SERVER].CallBack = Modem_SER_Change_Server;
    
    sATCmdList[_QUERY_UP_FIRM_HTTP].CallBack = Modem_SER_HTTP_Update;
    sATCmdList[_QUERY_UP_FIRM_FTP].CallBack = Modem_SER_FTP_Update;
    
    sATCmdList[_SET_MODEM_RESET].CallBack = Modem_SER_Set_cReset;
    sATCmdList[_QUERY_MODEM_RESET].CallBack = Modem_SER_Get_cReset;  
    
    sATCmdList[_SET_TIME_ALARM].CallBack = Modem_SER_Set_Time_Alarm;
    sATCmdList[_QUERY_TIME_ALARM].CallBack = Modem_SER_Get_Time_Alarm; 
    
    sATCmdList[_QUERY_STATUS].CallBack = Modem_SER_Get_Status;  
    
    sATCmdList[_SET_PRESS_MODE].CallBack = Modem_SER_Set_Pressure_Mode; 
    sATCmdList[_QUERY_PRESS_MODE].CallBack = Modem_SER_Get_Pressure_Mode; 
#endif
    
#ifdef USING_APP_SIM
    /*regis cb for app sim*/
    sAppSimVar.pRespond_Str = Modem_Respond_Str,
    sAppSimVar.pReset_MCU = Modem_Reset_MCU;
    sAppSimVar.pReset_MCU_Imediate = Modem_Reset_MCU_Immediately;
    sAppSimVar.pReq_Set_sTime = Modem_Set_sTime;
    sAppSimVar.pReq_Save_Box = NULL;
    sAppSimVar.pCheck_AT_User = Modem_Check_AT;
    
    sAppSimVar.pModePower_u8 = &sModemVar.ModePower_u8;
    sAppSimVar.pModeConnNow_u8 = &sModemVar.ModeConnNow_u8;
    sAppSimVar.pModeConnFuture_u8 = &sModemVar.ModeConnFuture_u8;
    sAppSimVar.pModeConnLast_u8 = &sModemVar.ModeConnLast_u8;
    
    sAppSimVar.pSim_Set_Server = Modem_Set_Server_Infor_To_App;

    sCbSimComm.pGet_Fw_Version = Modem_Get_Firm_Version;       
        
    sSimFwUpdate.sServer.sIP                = sModemVar.sFwUpdate.sServer.aIP;
    sSimFwUpdate.sServer.sPort              = sModemVar.sFwUpdate.sServer.aPORT;
    sSimFwUpdate.sServer.sMQTTUserName      = sModemVar.sFwUpdate.sServer.aUSER;
    sSimFwUpdate.sServer.sMQTTPassword      = sModemVar.sFwUpdate.sServer.aPASS;

    sSimFwUpdate.pPath      = sModemVar.sFwUpdate.aPATH;
    sSimFwUpdate.pFileName  = sModemVar.sFwUpdate.aFILE_NAME;
    sSimFwUpdate.pFileURL   = sModemVar.sFwUpdate.aFILE_URL;
#endif
  
#ifdef USING_APP_WM
    /*regis cb for app wm*/
    sWmVar.pRespond_Str     = Modem_Respond_Str;
    sWmVar.pPack_Alarm_Str  = Modem_Packet_Alarm_String;
    sWmVar.pGet_ID          = Modem_Get_ID;
        
    sWmVar.pModePower_u8    = &sModemVar.ModePower_u8;
    sWmVar.pHardware_u8     = &sModemVar.DetHardware_u8;
#endif
    
    /*regis cb for app wm*/
    sMemVar.pReset_MCU              = Modem_Reset_MCU;
    sMemVar.pRespond_Str            = Modem_Respond_Str,
    sMemVar.pModePower_u8           = &sModemVar.ModePower_u8;
    sMemVar.pRestart                = Modem_PowOn_Mem;
    sMemVar.pRespond_Req            = Modem_Resp_From_Mem;
    
#ifdef USING_APP_ETHERNET 
    sAppEthVar.pReset_MCU = Modem_Reset_MCU;
    sAppEthVar.pReset_MCU_Imediate = Modem_Reset_MCU_Immediately;
    sAppEthVar.pReq_Save_Box = NULL;
    sAppEthVar.pGet_Fw_Version = Modem_Get_Firm_Version;
    
    sAppEthVar.pModePower_u8            = &sModemVar.ModePower_u8;
    sAppEthVar.pModeConnNow_u8          = &sModemVar.ModeConnNow_u8;
    sAppEthVar.pModeConnFuture_u8       = &sModemVar.ModeConnFuture_u8;
    sAppEthVar.pModeConnLast_u8         = &sModemVar.ModeConnLast_u8;
    
    sAppEthVar.sUpdate.pFirmIP     = &sModemInfor.sUpdate.FirmIP;
    sAppEthVar.sUpdate.pFirmPort   = &sModemInfor.sUpdate.FirmPort;
    sAppEthVar.sUpdate.pFirmUser   = &sModemInfor.sUpdate.FTPUser;   
    sAppEthVar.sUpdate.pFirmPass   = &sModemInfor.sUpdate.FTPPass;
        
    sAppEthVar.sUpdate.pFirmPath   = &sModemInfor.sUpdate.FirmPath;
    sAppEthVar.sUpdate.pFirmName   = &sModemInfor.sUpdate.FirmName;
    sAppEthVar.sUpdate.pUrlHTTP    = &sModemInfor.sUpdate.UrlHTTP;
    
    sAppEthVar.sUpdate.aIP              = &sModemInfor.sUpdate.aIP[0];
    sAppEthVar.sUpdate.Port_u16         = &sModemInfor.sUpdate.Port_u16;
        
    sAppEthVar.sUpdate.DomainOrIp_u8    = &sModemInfor.sUpdate.DomainOrIp_u8;
                
#endif
    
#ifdef USING_APP_MESS
    /*regis cb for message*/
    sMessage.pModemId = sModemInfor.aID;
    sMessage.pTopic = sSV_Modem[sModemInfor.Type_u8].pTopic;
    
    sMessage.pPacket_Handshake = Modem_Packet_MePDV;
    sMessage.pPacket_Status    = Modem_Packet_Mess_Status;
    sMessage.pPacket_Intan     = Modem_Packet_Mess_Intan;
    sMessage.pPacket_Alarm     = Modem_Packet_Mess_Alarm;
    sMessage.pPacket_RespAT    = Modem_Packet_Mess_RespAT;
    sMessage.pPacket_SimID     = Modem_Packet_Mess_SimID;
    
    sMessage.pSER_Config       = Modem_SER_Config;
    sMessage.pSER_Setting      = Modem_SER_Setting;
    sMessage.pSER_Setting_2    = Modem_SER_Setting_2; 
    sMessage.pSER_Set_Time     = Modem_SER_Set_Time;
    sMessage.pSER_Clear_Rec    = NULL;
    sMessage.pSER_Req_AT       = Modem_SER_Req_AT;
    sMessage.pSER_Req_RF       = Modem_SER_Req_RF;
    sMessage.pSER_Req_Update   = Modem_SER_Req_Update;
    
    sMemVar.pForward_Mess = mGet_Data_From_Mem;
    sMemVar.pReset_Buff_Sim = mReset_Raw_Data;
    sMemVar.pMessage_Pending = mIs_Sending_Message;
#endif
}



void Modem_Init_Var (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t     temp = 0;
    uint8_t 	aBuff[512] = {0};

    temp = *(__IO uint8_t*) (ADDR_MODEM_INFOR);
    //Check Byte EMPTY
    if (temp == BYTE_WRITEN) {
        OnchipFlashReadData(ADDR_MODEM_INFOR, &aBuff[0], 512);
        UTIL_MEM_cpy(&sModemInfor, &aBuff[1], sizeof(sModemInformation));   
    } else {
        Modem_Save_Var();
    }
#endif
}


void Modem_Save_Var (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aBuff[512] = {0};

    aBuff[0] = BYTE_WRITEN;
    UTIL_MEM_cpy(&aBuff[1], &sModemInfor, sizeof(sModemInformation));   
    
    Erase_Firmware(ADDR_MODEM_INFOR, 1);
    OnchipFlashWriteData(ADDR_MODEM_INFOR, &aBuff[0], 512);
#endif
}



uint8_t Modem_Reset_MCU (void)
{
#ifdef USING_APP_WM
    if (AppWm_Over_5Pulse() == true)
    {
#endif
    	UTIL_Log_Str(DBLEVEL_M, "u_modem: soft reset mcu\r\n");
  
    #ifdef BOARD_READ_PULSE
    #ifdef USING_APP_WM
        if (AppWm_Save_Pulse() == true)
    #endif
        {
        #ifdef USING_APP_SIM
            //Power off module sim
            SIM_PW_OFF1;
        #endif 
            __disable_irq();
            //Reset
            NVIC_SystemReset(); // Reset MCU
        }
    #else
        #ifdef USING_APP_SIM
            //Power off module sim
            SIM_PW_OFF1;
        #endif 
        __disable_irq();
        //Reset
        NVIC_SystemReset(); // Reset MCU
    #endif
        
#ifdef USING_APP_WM
    } else {
    	UTIL_Printf_Str(DBLEVEL_L, "u_modem: xung chua vuot qua 5\r\n");
    }
#endif

#ifdef USING_APP_WM
    return 0;
#endif
}


uint8_t Modem_Reset_MCU_Immediately (void)
{
	UTIL_Log_Str(DBLEVEL_M, "u_modem: soft reset mcu imediately\r\n");

#ifdef BOARD_READ_PULSE
#ifdef USING_APP_WM
    if (AppWm_Save_Pulse() == true)
#endif
    {
    #ifdef USING_APP_SIM
        //Power off module sim
        SIM_PW_OFF1;
    #endif 
        __disable_irq();
        //Reset
        NVIC_SystemReset(); // Reset MCU
    }
#else
#ifdef USING_APP_SIM
    //Power off module sim
    SIM_PW_OFF1;
#endif 
    __disable_irq();
    //Reset
    NVIC_SystemReset(); // Reset MCU
#endif
    
    
#ifdef USING_APP_WM
    return false;
#endif 
}



/*
    Func: vao che do luu kho
        + Power off module SIM
        + Go to standby mode
        + Wait wake up later
*/
void Modem_Save_Box (void)
{

}



void Modem_Packet_Alarm_String (const char *str)
{
//    uint16_t len = strlen(str);
//
    UTIL_Log_Str (DBLEVEL_M, str);
//        
//    if ( (strlen(sModemVar.aALARM) + len + 1) > MAX_LENGTH_ALARM ) {
//        UTIL_MEM_set(sModemVar.aALARM, 0, MAX_LENGTH_ALARM);
//    }
//    
//    UTIL_MEM_cpy (sModemVar.aALARM + strlen(sModemVar.aALARM), (char *) '#', 1);
//    UTIL_MEM_cpy (sModemVar.aALARM + strlen(sModemVar.aALARM), str, len);
//
//#ifdef USING_APP_MESS
//    sMessage.aMESS_PENDING[SEND_ALARM] = TRUE;
//#endif
}

                        
/*============ Func Modem App ======================*/    

void Modem_Respond_Str (uint8_t portNo, const char *str, uint8_t ack)
{
    switch(portNo)   
    {
        case _UART_DEBUG:
            HAL_UART_Transmit(&uart_debug, (uint8_t *) str, strlen(str), 1000);
            break;
        case _UART_SIM:
        case _UART_LORA: 
            UTIL_MEM_set (sModemVar.aAT_RESP, 0, sizeof ( sModemVar.aAT_RESP ));
            sprintf(sModemVar.aAT_RESP, "(%s)(%s)", sModemVar.aAT_CMD, str);
            UTIL_MEM_set(sModemVar.aAT_CMD, 0, sizeof(sModemVar.aAT_CMD));
        #ifdef USING_APP_MESS  
            if (ack == 0) {
                sMessage.aMESS_PENDING[SEND_RESPOND_SERVER] = TRUE;
            } else {
                sMessage.aMESS_PENDING[SEND_RESPOND_SERVER_ACK] = TRUE;
            }
        #endif
            break;
        case _UART_485:
            HAL_UART_Transmit(&uart_rs485, (uint8_t *) str, strlen(str), 1000);
            break;
        default:
            break;
    }
}

/*============ Func Pointer LPM lib ================*/

void Modem_Deinit_Peripheral (void)
{
#ifdef USING_APP_WM
    AppWm_Off_Energy_Consumption();
#endif
    
#ifdef USING_APP_SENSOR
    AppSensor_Off_Energy_Consumption();
#endif
    
#ifdef BOARD_LC_V1_1
    HAL_I2C_DeInit(&hi2c2);
#endif
    HAL_I2C_DeInit(&hi2c1);
    
    MX_GPIO_DeInit();

    ADC_Desequence_Powerhungry_Channels();
    
    HAL_ADC_DeInit(&hadc1);
    
    //Deinit Uart
#ifdef USING_APP_SIM
    Sim_Stop_RX_Mode();
    HAL_UART_DeInit(&uart_sim);
#endif
    
    AppComm_DBG_Stop_RX_Mode();
    HAL_UART_DeInit(&uart_debug);
    
    RS485_Stop_RX_Mode();
    HAL_UART_DeInit(&uart_rs485);
    
#ifdef BOARD_LC_V1_2
    HAL_SPI_DeInit(&hspi2);
#endif
}


void Modem_Init_Peripheral (void)
{
    Modem_Init_Gpio_Again(); 
#ifdef BOARD_LC_V1_1
    HAL_GPIO_WritePin(OTG_ON_OFF_GPIO_Port, OTG_ON_OFF_Pin, GPIO_PIN_RESET);
    MX_I2C2_Init();
#endif
    
#ifdef BOARD_LC_V1_2
    MX_SPI2_Init();
#endif
    
#ifdef USING_APP_SIM
    Sim_Stop_RX_Mode();
    Sim_Init_Uart();
    Sim_Init_RX_Mode();
#endif
    
    AppComm_DBG_Stop_RX_Mode();
    AppComm_DBG_Init_Uart(); 
    AppComm_DBG_Init_RX_Mode();
    
#ifdef USING_APP_WM
    MEASURE_PIN_ON;
    
    RS485_Stop_RX_Mode();
    RS485_Init_Uart(); 
    RS485_Init_RX_Mode();
#endif 
}

void Modem_Init_Before_IRQ_Handle (void)
{
#ifdef USING_APP_LORA
    AppLora_Init_IO_Radio();
#endif
}


void Modem_Deinit_Before_IRQ_Handle (void)
{
#ifdef USING_APP_LORA
    AppLora_Deinit_IO_Radio();
#endif
}

void Modem_Uart_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure ={0};
    
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    GPIO_InitStructure.Pin =  GPIO_PIN_2|GPIO_PIN_3;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStructure );
    
    GPIO_InitStructure.Pin =  GPIO_PIN_9|GPIO_PIN_10;  
    HAL_GPIO_Init( GPIOA, &GPIO_InitStructure );
    
    GPIO_InitStructure.Pin =  GPIO_PIN_10|GPIO_PIN_11;  
    HAL_GPIO_Init( GPIOC, &GPIO_InitStructure );
    
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_USART2_CLK_DISABLE();
    __HAL_RCC_USART3_CLK_DISABLE();
}


void MX_GPIO_DeInit(void)
{
#if defined (BOARD_LC_V1_1)
    GPIO_InitTypeDef GPIO_InitStructure ={0};
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStructure.Mode   = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull   = GPIO_NOPULL;
                 
    //Port A 
    GPIO_InitStructure.Pin = ( SIM_PWR_KEY_Pin | SIM_RESET_Pin | LED_Pin | RS485_TXDE_Pin) ;    
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    //Port B 
    GPIO_InitStructure.Pin = (  SIM_DTR_Pin | SIM_RI_Pin | PULSE_DTC_Pin) ; 
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  
    
    //Port C 
    GPIO_InitStructure.Pin = ( PULSE_LED_Pin | BLE_DET_Pin);   
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);  
#endif
    
#if defined (BOARD_LC_V1_2)
    GPIO_InitTypeDef GPIO_InitStructure ={0};
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStructure.Mode   = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull   = GPIO_NOPULL;
                 
    //Port A 
    GPIO_InitStructure.Pin = ( SIM_PWR_KEY_Pin | SIM_RESET_Pin | LED_Pin | RS485_TXDE_Pin) ;    
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    //Port B 
    GPIO_InitStructure.Pin = (  SIM_DTR_Pin | SIM_RI_Pin | PULSE_DTC_Pin) ; 
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  
    
    //Port C 
    GPIO_InitStructure.Pin = ( PULSE_LED_Pin);   
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);  
#endif
}


/*
    Func: Cau hinh lai nhung chan deinit truoc khi ngu
    */
void Modem_Init_Gpio_Again(void)
{
#if defined (BOARD_LC_V1_1)
     GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, SIM_PWR_KEY_Pin|SIM_RESET_Pin|LED_Pin|RS485_TXDE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SIM_DTR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PULSE_LED_GPIO_Port, PULSE_LED_Pin, GPIO_PIN_RESET);

//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(PIN_ON_OFF_GPIO_Port, PIN_ON_OFF_Pin, GPIO_PIN_SET);
  
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SIM_PWR_KEY_Pin;  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SIM_PWR_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = SIM_RESET_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SIM_DTR_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = SIM_RI_Pin|PULSE_DTC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = PULSE_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BLE_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLE_DET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RS485_TXDE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RS485_TXDE_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif
  
  
#if defined (BOARD_LC_V1_2)
     GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, SIM_PWR_KEY_Pin|SIM_RESET_Pin|LED_Pin|RS485_TXDE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SIM_DTR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SIM_PWR_KEY_Pin;  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SIM_PWR_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = SIM_RESET_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SIM_DTR_Pin; 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = SIM_RI_Pin|PULSE_DTC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin */
  GPIO_InitStruct.Pin = PULSE_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RS485_TXDE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RS485_TXDE_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif
}



///*Func Cb Setting serial*/
#ifdef USING_AT_CONFIG

void Modem_SER_Set_ID (sData *str, uint16_t Pos)
{
    uint8_t Fix = 0;
    uint16_t Length = 0;
    
    if (PortConfig == 2)
    	Fix = 1;

    if (str->Length_u16 <= (Pos + 20 + Fix))   	//1 byte crc phia sau neu dung server
    {
        Length = str->Length_u16 - Pos - Fix;
        
        if (Length >= sizeof(sModemInfor.aID))
            Length = sizeof(sModemInfor.aID) - 1;
    
        UTIL_MEM_set(sModemInfor.aID, 0, sizeof(sModemInfor.aID));
        for (uint16_t i = 0; i < Length; i++) {
            sModemInfor.aID[i] = *(str->Data_a8 + Pos + i);
        }

        Modem_Save_Var();
        Modem_Reset_MCU_Immediately();
        
        Modem_Respond_Str (PortConfig, "\r\nOK", 1);
    } else {
    	Modem_Respond_Str(PortConfig, "\r\nFAIL LENGTH!", 0);  
    }
}


void Modem_SER_Get_ID (sData *str, uint16_t Pos)
{
    Modem_Respond_Str (PortConfig, sModemInfor.aID, 0) ;
}

void Modem_SER_Set_Duty_Cycle (sData *strRecei, uint16_t Pos)
{
    uint16_t TempFre = 0;
    uint8_t nTSVH = 0;
    
    if (strRecei->Length_u16 < (Pos + 5))
    {
        Modem_Respond_Str(PortConfig, "\r\nFAIL LENGTH", 0);
        return;
    }
    
    nTSVH = *(strRecei->Data_a8 + Pos++) - 0x30;
    nTSVH = nTSVH * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;
    Pos++;
    TempFre = *(strRecei->Data_a8 + Pos++) - 0x30;
    TempFre = TempFre * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;

    if ( ((nTSVH > 0) && (nTSVH <= MAX_NUM_TSVH))
        && (TempFre <= 60) )
    {
        sModemInfor.sFrequence.nTSVH_u8 = nTSVH;
        sModemInfor.sFrequence.Duration_u32 = TempFre;
    
        Modem_Save_Var();
        AppComm_Set_Next_TxTimer();
        
        Modem_Respond_Str (PortConfig, "\r\nOK", 0);
        return;
    }
   
    Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
}


void Modem_SER_Get_Duty_Cycle (sData *strRecei, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "duty = %02dx%02d\r\n", sModemInfor.sFrequence.nTSVH_u8, sModemInfor.sFrequence.Duration_u32);

    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Set_Dura_Online (sData *strRecei, uint16_t Pos)
{
    char aTEMP[5] = {0};
    
    if (UTIL_Get_strNum((char *) (strRecei->Data_a8 + Pos), aTEMP, sizeof(aTEMP)) != NULL) {
        sModemInfor.sFrequence.DurOnline_u32 = UtilStringToInt(aTEMP);
        Modem_Save_Var ();
        //Set lai timer
        AppComm_Set_Next_TxTimer();
        
        Modem_Respond_Str(PortConfig, "\r\nOK",  1);
    } else {
        Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
    }
}


void Modem_SER_Get_Dura_Online (sData *strRecei, uint16_t Pos)
{
    char aData[32] = {0};

    sprintf(aData, "duration online = %d\r\n", sModemInfor.sFrequence.DurOnline_u32);
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Set_Time_Alarm (sData *strRecei, uint16_t Pos)
{
    char aTEMP[5] = {0};
    
    if (UTIL_Get_strNum((char *) (strRecei->Data_a8 + Pos), aTEMP, sizeof(aTEMP)) != NULL) {
        sModemInfor.sFrequence.DurAlarm_u32 = UtilStringToInt(aTEMP);
        
        Modem_Save_Var ();
        //Set lai timer alarm
  
        Modem_Respond_Str(PortConfig, "\r\nOK",  1);
    } else {
        Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
    }
}


void Modem_SER_Get_Time_Alarm (sData *strRecei, uint16_t Pos)
{
    char aData[32] = {0};

    sprintf(aData, "%d\r\n", sModemInfor.sFrequence.DurAlarm_u32);
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Set_cReset (sData *strRecei, uint16_t Pos)
{
    char aTEMP[5] = {0};
    if (UTIL_Get_strNum((char *) (strRecei->Data_a8 + Pos), aTEMP, sizeof(aTEMP)) != NULL) {
        sModemInfor.cReset_u16 = UtilStringToInt(aTEMP);
        Modem_Save_Var ();
        Modem_Respond_Str(PortConfig, "\r\nOK",  1);
    } else {
        Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
    }
}

//void Modem_SER_Get_cReset (sData *strRecei, uint16_t Pos)
//{
//    char aData[32] = {0};
//
//    sprintf(aData, "%d\r\n", sModemInfor.cReset_u16);
//    
//    Modem_Respond_Str(PortConfig, aData, 0);
//}

void Modem_SER_Get_cReset (sData *strRecei, uint16_t Pos)
{
    char aData[256] = {0};

    sprintf(aData, "%d/%d/%d %d:%d:%d: count = %d\r\n", sModemInfor.sTimeReset.year, sModemInfor.sTimeReset.month, sModemInfor.sTimeReset.date, 
                                                        sModemInfor.sTimeReset.hour, sModemInfor.sTimeReset.min, sModemInfor.sTimeReset.sec, 
                                                        sModemInfor.cReset_u16);
    
    sprintf(aData + strlen(aData), "RS source: %s", sModemInfor.aRS_RESOURCE);
                
    Modem_Respond_Str(PortConfig, aData, 0);
}

void Modem_SER_Set_Numwakeup (sData *strRecei, uint16_t Pos)
{
    uint8_t TempU8 = 0;

    TempU8 = *(strRecei->Data_a8 + Pos++) - 0x30;
    TempU8 = TempU8 * 10 + *(strRecei->Data_a8 + Pos++) - 0x30;
    
    if (TempU8 <= MAX_NUM_TSVH)
    {
        sModemInfor.sFrequence.nTSVH_u8 = TempU8;
        Modem_Save_Var ();
        
        Modem_Respond_Str(PortConfig, "\r\nOK",  1);
        return;
    }
    
    Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
}


void Modem_SER_Set_sTime (sData *strRecei, uint16_t Pos)
{
    ST_TIME_FORMAT   sRTC_temp = {0};
	
    if ( (*(strRecei->Data_a8+1) == '-') && (*(strRecei->Data_a8+4) == '-') &&  (*(strRecei->Data_a8+7) == '-')  
        && (*(strRecei->Data_a8+10) == ',') && (*(strRecei->Data_a8+13) == ':') && (*(strRecei->Data_a8+16) == ':') )
	{
		sRTC_temp.day   = (*(strRecei->Data_a8))-0x30 - 1;
		sRTC_temp.date  = ((*(strRecei->Data_a8 + 2))-0x30)*10 + (*(strRecei->Data_a8 + 3) - 0x30);
		sRTC_temp.month = ((*(strRecei->Data_a8 + 5))-0x30)*10 + (*(strRecei->Data_a8 + 6) - 0x30);
		sRTC_temp.year  = ((*(strRecei->Data_a8 + 8))-0x30)*10 + (*(strRecei->Data_a8 + 9) - 0x30);
  
        sRTC_temp.hour  = ((*(strRecei->Data_a8 + 11))-0x30)*10 + (*(strRecei->Data_a8 + 12) - 0x30);
		sRTC_temp.min   = ((*(strRecei->Data_a8 + 14))-0x30)*10 + (*(strRecei->Data_a8 + 15) - 0x30);
		sRTC_temp.sec   = ((*(strRecei->Data_a8 + 17))-0x30)*10 + (*(strRecei->Data_a8 + 18) - 0x30);
        
        if (Check_update_Time(&sRTC_temp) == true)
        {
            sModemVar.sRTCSet.year   = sRTC_temp.year;
            sModemVar.sRTCSet.month  = sRTC_temp.month;
            sModemVar.sRTCSet.date   = sRTC_temp.date;
            sModemVar.sRTCSet.hour   = sRTC_temp.hour;
            sModemVar.sRTCSet.min    = sRTC_temp.min;
            sModemVar.sRTCSet.sec    = sRTC_temp.sec;
            sModemVar.sRTCSet.day    = sRTC_temp.day % 7 + 1;

            fevent_active(sEventAppComm, _EVENT_SET_RTC);
            
            Modem_Respond_Str(PortConfig, "OK", 0);
            return;
        }
	}
    
    Modem_Respond_Str(PortConfig, "ERROR", 0);
}


void Modem_SER_Get_sTime (sData *strRecei, uint16_t Pos)
{
    char aData[48] = {0};
    
    Get_RTC();
    sprintf(aData, "\r\n%d-%d-%d-%d \r\n%d:%d:%d\r\n", sRTC.day, sRTC.date, sRTC.month, sRTC.year,
                                                        sRTC.hour, sRTC.min, sRTC.sec);
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Set_Level_Debug (sData *str_Receiv, uint16_t Pos)
{
    //set level debug
    uint8_t temp = *(str_Receiv->Data_a8 + Pos) - 0x30;
    
    if ( (temp >= DBLEVEL_MESS) && (temp <= DBLEVEL_H) )
    {
        VLevelDebug = temp;
//        UTIL_ADV_TRACE_SetVerboseLevel(DBLEVEL_M);
        
        Modem_Respond_Str(PortConfig, "OK", 0);
    } else
    {
        Modem_Respond_Str(PortConfig, "ERROR", 0);
    }
}

void Modem_SER_Get_Level_Debug (sData *str_Receiv, uint16_t Pos)
{
    char temp[2] = {VLevelDebug + 0x30, 0};
    
    Modem_Respond_Str(PortConfig, temp,  0);
}


void Modem_SER_Get_Firm_Version (sData *strRecei, uint16_t Pos)
{
    Modem_Respond_Str(PortConfig, sFirmVersion, 0);
}


void Modem_SER_Req_Reset (sData *strRecei, uint16_t Pos)
{
    Modem_Reset_MCU_Immediately();
}


uint8_t Modem_Extract_Server (char *src, sServerInfor *pServer)
{
    int16_t length = 0;
    
    length = UTIL_Cut_String(src, pServer->aIP,  ',', MAX_LENGTH_IP);
    if (length >= 0) {
        src += length + 1;
        length = UTIL_Cut_String(src, pServer->aPORT, ',', MAX_LENGTH_PORT);
        if (length >= 0) {
            src += length + 1;
            length = UTIL_Cut_String(src, pServer->aUSER, ',', MAX_LENGTH_USER_PASS);
            if (length >= 0) {
                src += length + 1;
                length = UTIL_Cut_String(src, pServer->aPASS, ',', MAX_LENGTH_USER_PASS);
                if (length >= 0) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void Modem_SER_Set_Server_Main (sData *strRecei, uint16_t Pos)
{
    sServerInfor sServerTemp = {0};

    if (Modem_Extract_Server((char *) (strRecei->Data_a8 + Pos), &sServerTemp) == true) {
        UTIL_MEM_set(&sModemInfor.sServerMain, 0, sizeof(sModemInfor.sServerMain));
        UTIL_MEM_cpy(&sModemInfor.sServerMain, &sServerTemp, sizeof(sModemInfor.sServerMain));
        sModemInfor.sServerMain.KeepAlive_u32 = 60;

        //Convert lai IP       
        if (UTIL_Convert_IP_To_Buff(sModemInfor.sServerMain.aIP, sModemInfor.sServerMain.IPnum) == true) {
            sModemInfor.sServerMain.DomainOrIp_u8 = __SERVER_IP;
        } else {
            sModemInfor.sServerMain.DomainOrIp_u8 = __SERVER_DOMAIN;
        }
        sModemInfor.sServerMain.Port_u16 = (uint16_t) UtilStringToInt(sModemInfor.sServerMain.aPORT);
        
        Modem_Save_Var();
        Modem_Respond_Str(PortConfig, "OK", 0);
        return;
    }
        
    Modem_Respond_Str(PortConfig, "ERROR", 0);
}


void Modem_SER_Set_Server_Back (sData *strRecei, uint16_t Pos)
{
    sServerInfor sServerTemp = {0};

    if (Modem_Extract_Server((char *) (strRecei->Data_a8 + Pos), &sServerTemp) == true) {
        UTIL_MEM_set(&sModemInfor.sServerBack, 0, sizeof(sModemInfor.sServerBack));
        UTIL_MEM_cpy(&sModemInfor.sServerBack, &sServerTemp, sizeof(sModemInfor.sServerBack));
        sModemInfor.sServerBack.KeepAlive_u32 = 60;

        //Convert lai IP       
        if (UTIL_Convert_IP_To_Buff(sModemInfor.sServerBack.aIP, sModemInfor.sServerBack.IPnum) == true) {
            sModemInfor.sServerBack.DomainOrIp_u8 = __SERVER_IP;
        } else {
            sModemInfor.sServerBack.DomainOrIp_u8 = __SERVER_DOMAIN;
        }
        sModemInfor.sServerBack.Port_u16 = (uint16_t) UtilStringToInt(sModemInfor.sServerBack.aPORT);
        
        Modem_Save_Var();
        Modem_Respond_Str(PortConfig, "OK", 0);
        return;
    }
        
    Modem_Respond_Str(PortConfig, "ERROR", 0);
}

void Modem_SER_Get_Server_Main (sData *strRecei, uint16_t Pos)
{
    char aData[128] = {0};
    
    sprintf((char*) aData, "%s:%s\r\n", sModemInfor.sServerMain.aIP,
                                        sModemInfor.sServerMain.aPORT);  
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Get_Server_Back (sData *strRecei, uint16_t Pos)
{
    char aData[128] = {0};
    
    sprintf((char*) aData, "%s:%s\r\n", sModemInfor.sServerBack.aIP,
                                        sModemInfor.sServerBack.aPORT);  
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Change_Server (sData *strRecei, uint16_t Pos)
{
    Modem_Change_Server();
}




uint8_t Modem_Extract_URL (sUpdateInfor *pUpdate, char *src)
{   
    char *pFind, *pName = NULL;
    char sURL[] = "http://";
    int16_t length = 0, i = 0;
    
    pFind = strstr(src, sURL);
    if (pFind != NULL) {
        src += strlen(sURL);
        length = UTIL_Cut_String(src, pUpdate->sServer.aIP, ':', MAX_LENGTH_IP);
        if (length >= 0) {
            src += length + 1;
            length = UTIL_Cut_String(src, pUpdate->sServer.aPORT, ',', MAX_LENGTH_PORT);
            if (length >= 0) {
                src += length + 1;
                length = UTIL_Cut_String(src, pUpdate->sServer.aUSER, ',', MAX_LENGTH_USER_PASS);
                if (length >= 0) {
                    src += length + 1;
                    length = UTIL_Cut_String(src, pUpdate->sServer.aPASS, '/', MAX_LENGTH_USER_PASS);
                }
            }
        }
    }
    
    if (length >= 0) 
    {
        src += length + 1;
        //copy
        if (strlen(src) < (MAX_LENGTH_FILE_PATH + MAX_LENGTH_FILE_NAME) ) 
        {
            sprintf(pUpdate->aFILE_URL, "%s%s:%s/%s", sURL, pUpdate->sServer.aIP, pUpdate->sServer.aPORT, src);  
                
            for (i = 0; i < strlen(src);i++) {
                if (*(src + i) == '/') {
                    pName = src + i;
                }
            }
            
            if (pName != NULL) {
                //copy vao path
                if ( (pName < (src + MAX_LENGTH_FILE_PATH))
                       && (strlen(pName) < MAX_LENGTH_FILE_NAME) ) {
                    UTIL_MEM_cpy(pUpdate->aPATH, src, (pName - src));
                    UTIL_MEM_cpy(pUpdate->aFILE_NAME, pName + 1, strlen(pName) - 1);
                    return true;
                }
            } else {
                if (strlen(src) < MAX_LENGTH_FILE_NAME) {
                    UTIL_MEM_cpy(pUpdate->aFILE_NAME, src, strlen(src));
                    return true;
                }
            }
        }
    }
    
    return false;
}    
    
    
void Modem_SER_HTTP_Update (sData *strRecei, uint16_t Pos)
{   
    char *src = (char *) (strRecei->Data_a8 + Pos);
    sUpdateInfor sUpdate = {0};
    
    if (Modem_Extract_URL(&sUpdate, src) == true) {
        UTIL_MEM_set(&sModemVar.sFwUpdate, 0, sizeof(sModemVar.sFwUpdate));
        UTIL_MEM_cpy(&sModemVar.sFwUpdate, &sUpdate, sizeof(sModemVar.sFwUpdate));
        UTIL_Printf_Str( DBLEVEL_M, sModemVar.sFwUpdate.aFILE_URL );

        sModemVar.ModeConnFuture_u8 = MODE_CONNECT_HTTP; 
        
    #ifdef USING_APP_ETHERNET
        sAppEthVar.Status_u8  = _ETH_START;
        fevent_active(sEventAppEth, _EVENT_ETH_HARD_RESET);
    #endif
        
    #ifdef USING_APP_SIM
    #ifdef USING_APP_ETHERNET
        if (sAppEthVar.Status_u8 < _ETH_TCP_CONNECT)
            AppSim_Restart_Imediate();
    #else
        AppSim_Restart_Imediate();
    #endif
    #endif
        
        Modem_Respond_Str(PortConfig, "OK", 0);
    } else {  
        Modem_Respond_Str(PortConfig, "ERROR", 0);
    }
}


void Modem_SER_FTP_Update (sData *strRecei, uint16_t Pos)
{
    char *src = (char *) (strRecei->Data_a8 + Pos);
    sUpdateInfor sUpdate = {0};
    
    if (Modem_Extract_URL(&sUpdate, src) == true) {
        UTIL_MEM_set(&sModemVar.sFwUpdate, 0, sizeof(sModemVar.sFwUpdate));
        UTIL_MEM_cpy(&sModemVar.sFwUpdate, &sUpdate, sizeof(sModemVar.sFwUpdate));
        UTIL_Printf_Str( DBLEVEL_M, sModemVar.sFwUpdate.aFILE_URL );

        sModemVar.ModeConnFuture_u8 = MODE_CONNECT_FTP; 
        
    #ifdef USING_APP_ETHERNET
        sAppEthVar.Status_u8  = _ETH_START;
        fevent_active(sEventAppEth, _EVENT_ETH_HARD_RESET);
    #endif
        
    #ifdef USING_APP_SIM
        #ifdef USING_APP_ETHERNET
            if (sAppEthVar.Status_u8 < _ETH_TCP_CONNECT)
                AppSim_Restart_Imediate();
        #else
            AppSim_Restart_Imediate();
        #endif
    #endif
        
        Modem_Respond_Str(PortConfig, "OK", 0);
    } else {  
        Modem_Respond_Str(PortConfig, "ERROR", 0);
    }
}




void Modem_SER_Set_Pressure_Mode(sData *strRecei, uint16_t Pos)
{
    uint8_t TempU8 = 0;

    TempU8 = *(strRecei->Data_a8 + Pos++) - 0x30;
    
    if (TempU8 <= MAX_NUM_TSVH)
    {
        sModemInfor.PressMode_u8 = MIN(1, TempU8);
        Modem_Save_Var();
        
        Modem_Respond_Str(PortConfig, "\r\nOK",  1);
        return;
    }
    
    Modem_Respond_Str(PortConfig, "\r\nERROR", 0);
    
}


void Modem_SER_Get_Pressure_Mode(sData *strRecei, uint16_t Pos)
{
    char aData[32] = {0};

    sprintf(aData, "%d\r\n", sModemInfor.PressMode_u8);
    
    Modem_Respond_Str(PortConfig, aData, 0);
}


void Modem_SER_Get_Status (sData *strRecei, uint16_t Pos)
{
    char aData[128] = {0}; 
    
    sprintf(aData + strlen(aData), "sim\r\ncsq : %d\r\nerr : %d\r\nlow_freq: %s\r\n", 
                            sSimCommInfor.RSSI_u8, sSimCommVar.sErrorCode.Code_u8, 
                            (iLSEorLSI_u8 == __LOW_FREQ_LSE) ? "LSE" : "LSI");
    
    sprintf(aData + strlen(aData), "mem\r\ntype : %d\r\nsize : %d\r\nstatus : %d\r\nerr: %d\r\n", 
                            sMemVar.Type_u8, sMemVar.Size_u16, sMemVar.Status_u8, sMemVar.Error_u8);
    
    Modem_Respond_Str(PortConfig, aData, 0);
}





#endif


/*
    Func: Set server Infor
        type: 0: Main
              1: Back
*/
void Modem_Set_Server_Infor_To_App(void)
{
    if (sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_MAIN)
    {
    #ifdef USING_APP_ETHERNET
        sAppEthVar.sServer.sIP = &sModemInfor.sServerMain.sIP;
        sAppEthVar.sServer.sPort = &sModemInfor.sServerMain.sPort;
        sAppEthVar.sServer.sMessageUserName = &sModemInfor.sServerMain.sMessageUserName;
        sAppEthVar.sServer.sMessagePassword = &sModemInfor.sServerMain.sMessagePassword;
        
        sAppEthVar.sServer.aIP = sModemInfor.sServerMain.aIP;
        sAppEthVar.sServer.Port_u16 = &sModemInfor.sServerMain.Port_u16;
            
        sAppEthVar.sServer.MQTTKeepAlive_u32 = &sModemInfor.sServerMain.MQTTKeepAlive_u32;
        sAppEthVar.sServer.DomainOrIp_u8 = &sModemInfor.sServerMain.DomainOrIp_u8 ;
    #endif
            
    #ifdef USING_APP_SIM
        sSimCommVar.sServer.sIP = sModemInfor.sServerMain.aIP;
        sSimCommVar.sServer.sPort = sModemInfor.sServerMain.aPORT;
        sSimCommVar.sServer.sMQTTUserName = sModemInfor.sServerMain.aUSER;
        sSimCommVar.sServer.sMQTTPassword = sModemInfor.sServerMain.aPASS;  
    #endif   
    } else 
    {
    #ifdef USING_APP_ETHERNET
        sAppEthVar.sServer.sIP = &sModemInfor.sServerBack.sIP;
        sAppEthVar.sServer.sPort = &sModemInfor.sServerBack.sPort;
        sAppEthVar.sServer.sMessageUserName = &sModemInfor.sServerBack.sMessageUserName;
        sAppEthVar.sServer.sMessagePassword = &sModemInfor.sServerBack.sMessagePassword;
        
        sAppEthVar.sServer.aIP = sModemInfor.sServerBack.aIP;
        sAppEthVar.sServer.Port_u16 = &sModemInfor.sServerBack.Port_u16;
            
        sAppEthVar.sServer.MQTTKeepAlive_u32 = &sModemInfor.sServerBack.MQTTKeepAlive_u32;
        sAppEthVar.sServer.DomainOrIp_u8 = &sModemInfor.sServerBack.DomainOrIp_u8 ;
    #endif
            
    #ifdef USING_APP_SIM
        sSimCommVar.sServer.sIP = sModemInfor.sServerBack.aIP;
        sSimCommVar.sServer.sPort = sModemInfor.sServerBack.aPORT;
        sSimCommVar.sServer.sMQTTUserName = sModemInfor.sServerBack.aUSER;
        sSimCommVar.sServer.sMQTTPassword = sModemInfor.sServerBack.aPASS;  
    #endif  
    }
}


//Func cb for message
void Modem_Packet_MePDV (sData *pData)
{
    uint16_t i = 0;
    uint8_t TempCrc = 0;
#ifdef USING_APP_SIM
    uint8_t aTEMP[128] = {0};
//    uint8_t Length = 0, nChannelTotal_u8 = 0;
#endif
    
    //Thoi gian thu thap
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    //Cuong do song
#ifdef USING_APP_SIM
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_RSSI_1, &sSimCommInfor.RSSI_u8, 1, 0x00);
#endif
    //Thoi gian thu thap
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_FREQ_SEND, &sModemInfor.sFrequence.UnitMin_u16, 2, 0x00);
#ifdef USING_APP_SIM
    //Seri SIM
    if (strlen(sSimCommInfor.aSIM_ID) != 0) {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SERI_SIM, sSimCommInfor.aSIM_ID, strlen(sSimCommInfor.aSIM_ID), 0xAA);
    }
    
    //Dia chi server dang connect
    if (sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_MAIN) {
        sprintf((char*) aTEMP, "%s:%s\r\n", sModemInfor.sServerMain.aIP, sModemInfor.sServerMain.aPORT);  
    } else {
        sprintf((char*) aTEMP, "%s:%s\r\n", sModemInfor.sServerBack.aIP, sModemInfor.sServerBack.aPORT); 
    }

    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SER_ADD, aTEMP, strlen((char *) aTEMP), 0xAA);

#endif
    
#ifdef USING_APP_SENSOR
    *(pData->Data_a8 + pData->Length_u16++) = OBIS_NUM_CHANNEL_WM;  
    *(pData->Data_a8 + pData->Length_u16++) = 0x01;   
    *(pData->Data_a8 + pData->Length_u16++) = MAX_CHANNEL_SS;  
#endif
    
    //Firmware version
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_FW_VERSION, sFirmVersion, strlen(sFirmVersion), 0xAA);
    
    // caculator crc
    pData->Length_u16++;
	for (i = 0; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= *(pData->Data_a8 + i);

    *(pData->Data_a8 + pData->Length_u16 - 1) = TempCrc;    
}


void Modem_Packet_Mess_Status (sData *pData)
{
    char *str = (char *) pData->Data_a8;

    //firmware version
    sprintf(str + strlen(str), "#%s#", sFirmVersion);
    
#ifdef USING_APP_SIM
    sprintf(str + strlen(str), "%s#CSQ-%d#", sSimCommInfor.aSIM_ID, sSimCommInfor.RSSI_u8);
#endif

#ifdef USING_APP_ETHERNET
    if (sAppEthVar.Status_u8 == _ETH_MQTT_CONNECTED) {
        sprintf(str + strlen(str), "Ethernet#"); 
    } else {
        sprintf(str + strlen(str), "Cellular#"); 
    }
#endif
}

extern uint8_t ucTCPResponseFrame [300]; //
extern uint16_t ucTCPResponseLen;

void Modem_Packet_Mess_Intan (sData *pData)
{
#if defined(USING_APP_WM) || defined (USING_APP_EMET)
//    AppWm_Packet_Intan (pData);
#endif
    
#if defined(USING_APP_SENSOR) 
    AppSensor_Packet_Intan (pData);
#endif
    
#if defined(USING_APP_SO)
    for (uint16_t i = 0; i < ucTCPResponseLen; i++) 
        *(pData->Data_a8 + pData->Length_u16++) = ucTCPResponseFrame[i];
#endif  
}


void Modem_Packet_Mess_Alarm (sData *pData)
{            
    for (uint16_t i = 0; i < strlen(sModemVar.aALARM); i++) {
        *(pData->Data_a8 + pData->Length_u16++) = sModemVar.aALARM[i];
    }

    UTIL_MEM_set(sModemVar.aALARM, 0, sizeof(sModemVar.aALARM) );
}

void Modem_Packet_Mess_RespAT (sData *pData)
{
    for (uint16_t i = 0; i < strlen(sModemVar.aAT_RESP); i++)
      *(pData->Data_a8 +  pData->Length_u16++) = sModemVar.aAT_RESP[i];
}


void Modem_Packet_Mess_SimID (sData *pData)
{
    uint8_t	    i = 0;
    uint8_t     TempCrc = 0;
    
#ifdef USING_APP_SIM
    //Seri SIM
    if (strlen(sSimCommInfor.aSIM_ID) != 0) {
        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SERI_SIM, sSimCommInfor.aSIM_ID, strlen(sSimCommInfor.aSIM_ID), 0xAA);
    }
#endif 

    // caculator crc
    pData->Length_u16++;
	for (i = 0; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= *(pData->Data_a8 + i);

    *(pData->Data_a8 + pData->Length_u16 - 1) = TempCrc;
}


void Modem_SER_Config (sData *strRecv, uint16_t pos)
{
    uint16_t  PosFix = pos;
    uint8_t   TempLeng;
    uint8_t   ObisConfig = 0;
    uint8_t   TempU8 = 0, i = 0;
    uint16_t  TempU16 = 0;
    //chay tu tren xuong duoi de check tat ca cac obis cau hinh
    while ((PosFix + 4) <= strRecv->Length_u16)   //vi byte cuoi la crc
    {
        TempLeng = 0;
        ObisConfig = *(strRecv->Data_a8 + PosFix++);
        switch(ObisConfig)   //luon luon co 1 byte crc phía sau
        {
            case 0x01:  //Lenh set duty cycle: Ex: 0102000A09.
                if (sModemVar.ModePower_u8 == _POWER_MODE_ONLINE)
                {
                    TempU16 = 0;
                    TempLeng = *(strRecv->Data_a8 + PosFix++);
                    for (i = 0; i < TempLeng; i++)
                        TempU16 = (TempU16 << 8 ) | *(strRecv->Data_a8 + PosFix++);

                    sModemInfor.sFrequence.DurOnline_u32 = TempU16;
                    
                } else
                {
                    TempU16 = 0;
                    TempLeng = *(strRecv->Data_a8 + PosFix++);
                    for (i = 0; i < TempLeng; i++)
                        TempU16 = (TempU16 << 8 ) | *(strRecv->Data_a8 + PosFix++);

                    sModemInfor.sFrequence.Duration_u32 = TempU16;
                }
                
                //Luu lai
                Modem_Save_Var();
                Modem_Respond_Str(_UART_SIM,"\r\nOK", 1);
                AppComm_Set_Next_TxTimer();
                break;
            case 0x02:   //Lenh Reset so xung ve 0: data là channel
                TempLeng = *(strRecv->Data_a8 + PosFix++);
                for (i = 0; i < TempLeng; i++)
                    TempU16 = TempU16 * 256 + *(strRecv->Data_a8 + PosFix++);
                
            #ifdef USING_APP_WM
                if ((TempU16 > 0) && (TempU16 <= MAX_CHANNEL)) {
                    sPulse[TempU16-1].Number_i64 = 0;
                    sPulse[TempU16-1].NumberInit_i64 = sPulse[TempU16-1].Number_i64;
                }
            #endif
                Modem_Respond_Str(_UART_SIM, "\r\nRESET SO NUOC OK!", 1);
                break;
            case 0x03:  //set ve che do luu kho: 03010103
                PosFix += 2;

                Modem_Respond_Str (_UART_SIM, "\r\nOK", 1);
                sModemVar.rSaveBox_u8 = true;
                break;
            case 0x06:  
                TempLeng = *(strRecv->Data_a8 + PosFix++);   //length
                TempU8 = *(strRecv->Data_a8 + PosFix++);

                if ((TempLeng == 1) && (TempU8 <= MAX_NUM_TSVH)) {
                    sModemInfor.sFrequence.nTSVH_u8 = TempU8;

                    Modem_Save_Var();
                    Modem_Respond_Str (_UART_SIM, "OK", 1);
                } else {
                    Modem_Respond_Str(_UART_SIM, "FAIL", 0);
                }
                
                break;
            default:
                return;
        }
    }
}
uint8_t Modem_SER_Setting (sData *strRecv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t   Obis, Len, scale, chann = 0xFF;
    uint8_t   Result = true;
    uint16_t  i = 0;
    uint64_t  TempU64 = 0;
        
    if ((Pos + 4) > strRecv->Length_u16) {
        return error;
    }
        
    //chay tu tren xuong duoi de check tat ca cac obis cau hinh //vi byte cuoi la crc
    while ( (Pos + 4) <= strRecv->Length_u16 )   
    {
        Len = 0; TempU64 = 0;
        
        Obis = *(strRecv->Data_a8 + Pos++);
        Len = *(strRecv->Data_a8 + Pos++);
        
        if (Len <= 8) {
            for (i = 0; i < Len; i++)
                TempU64 = TempU64 * 256 + *(strRecv->Data_a8 + Pos++);    
        } else {
            Result = false;
        }
        
        switch (Obis)   
        {
            case OBIS_SERI_SENSOR: //lay byte cuoi cung
                chann = (uint8_t) TempU64 - 0x30;
                if ((chann == 0) || (chann > (MAX_CHANNEL + MAX_SLAVE_MODBUS))) { 
                    Result = false;
                }
                break;
            case OBIS_SETT_PULSE_FACTOR:                 
                //lay scale vao he so xung
                if (chann <= MAX_CHANNEL) {
                    sPulse[chann - 1].FactorInt_i16 = TempU64;
                    sPulse[chann - 1].FactorDec_u8 = *(strRecv->Data_a8 + Pos++);
                } else {
                    Pos++;
                }
                break;
            case OBIS_SETT_PULSE_START:   
                if (chann <= MAX_CHANNEL) {
                    scale = *(strRecv->Data_a8 + Pos++);
                    sPulse[chann - 1].Start_lf = ((double) TempU64) * Convert_Scale(scale);
                } else {
                    Pos++;
                }
                break;
            case OBIS_SETT_LI_IN_MIN:                
                Pos++; // scale
                sWmVar.aPRESSURE[chann - 1].sLinearInter.InMin_u16 = ( uint16_t ) TempU64;   
                break;
            case OBIS_SETT_LI_IN_MAX:
                Pos++; // scale
                sWmVar.aPRESSURE[chann - 1].sLinearInter.InMax_u16 = ( uint16_t ) TempU64;
                break;
            case OBIS_SETT_LI_IN_UNIT:
                if (TempU64 <= _UNIT_BAR)
                    sWmVar.aPRESSURE[chann - 1].sLinearInter.InUnit_u8 = (uint8_t) TempU64;
                break;
            case OBIS_SETT_LI_OUT_MIN:
                Pos++; // scale
                sWmVar.aPRESSURE[chann - 1].sLinearInter.OutMin_u16 = ( uint16_t ) TempU64; 
                break;
            case OBIS_SETT_LI_OUT_MAX:
                Pos++; // scale
                sWmVar.aPRESSURE[chann - 1].sLinearInter.OutMax_u16 = ( uint16_t ) TempU64;  
                break;
            case OBIS_SETT_LI_OUT_UNIT:
                if (TempU64 <= _UNIT_BAR)
                    sWmVar.aPRESSURE[chann - 1].sLinearInter.OutUnit_u8 = (uint8_t) TempU64;
                break;
            case OBIS_SETT_PRESS_FACTOR:  
                sWmVar.aPRESSURE[chann - 1].sLinearInter.FactorDec_u8 = *(strRecv->Data_a8 + Pos++);
                sWmVar.aPRESSURE[chann - 1].sLinearInter.Factor_u16 = (uint16_t) TempU64;

                if ( (sWmVar.aPRESSURE[chann - 1].sLinearInter.FactorDec_u8 < 0xFC) && (sWmVar.aPRESSURE[chann - 1].sLinearInter.FactorDec_u8 >= 2)) {
                    sWmVar.aPRESSURE[chann - 1].sLinearInter.FactorDec_u8 = 0;
                }
                break;
            case OBIS_SETT_PRESS_TYPE:
                if (TempU64 < __AN_END)
                    sWmVar.aPRESSURE[chann - 1].sLinearInter.Type_u8 = (uint8_t) TempU64;
                    
                if (chann > MAX_CHANNEL) {
                    sWmDigVar.sModbDevData[chann - MAX_CHANNEL - 1].TypeSett_u8 = (uint8_t) TempU64;
                }
                break;
            case OBIS_VAL_HI_QUAN:                 
                scale = *(strRecv->Data_a8 + Pos++);
                if (chann <= MAX_CHANNEL) {
                    sMeterThreshold[chann - 1].Quan[0].ValMax_f = TempU64 * Convert_Scale(scale);
                }
                break;
            case OBIS_VAL_LOW_QUAN:
                scale = *(strRecv->Data_a8 + Pos++);
                if (chann <= MAX_CHANNEL) {
                    sMeterThreshold[chann - 1].Quan[0].ValMin_f = TempU64 * Convert_Scale(scale);
                }
                break;
            case OBIS_VAL_HI_FLOW:
                scale = *(strRecv->Data_a8 + Pos++);
                if (chann <= MAX_CHANNEL) {
                    sMeterThreshold[chann - 1].Flow[0].ValMax_f = TempU64 * Convert_Scale(scale);
                }
                break;
            case OBIS_VAL_LOW_FLOW:
                scale = *(strRecv->Data_a8 + Pos++);
                if (chann <= MAX_CHANNEL) {
                    sMeterThreshold[chann - 1].Flow[0].ValMin_f = TempU64 * Convert_Scale(scale);
                }
                break;
            case OBIS_VAL_LOW_PIN:
                scale = *(strRecv->Data_a8 + Pos++);
                if (chann <= MAX_CHANNEL) {
                    sMeterThreshold[chann - 1].LowBatery = (uint16_t) MIN( (TempU64 * Convert_Scale(scale)), 100 );
                }
                break;
            case OBIS_WM_LEVEL_WIRE:
                if (chann > MAX_CHANNEL) {
                    scale = *(strRecv->Data_a8 + Pos++);
                    sWmDigVar.sModbDevData[chann - MAX_CHANNEL - 1].Lwire_f = ((float) TempU64) * Convert_Scale(scale);
                } else {
                    Pos++;
                }
                break;
            case OBIS_WM_LEVEL_VAL_STA:
                if (chann > MAX_CHANNEL) {
                    scale = *(strRecv->Data_a8 + Pos++);
                    sWmDigVar.sModbDevData[chann - MAX_CHANNEL - 1].Lstatic_f = ((float) TempU64) * Convert_Scale(scale);
                } else {
                    Pos++;
                }
                break;
            default:
                if ( (Obis == 0x0D) && (Len == 0x0A) ) {
                    Pos = strRecv->Length_u16;  //ket thuc
                } else {
                    Result = false;
                }
                break;
        }
        
        if (Result == false)
            break;
    }
    
    AppWm_Save_WM_Dig_Infor();
    AppWm_Save_Press_Infor();
    AppWm_Save_Pulse();

    return Result;
#else
    return true;
#endif
}



uint8_t Modem_SER_Setting_2 (sData *strRecv, uint16_t Pos)
{
#ifdef USING_APP_WM
    uint8_t   Obis, Len, scale, chann = 0xFF, block = 0;
    uint8_t   Result = true;
    uint16_t  i = 0;
    uint64_t  TempU64 = 0;
    sThreshold  sThreshConfig = {0};
        
    if ((Pos + 4) > strRecv->Length_u16) {
        return error;
    }
        
    //chay tu tren xuong duoi de check tat ca cac obis cau hinh //vi byte cuoi la crc
    while ( (Pos + 4) <= strRecv->Length_u16 )   
    {
        Len = 0; TempU64 = 0;
        UTIL_MEM_set (&sThreshConfig, 0, sizeof(sThreshold)); 
         
        Obis = *(strRecv->Data_a8 + Pos++);
        Len = *(strRecv->Data_a8 + Pos++);
        
        if ((Obis == OBIS_SERI_SENSOR) 
            || (Obis == OBIS_THRESH_PIN) ) {
            if (Len <= 8) {
                for (i = 0; i < Len; i++)
                    TempU64 = TempU64 * 256 + *(strRecv->Data_a8 + Pos++);    
            } else {
                Result = false;
            }
        }

        switch (Obis)   
        {
            case OBIS_SERI_SENSOR: 
                chann = (uint8_t) TempU64 - 0x30;
                if ((chann == 0) || (chann > MAX_CHANNEL)) {
                    Result = false;
                }
                break;
            case OBIS_THRESH_QUAN:  
            case OBIS_THRESH_FLOW:
            case OBIS_THRESH_PRESS:
            case OBIS_THRESH_LEVEL:
                block = *(strRecv->Data_a8 + Pos++);
                if ((block == 0) || (block > MAX_BLOCK)) {        
                    sThreshConfig.status_u8 = true;
                    sThreshConfig.tStart_u16 = *(strRecv->Data_a8 + Pos) << 8 | *(strRecv->Data_a8 + Pos + 1);  Pos += 2;
                    sThreshConfig.tEnd_u16 = *(strRecv->Data_a8 + Pos) << 8 | *(strRecv->Data_a8 + Pos + 1);  Pos += 2;
                    
                    sThreshConfig.ValMin_f = *( (float*)(strRecv->Data_a8 + Pos) ); Pos += 4;
                    sThreshConfig.ValMax_f = *( (float*)(strRecv->Data_a8 + Pos) ); Pos += 4;
                    
                    switch (Obis) 
                    {
                        case OBIS_THRESH_QUAN:  
                            UTIL_MEM_cpy (&sMeterThreshold[chann - 1].Quan[block], &sThreshConfig, sizeof(sThreshold)); 
                            break;
                        case OBIS_THRESH_FLOW:
                            UTIL_MEM_cpy (&sMeterThreshold[chann - 1].Flow[block], &sThreshConfig, sizeof(sThreshold)); 
                            break;
                        case OBIS_THRESH_PRESS:
                            UTIL_MEM_cpy (&sMeterThreshold[chann - 1].Press[block], &sThreshConfig, sizeof(sThreshold)); 
                            break;
                        case OBIS_THRESH_LEVEL:
                            UTIL_MEM_cpy (&sMeterThreshold[chann - 1].Level[block], &sThreshConfig, sizeof(sThreshold)); 
                            break;
                            break;
                        default:
                            break;
                    }   
                } else {
                    Result = false;
                }
                break;
            case OBIS_THRESH_PIN:
                scale = *(strRecv->Data_a8 + Pos++);
                sMeterThreshold[chann - 1].LowBatery = (uint16_t) MIN( (TempU64 * Convert_Scale(scale)), 100 );
                break;
            default:
                if ( (Obis == 0x0D) && (Len == 0x0A) ) {
                    Pos = strRecv->Length_u16;  //ket thuc
                } else {
                    Result = false;
                }
                Result = false;
                break;
        }
        
        if (Result == false)
            break;
    }
    
    AppWm_Save_Press_Infor();

    return Result;
#else
    return true;
#endif
}


void Modem_SER_Set_Time (sData *strRecv, uint16_t Pos)
{
#ifdef USING_APP_MESS
    ST_TIME_FORMAT 	sRTC_temp = {0};

	if (Pos > 0)
    { // nhan duoc ban tin Realtime
		sRTC_temp.year 		= *(strRecv->Data_a8 + Pos);
		sRTC_temp.month 	= *(strRecv->Data_a8 + Pos + 1);
		sRTC_temp.date 		= *(strRecv->Data_a8 + Pos + 2);
		sRTC_temp.day 		= *(strRecv->Data_a8 + Pos + 3);
		sRTC_temp.hour 		= *(strRecv->Data_a8 + Pos + 4);
		sRTC_temp.min 		= *(strRecv->Data_a8 + Pos + 5);
		sRTC_temp.sec 		= *(strRecv->Data_a8 + Pos + 6);
	}
    
    if (Modem_Set_sTime(&sRTC_temp, 0) == true) {
        sMessage.aMESS_PENDING[SEND_SERVER_TIME_OK] = 1;
	} else {
		if ((*(strRecv->Data_a8 + Pos + 2) == 'U') && (*(strRecv->Data_a8 + Pos + 3) == 'd')) {
			sMessage.aMESS_PENDING[SEND_SERVER_TIME_PENDING] = 1;
		} else {
			sMessage.aMESS_PENDING[SEND_SERVER_TIME_FAIL] = 1;
        }
	}
#endif
}


void Modem_SER_Req_AT (sData *strRecv, uint16_t pos)
{
    Modem_Check_AT(strRecv, _UART_SIM);  
}

void Modem_SER_Req_RF (sData *strRecv, uint16_t pos)
{
    Modem_Check_AT(strRecv, _UART_LORA);  
}


void Modem_SER_Req_Update (sData *strRecv, uint16_t pos)
{
    Modem_Respond_Str(_UART_SIM, "\r\nERROR", 0);
}




/*
 *  Return 	1 : Neu chia cac chuoi ten ra thanh cong
 *  		0 : ko thanh cong
 */

uint8_t	 Modem_Split_Update_Infor (sData *str_Receiv, uint16_t Pos)
{
     
	return 1;
}


uint8_t Modem_Check_AT(sData *str, uint8_t Type)
{
#ifdef USING_AT_CONFIG
	uint8_t var;
	int Pos_Str = -1;
	uint16_t i = 0;
    uint8_t aDATA_CONFIG[128] = {0};
	sData sDataConfig = {&aDATA_CONFIG[0], 0};

	for (var = _SET_DEV_SERIAL; var < _END_AT_CMD; var++)
	{
		Pos_Str = Find_String_V2((sData*) &sATCmdList[var].sCmd, str);
		if (Pos_Str >= 0)
		{
            if (sATCmdList[var].CallBack == NULL) {
                Modem_Respond_Str(Type, "\r\nNOT SUPPORT!\r\n", 0);
                return true;
            }
            
            //Copy lenh vao buff. de repond kem theo lenh
            UTIL_MEM_set(sModemVar.aAT_CMD, 0, sizeof(sModemVar.aAT_CMD));
            UTIL_MEM_cpy(sModemVar.aAT_CMD, sATCmdList[var].sCmd.Data_a8, sATCmdList[var].sCmd.Length_u16);
            
            //Copy data after at cmd
            Pos_Str += sATCmdList[var].sCmd.Length_u16;  
            for (i = Pos_Str; i < str->Length_u16; i++)
            {
                if (*(str->Data_a8 + i) == 0x0D) {
                    if ( (i < (str->Length_u16 - 1)) && (*(str->Data_a8 + i + 1) == 0x0A) ) {
                        break;
                    }
                }
                //
                if ( sDataConfig.Length_u16 < sizeof(aDATA_CONFIG) ) {
                    *(sDataConfig.Data_a8 + sDataConfig.Length_u16++) = *(str->Data_a8 + i);
                }
            }
            
            PortConfig = Type;
            
            switch (Type)
            {
                case _UART_DEBUG:
                case _UART_SIM: 
                case _UART_485: 
                    sATCmdList[var].CallBack(&sDataConfig, 0);
                    break;
                default:
                    break;
            }

			return true;
		}
	}
#endif
    
	return false;
}


/*
    Func: monitor connect server
        + change server if connect retry
        + reset mcu if 12 hour disconnect
*/
void Modem_Monitor_Connect_Server (void)
{
    //check change server
    if ( (sAppSimVar.cHardReset_u16 > MAX_SIM_CHANGE_SERVER)
    #ifdef USING_APP_ETHERNET
        && (sAppEthVar.cHardReset_u16 > MAX_ETH_CHANGE_SERVER) 
    #endif
        )
    {
        sAppSimVar.cHardReset_u16 = 0;
    #ifdef USING_APP_ETHERNET
        sAppEthVar.cHardReset_u16 = 0;
    #endif
        
        //doi server
        if ((sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_MAIN) 
                && (sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_BACKUP))
        {
            //doi mode neu dang o update
            sModemVar.ModeConnNow_u8 = sModemVar.ModeConnLast_u8;
            sMessage.aMESS_PENDING[SEND_UPDATE_FIRM_FAIL] = TRUE;
        } else
        {
//            UTIL_Log_Str(DBLEVEL_M, "u_modem: change server >>> \r\n" );
//            Modem_Change_Server();
        }
    }

    //check reset mcu
#ifdef USING_APP_SIM
    if (Check_Time_Out(sAppSimVar.LandMarkConnMQTT_u32, MAX_TIME_DISCONNECT) == true) 
#endif
    {  
    #ifdef USING_APP_ETHERNET
        if (Check_Time_Out(sAppEthVar.LandMarkConnMQTT_u32, MAX_TIME_DISCONNECT) == true)
    #endif
        {
            if ((sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_MAIN) 
                || (sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_BACKUP))
            {
                if (sModemVar.ModePower_u8 == _POWER_MODE_ONLINE)
                    Modem_Reset_MCU();
            }
        }
    }
}




uint8_t Modem_Set_sTime (ST_TIME_FORMAT *sClock, uint8_t GMT)
{
    if (Check_update_Time(sClock) == true)
    {
        sModemVar.sRTCSet.year   = sClock->year;
        sModemVar.sRTCSet.month  = sClock->month;
        sModemVar.sRTCSet.date   = sClock->date;
        sModemVar.sRTCSet.hour   = sClock->hour;
        sModemVar.sRTCSet.min    = sClock->min;
        sModemVar.sRTCSet.sec    = sClock->sec;
        sModemVar.sRTCSet.day    = sClock->day;

        Convert_sTime_ToGMT(&sModemVar.sRTCSet, GMT); 
        //Convert lai day. 1/1/2012 la chu nhat. Thu 2 - cn: 2-8
        sModemVar.sRTCSet.day = ((HW_RTC_GetCalendarValue_Second (sModemVar.sRTCSet, 1) / SECONDS_IN_1DAY) + 6) % 7 + 1;
        
        fevent_active(sEventAppComm, _EVENT_SET_RTC);
        
        return true;
    }
    
    return false;
}



char* Modem_Get_Firm_Version (void)
{
    return sFirmVersion;
}

char* Modem_Get_ID (void)
{
    return sModemInfor.aID;
}



uint8_t Modem_Change_Server (void)
{
    //Doi server
    if (sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_BACKUP)
    {
        sModemVar.ModeConnNow_u8 = MODE_CONNECT_DATA_MAIN;
        Modem_Set_Server_Infor_To_App();
    } else if (sModemVar.ModeConnNow_u8 == MODE_CONNECT_DATA_MAIN)
    {
        sModemVar.ModeConnNow_u8 = MODE_CONNECT_DATA_BACKUP;
        Modem_Set_Server_Infor_To_App();
    }
    
    sMessage.aMESS_PENDING[DATA_HANDSHAKE] = TRUE;
    
#ifdef USING_APP_ETHERNET
    sAppEthVar.Status_u8  = _ETH_START;
    fevent_active(sEventAppEth, _EVENT_ETH_HARD_RESET); 
#endif
    
#ifdef USING_APP_SIM
#ifdef USING_APP_ETHERNET
    if (sAppEthVar.Status_u8 < _ETH_TCP_CONNECT)
        AppSim_Restart_Imediate();
#else
    AppSim_Restart_Imediate();
#endif
#endif
    
    return 1;
}

void Modem_PowOn_Mem (void)
{
    FLASH_RESET_OFF;
    FLASH_POWER_OFF;
    HAL_Delay(200);
    FLASH_POWER_ON;
}


void Modem_Resp_From_Mem(uint8_t tdata, uint8_t *pdata, uint16_t length)
{
    char aData[512] = {0};
    
    if (length >= 230) {
        return;
    }
    
    sprintf(aData + strlen(aData), "\r\nrecord: ");
    
    if (sMemVar.sMemReq.Type_u8 == _MEM_DATA_LOG) {
        for (uint16_t i = 0; i < length; i++) {
            sprintf(aData + strlen(aData), "%c", pdata[i]);
        } 
    } else {
        for (uint16_t i = 0; i < length; i++) {
            sprintf(aData + strlen(aData), "%02X", pdata[i]);
        }
    }
    sprintf(aData + strlen(aData), "-end\r\n");
    
    Modem_Respond_Str(sMemVar.sMemReq.Port_u8, aData, 0);
}


