

#include "user_app_wm.h"

#include "user_string.h"
#include "string.h"
#include "user_json.h"

#include "user_adc.h"
#include "user_timer.h"
#include "user_time.h"

#include "user_internal_mem.h"
#include "user_uart.h"

#include "user_app_comm.h"
#include "user_modbus_rtu.h"

/*================ Define =================*/
static uint8_t     _Cb_Entry_Wm (uint8_t event);
static uint8_t     _Cb_Log_TSVH (uint8_t event);
static uint8_t     _Cb_ext_irq (uint8_t event);

static uint8_t     _Cb_Scan_Alarm (uint8_t event);
static uint8_t     _Cb_Control_Led1 (uint8_t event);
/*================ Struct =================*/
sEvent_struct sEventAppWM [] =
{
    { _EVENT_ENTRY_WM, 		    0, 0, 500, 	    _Cb_Entry_Wm }, 
        
    { _EVENT_LOG_TSVH, 		    0, 0, 500, 	    _Cb_Log_TSVH },
    { _EVENT_EXT_IRQ,           0, 0, 100, 	    _Cb_ext_irq },               

    { _EVENT_SCAN_ALARM,	    1, 0, 50,       _Cb_Scan_Alarm }, 
    { _EVENT_CONTROL_LED1,		1, 0, 200,      _Cb_Control_Led1 }, 
};
            
static uint8_t aMARK_ALARM[10];
static uint32_t aLandMarkAlarm_u32[10];

sAppWmVariable sAppWmVar = 
{
    .nChannel_u8 = MAX_CHANNEL,
};

sAppWmInformation sAppWmInfor = 
{
    .sThreshold = {
                    .FlowHigh = 0xFFFF,
                    .FlowLow = 0,
                    .PeakHigh = 0xFFFF,
                    .PeakLow = 0,
                    .LowBatery = 10,
                    .LevelHigh = 4000,
                    .LevelLow  = 10,
                }, 
};


static GPIO_TypeDef*  LED_PORT[1] = {LED_1_GPIO_Port};
static const uint16_t LED_PIN[1] = {LED_1_Pin};

/*================ Struct =================*/


void AppWm_Init (void)
{   
    //Init Pulse
    AppWm_Init_Infor();
    
#ifdef USING_AT_CONFIG
    sATCmdList[_SET_DEFAULT_PULSE].CallBack = AppWm_SER_Set_Default_Pulse;
    
    sATCmdList[_SET_FLOW_THRESH].CallBack   = AppWm_SER_Set_Thresh_Flow;
    sATCmdList[_QUERY_FLOW_THRESH].CallBack = AppWm_SER_Get_Thresh_Flow;
    
    sATCmdList[_SET_QUAN_THRESH].CallBack   = AppWm_SER_Set_Thresh_Quan;
    sATCmdList[_QUERY_QUAN_THRESH].CallBack = AppWm_SER_Get_Thresh_Quan;
    
    sATCmdList[_SET_BAT_THRESH].CallBack    = AppWm_SER_Set_Thresh_Bat;
    sATCmdList[_QUERY_BAT_THRESH].CallBack  = AppWm_SER_Get_Thresh_Bat;
    
    sATCmdList[_SET_PULSE_VALUE].CallBack   = AppWm_SER_Set_Pulse;
    sATCmdList[_QUERY_PULSE_VALUE].CallBack = AppWm_SER_Get_Pulse;
    
    sATCmdList[_SET_LEVEL_THRESH].CallBack  = AppWm_SER_Set_Level_Thresh;
    sATCmdList[_QUERY_LEVEL_THRESH].CallBack = AppWm_SER_Get_Level_Thresh;
#endif
}


uint8_t AppWm_Task(void)
{
	uint8_t i = 0;
	uint8_t Result = 0;

	for (i = 0; i < _EVENT_END_WM; i++)
	{
		if (sEventAppWM[i].e_status == 1)
		{
            if ( (i != _EVENT_SCAN_ALARM) 
                  && (i != _EVENT_CONTROL_LED1) )
                Result = 1;

			if ((sEventAppWM[i].e_systick == 0) ||
					((HAL_GetTick() - sEventAppWM[i].e_systick)  >=  sEventAppWM[i].e_period))
			{
                sEventAppWM[i].e_status = 0;  //Disable event
				sEventAppWM[i].e_systick = HAL_GetTick();
				sEventAppWM[i].e_function_handler(i);
			}
		}
	}
    
	return Result;
}



static uint8_t _Cb_Entry_Wm (uint8_t event)
{
    fevent_active(sEventAppWM, _EVENT_LOG_TSVH);
    fevent_active(sEventAppWM, _EVENT_SCAN_ALARM);
    
	return 1;
}


static uint8_t _Cb_Log_TSVH (uint8_t event)
{
	//Log Data meter to flash
    UTIL_Printf_Str(DBLEVEL_M, "u_app_wm:log tsvh data\r\n");
    
    Get_RTC();
	AppWm_Get_VBat();
    AppWm_Get_VOut();
    
    APP_LOG(TS_OFF, DBLEVEL_M, "u_app_wm: Vpin in: %d mV \r\n", sAppWmInfor.sBattery.mVol_u32);
    
    #ifdef LPTIM_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
        AppWm_Plus_5Pulse(&sAppWmInfor.sPulse[0]);
    #endif
            
    //Luu lai xung
    AppWm_Save_Infor();
    
    if (sRTC.year < 20) {
    	return 0;
    }

    AppWm_Log_Data_TSVH();  

	return 1;
}


static uint8_t _Cb_ext_irq(uint8_t event)
{
    UTIL_Printf_Str(DBLEVEL_M, "u_app_wm: irq send intan\r\n");

	sAppWmVar.rIntan_u8 = true;
	//Active event Measure Press
	fevent_active(sEventAppWM, _EVENT_ENTRY_WM);

#ifdef USING_APP_SIM
    AppSim_Restart_If_PSM(); 
    
    if (sAppWmVar.pRq_Instant != NULL)
        sAppWmVar.pRq_Instant();
#endif

    return 1;
}


static uint8_t _Cb_Scan_Alarm(uint8_t event)
{   
    static uint16_t cCheck = 0;

    if ( cCheck++ >= (TIME_CHECK_ALARM / sEventAppWM[event].e_period) )
    {
        cCheck = 0;
    #ifdef LPTIM_ENCODER_MODE 
        LPTIM_Encoder_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
    #else   
        LPTIM_Counter_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
        AppWm_Plus_5Pulse(&sAppWmInfor.sPulse[0]);
    #endif

        Get_RTC();
        AppWm_Get_VBat();
        AppWm_Get_VOut();
        AppWm_Cacal_Param();
        
        if (sRTC.year > 20) {
            if (AppWm_Scan_Alarm() == 1) {
                AppWm_Log_Data_Event();
            }
        }
    }
            
    fevent_enable(sEventAppWM, event);
        
	return 1;
}


/*
    Func: Set Mode Control Led
        - Mode: + _LED_MODE_ONLINE_INIT     : Nhay deu 1s
                + _LED_MODE_CONNECT_SERVER  : Nhay Duty: 430ms off, 70ms on
                + _LED_MODE_UPDATE_FW       : Nhay deu 100ms
                + _LED_MODE_POWER_SAVE      : Off led
                + _LED_MODE_TEST_PULSE      : Nhay theo xung doc vao
*/
uint8_t AppWm_Get_Mode_Led (void)
{
    uint8_t Result = 0;
    
    #ifdef USING_APP_SIM
        if ((sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_MAIN) 
            && (sModemVar.ModeConnNow_u8 != MODE_CONNECT_DATA_BACKUP))  
        {
            Result = _LED_MODE_UPDATE_FW;
        } else if (sAppWmVar.rTestsPulse_u8 == FALSE)
        {
            if ( (sSimCommVar.State_u8 == _SIM_CONN_MQTT)
            #ifdef USING_APP_ETHERNET
                || (sAppEthVar.Status_u8 == _ETH_MQTT_CONNECTED) 
            #endif
                    )
                Result = _LED_MODE_CONNECT_SERVER;
            else
                Result = _LED_MODE_ONLINE_INIT;
        } else
            Result = _LED_MODE_TEST_PULSE;
    #else
        if (sAppWmVar.rTestsPulse_u8 == FALSE) 
        {
            Result = _LED_MODE_ONLINE_INIT;
        } else
            Result = _LED_MODE_TEST_PULSE;  
    #endif
        
    return Result;
}

/*
    Func: CB Event Control Led
*/
static uint8_t _Cb_Control_Led1(uint8_t event)
{
    uint8_t ModeControl = 0;
    static uint8_t CountToggle = 0;
    static uint8_t MarkFirst = 0;
    static uint8_t Status = 0;
    static uint64_t LastPulseControl = 0;
    static uint32_t LastTimeCheckPulse = 0;
    
    if ( (sAppWmVar.rDefault_u8 == true) && (CountToggle <= 10) ) {
        ModeControl = _LED_MODE_DEFAULT_DEV;
    } else {
        ModeControl = AppWm_Get_Mode_Led();
    }
    
    //Handler in step control one
    switch ( ModeControl )
    {
        case _LED_MODE_DEFAULT_DEV: 
            CountToggle++;
            sEventAppWM[event].e_period = 100;
            LED_Toggle(_LED_STATUS);
            break;
        case _LED_MODE_ONLINE_INIT:
            sEventAppWM[event].e_period = 1000;
            LED_Toggle(_LED_STATUS);
            break;
        case _LED_MODE_CONNECT_SERVER:
            if (sEventAppWM[event].e_period != 430) {
                sEventAppWM[event].e_period = 430;
                LED_Off (_LED_STATUS);
            } else {
                sEventAppWM[event].e_period = 70;
                LED_On (_LED_STATUS);
            }
            break;
        case _LED_MODE_UPDATE_FW:
            sEventAppWM[event].e_period = 100;
            LED_Toggle(_LED_STATUS);
            break;      
        case _LED_MODE_POWER_SAVE:
            sEventAppWM[event].e_period = 1000;
            LED_Off (_LED_STATUS);
            break;
        case _LED_MODE_TEST_PULSE:
            sEventAppWM[event].e_period = 200;
            if (MarkFirst == 0)
            {
                MarkFirst = 1;
                LastPulseControl = sAppWmInfor.sPulse[0].Number_u64;
                LastTimeCheckPulse = HAL_GetTick();
                LED_On (_LED_STATUS);
                
                return 1;
            }
            
        #ifdef LPTIM_ENCODER_MODE 
            LPTIM_Encoder_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
        #else   
            LPTIM_Counter_Get_Pulse(&hlptim1, &sAppWmInfor.sPulse[0]);
            AppWm_Plus_5Pulse(&sAppWmInfor.sPulse[0]);
        #endif
            
            if (Cal_Time(LastTimeCheckPulse, HAL_GetTick()) <= 60000)
            {
                //Neu co xung nuoc thay doi -> Nhay led
                if (LastPulseControl != sAppWmInfor.sPulse[0].Number_u64)
                {
                    LastPulseControl = sAppWmInfor.sPulse[0].Number_u64;
                    
                    if (Status == FALSE)
                        Status = TRUE;
                } 
                
                if (Status == TRUE)
                {
                    LED_Off (_LED_STATUS);
                    Status = PENDING;
                } else if (Status == PENDING)   
                {
                    LED_On (_LED_STATUS);
                    Status = FALSE;
                }
            }
        default:
            break;
    }
    
    fevent_enable(sEventAppWM, event);
       
    return 1;
}

/*================ Function Handler =================*/

uint16_t AppWm_Get_VBat(void)
{
    //Get batterry
    sAppWmInfor.sBattery.mVol_u32 = ADC_Get_Value (ADC_CHANNEL_9);
    sAppWmInfor.sBattery.mVol_u32 *= 2;   //Phan ap chia 2
    sAppWmInfor.sBattery.Level_u16 = ADC_Convert_Vol_To_Percent (sAppWmInfor.sBattery.mVol_u32, VDD_BAT, VDD_MIN); 

	return sAppWmInfor.sBattery.Level_u16;
}

uint16_t AppWm_Get_VOut(void)
{
    sAppWmInfor.sVout.mVol_u32 = ADC_Get_Value(ADC_CHANNEL_10);
    sAppWmInfor.sVout.mVol_u32 = sAppWmInfor.sVout.mVol_u32 * 2;
    sAppWmInfor.sVout.Level_u16 = ADC_Convert_Vol_To_Percent (sAppWmInfor.sVout.mVol_u32, VDD_OUT_MAX, VDD_OUT_MIN); 
   
	return sAppWmInfor.sVout.Level_u16;
}

//neu khoi dong lai: cong them 5 xung ban dau cua che do counter
void AppWm_Plus_5Pulse (Struct_Pulse *sPulse)
{    
    if ( (sPulse->IsOver5Pulse_u8 == false)
        && (sPulse->Number_u64 != sPulse->NumberInit_u64) ) {
        if (sPulse->Direct_u8 == 1) {
            sPulse->Number_u64  -= 5;
        } else {
            sPulse->Number_u64  += 5;
        }
        
        sPulse->IsOver5Pulse_u8 = true;
    }
}

/*
    Func: Tat het nguon tieu thu nang luong cua app temh
        + Off 12v
        + Off Led 1 2 3
*/

void AppWm_Off_Energy_Consumption (void)
{
    LED_Off (_LED_STATUS);
}


void AppWm_Cacal_Param (void)
{
    static uint8_t mCacal = false;
    static uint32_t LastTime = 0;
    static uint64_t LastNum = 0;
    uint32_t CurrTime = HW_RTC_GetCalendarValue_ms(sRTC);
    uint32_t DelTime = 0;
        
    if ( (mCacal == false ) || (sRTC.year == 0) ) {
        LastNum = sAppWmInfor.sPulse[0].Number_u64;
        LastTime = CurrTime;
    } else {
    #ifdef LPTIM_ENCODER_MODE
        sAppWmInfor.sPulse[0].Quantity_i32 = AppWm_Cacul_Quantity(sAppWmInfor.sPulse[0].Number_u64, LastNum) / 4;
    #else
        sAppWmInfor.sPulse[0].Quantity_i32 = AppWm_Cacul_Quantity(sAppWmInfor.sPulse[0].Number_u64, LastNum);
    #endif
        
        DelTime = CurrTime - LastTime;
        sAppWmInfor.sPulse[0].Flow_i32 = (sAppWmInfor.sPulse[0].Quantity_i32 * 3600) / DelTime; 
    }
}

/*
    - Function tinh toan va check alarm
        + Co the doc ra tu record truoc va lay lai duoc chi so o phia truoc + thoi gian: Sau do tính
        + Neu nhu thiet bi duoc cam moi hoan toan: 
                *Index Record  = 0
                *Page cuoi cung se bi xoa roi. Nen se check ra firstbyte = 0 va length = 0.
                * Khi do check alarm se khong bao alarm
*/

uint8_t AppWm_Scan_Alarm (void)
{
    uint8_t i = 0;

    //so sanh voi cac threshold va Set flag mark alarm. 
    if (sAppWmInfor.sPulse[0].Quantity_i32 > sAppWmInfor.sThreshold.PeakHigh) 
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_PEAK_HIGH ) == TRUE)
            if (sAppWmVar.pPack_Alarm_Str != NULL)
                sAppWmVar.pPack_Alarm_Str ("u_app_wm: high quantity!\r\n");
    }
    //Peak Low
    if (sAppWmInfor.sPulse[0].Quantity_i32 < sAppWmInfor.sThreshold.PeakLow)  
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_PEAK_LOW ) == TRUE)
            if (sAppWmVar.pPack_Alarm_Str != NULL)
                sAppWmVar.pPack_Alarm_Str ("u_app_wm: low quantity!\r\n");
    } 
    //Flow high
    if (sAppWmInfor.sPulse[0].Flow_i32 > sAppWmInfor.sThreshold.FlowHigh) 
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_FLOW_HIGH ) == TRUE)
            if (sAppWmVar.pPack_Alarm_Str != NULL)
                sAppWmVar.pPack_Alarm_Str ("u_app_wm: high flow!\r\n");
    } 
    //Flow low
    if (sAppWmInfor.sPulse[0].Flow_i32 < sAppWmInfor.sThreshold.FlowLow)  //   
    {
        if (AppWm_Check_Replay_Alarm( _ALARM_FLOW_LOW ) == TRUE)
            if (sAppWmVar.pPack_Alarm_Str != NULL)
                sAppWmVar.pPack_Alarm_Str ("u_app_wm: low flow!\r\n");
    }
    //Check Vbat threshold
    if (sAppWmInfor.sBattery.Level_u16 < sAppWmInfor.sThreshold.LowBatery) 
    {
        if (AppWm_Check_Replay_Alarm(_ALARM_VBAT_LOW) == TRUE)
            if (sAppWmVar.pPack_Alarm_Str != NULL)
                sAppWmVar.pPack_Alarm_Str ("u_app_wm: low battery!\r\n");
    } 
    
    //Check cac mark xem return 1 hay 0
    for (i = 0; i < _ALARM_END;i++)
        if (aMARK_ALARM[i] == 1)
            return 1;
    
    return 0;
}


/*
    Func: handler when check Alarm true
        + Neu truoc do da canh bao -> Sau MAX_ALARM_CONTINUOUS lan ms dc canh bao lai
        + Neu truoc do chua co canh bao: Mark alarm
*/
uint8_t AppWm_Check_Replay_Alarm (uint8_t Index)
{
    if (Check_Time_Out(aLandMarkAlarm_u32[Index], MAX_PERIOD_ALARM) == true)
    {
        aLandMarkAlarm_u32[Index] = RtCountSystick_u32;
        aMARK_ALARM[Index] = TRUE;
        
        return true;
    }
    
    return false;
}


uint8_t AppWm_Packet_TSVH (sData *pData)
{

    return true;
}



double AppWm_Linear_Interpolation(double InVal,
                                   double InMin, double InMax,
                                   double OutMin, double OutMax)
{
    if ( (InMin > InMax) || (OutMin > OutMax) )
        return -1000;
    
    //3 truong hop
    //TH1: k lap 
    if (InVal < InMin) 
        return -1000;
    //
    if (InVal > InMax)
        return 0xFFFF *1000;
    
    return ( OutMin + ((InVal - InMin) * (OutMax - OutMin) / (InMax - InMin)) );	
}


uint8_t AppWm_Packet_TSVH_Channel (sData *pData, uint8_t channel)
{
#ifdef PAYLOAD_HEX_FORMAT
    uint16_t FistPos = pData->Length_u16;
    uint64_t TempPulse = 0;
    
#ifdef USING_APP_COMM   
    uint16_t i = 0;
    uint8_t TempCrc = 0;
    uint8_t TempVal = 0;     
    uint8_t strChanel[] = {"0001"};
    
    //----------sTime--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_TIME_DEVICE, &sRTC, 6, 0xAA);
    
    //Channel ID
    strChanel[3] = 0x31 + channel;
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_SERI_SENSOR, &strChanel, 4, 0xAA);
    
    //----------chi so xung--------------------
#if (TSVH_OPERA_OR_FLASH == TSVH_MULTI)
    TempPulse = sAppWmInfor.sPulse[channel].Total_u64;
    
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PULSE, &TempPulse, 8, sAppWmInfor.sPulse[channel].Factor_u8);

    //----------luu luong --------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_DIG_FLOW, &sAppWmInfor.sPulse[channel].Flow_i32, 4, 0xFE);   
#else
    #ifdef LPTIM_ENCODER_MODE 
        TempPulse = sAppWmInfor.sPulse[channel].Number_u64 / 4;
    #else    
        TempPulse = sAppWmInfor.sPulse[channel].Number_u64;
    #endif
        
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PULSE, &TempPulse, 8, 0x00);

    //----------luu luong --------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_FLOW, &TempVal, 2, 0x00);   
#endif
    
    //----------Ap suat--------------------
//#ifdef PRESSURE_DECODE
//    if (sAppWmVar.aPRESSURE[channel].sLinearInter.Type_u8 == __AN_PRESS) {
//        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PRESSURE, &sAppWmVar.aPRESSURE[channel].Val_i32, 2, 0xFD);    
//    } else {
//        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_LEVEL_VAL_SENSOR, &sAppWmVar.aPRESSURE[channel].Val_i32, 2, 0xFD);    
//    } 
//#else
//    if (sAppWmVar.aPRESSURE[channel].sLinearInter.Type_u8 == __AN_PRESS) {
//        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_PRESSURE, &sAppWmVar.aPRESSURE[channel].Val_i32, 2, 0);    
//    } else {
//        SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_WM_LEVEL_VAL_SENSOR, &sAppWmVar.aPRESSURE[channel].Val_i32, 2, 0);    
//    }
//#endif
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL1, &sAppWmInfor.sBattery.mVol_u32, 2, 0xFD);
    
    //----------Cuong do song--------------------
#ifdef USING_APP_SIM
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_RSSI_1, &sSimCommInfor.RSSI_u8, 1, 0x00);
#endif
    
    //----------Dien ap Pin--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_DEV_VOL2, &sAppWmInfor.sVout.mVol_u32, 2, 0xFD);    
    
    //----------Tan suat--------------------
    SV_Protocol_Packet_Data(pData->Data_a8, &pData->Length_u16, OBIS_FREQ_SEND, &sModemInfor.sFrequence.UnitMin_u16, 2, 0x00);

    // caculator crc
    pData->Length_u16++;
	for (i = FistPos; i < (pData->Length_u16 - 1); i++)
		TempCrc ^= pData->Data_a8[i];

    pData->Data_a8[pData->Length_u16-1] = TempCrc;
#endif  // USING_APP_COMM  
    return  (pData->Length_u16- FistPos);
#else
    uint8_t aTIME_DATA[40] = {0};
    sData   sTimeData = {aTIME_DATA, 0};
    uint8_t aID_CHANEL[50] = {0};
    uint8_t LenIDChanel = 0;
    int32_t RealPress_u32 = 0;
    sData *pModemID = sAppWmVar.pGet_ID();
    uint32_t TempPulse = 0;
        
    UTIL_MEM_cpy(aID_CHANEL, pModemID->Data_a8, pModemID->Length_u16 );
    LenIDChanel = pModemID->Length_u16;
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = '0';
    aID_CHANEL[LenIDChanel++] = 0x31 + channel;

    Json_Add_Root(pData, NULL, 0, 0);
    Json_Add_Object_String(pData, (uint8_t *) "deviceID", strlen("deviceID"),
                                          aID_CHANEL , LenIDChanel, 1);
    
    String_Packet_Stime(&sTimeData, sRTC);
    Json_Add_Object_String(pData, (uint8_t *) "sentAt", strlen("sentAt"),
                                          sTimeData.Data_a8, sTimeData.Length_u16, 1);
    
    //Chuyen doi Pressure ra gia tri thuc te
    RealPress_u32 = AppWm_Linear_Interpolation(sAppWmVar.aPRESSURE[channel].Val_i32, 
                                               sAppWmVar.aPRESSURE[channel].sLinearInter.InMin_u16, 
                                               sAppWmVar.aPRESSURE[channel].sLinearInter.InMax_u16,
                                               sAppWmVar.aPRESSURE[channel].sLinearInter.OutMin_u16  * 1000, 
                                               sAppWmVar.aPRESSURE[channel].sLinearInter.OutMax_u16 * 1000 );
    
    Json_Add_Object_Number(pData, (uint8_t *) "presureVal", strlen("presureVal"), 
                                              RealPress_u32, 3, 1);

    #ifdef LPTIM_ENCODER_MODE 
        TempPulse = sAppWmInfor.sPulse[channel].Number_u64 / 4;
    #else    
        TempPulse = sAppWmInfor.sPulse[channel].Number_u64;
    #endif
     
    Json_Add_Object_Number(pData, (uint8_t *) "pulse", strlen("pulse"), 
                                               TempPulse, 0, 1);
        
    Json_Add_Object_Number(pData, (uint8_t *) "intbattery", strlen("intbattery"), 
                                           sAppWmInfor.sBattery.mVol_u32, 3, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "exbattery", strlen("exbattery"), 
                                           sVout.mVol_u32, 3, 1);       
    Json_Add_Object_Number(pData, (uint8_t *) "rssi", strlen("rssi"), 
                                           0 - sSimCommInfor.RSSI_u8, 0, 1);
    Json_Add_Object_Number(pData, (uint8_t *) "freqsend", strlen("freqsend"), 
                                           sModemInfor.sFrequence.UnitMin_u16, 0, 1);     
        
    return  (pData->Length_u16);
#endif
}


                    
                    
                    
uint8_t AppWm_Packet_Event (uint8_t *pData)
{
    uint8_t     length = 0;
    uint16_t	i = 0;
    uint8_t     TempCrc = 0;
    
    //----------------------- start send data below ---------------------
    pData[length++] = 0x01;   // sTime
    pData[length++] = 0x06;
    pData[length++] = sRTC.year;
    pData[length++] = sRTC.month;
    pData[length++] = sRTC.date;
    pData[length++] = sRTC.hour;
    pData[length++] = sRTC.min;
    pData[length++] = sRTC.sec;
    
    //Chăc chan vao day da co 1 alarm
    for (i = 0; i < _ALARM_END;i++)
    {
        if (aMARK_ALARM[i] == 1)
        {
            //Obis
            pData[length++] = i;
            //Length +Data + scale
            switch (i)
            {
                case _ALARM_FLOW_LOW:
                case _ALARM_FLOW_HIGH:
                    pData[length++] = 4;  //length
                    pData[length++] = (sAppWmInfor.sPulse[0].Flow_i32 >> 24) & 0xFF;  //
                    pData[length++] = (sAppWmInfor.sPulse[0].Flow_i32 >> 16) & 0xFF; //
                    pData[length++] = (sAppWmInfor.sPulse[0].Flow_i32 >> 8) & 0xFF;  //
                    pData[length++] = sAppWmInfor.sPulse[0].Flow_i32 & 0xFF;  // doi vi pulse/hour
                    pData[length++] = 0x00;
                    break;
                case _ALARM_PEAK_LOW:
                case _ALARM_PEAK_HIGH:
                    pData[length++] = 4;  //length
                    pData[length++] = (sAppWmInfor.sPulse[0].Quantity_i32 >> 24) & 0xFF;   //
                    pData[length++] = (sAppWmInfor.sPulse[0].Quantity_i32 >> 16) & 0xFF;   //
                    pData[length++] = (sAppWmInfor.sPulse[0].Quantity_i32 >> 8) & 0xFF;    //
                    pData[length++] = sAppWmInfor.sPulse[0].Quantity_i32 & 0xFF;           // doi vi pulse
                    pData[length++] = 0x00;
                    break;
                case _ALARM_VBAT_LOW:
                    pData[length++] = 2;  //length
                    pData[length++] = (sAppWmInfor.sBattery.Level_u16 >> 8) & 0xFF;  //
                    pData[length++] = sAppWmInfor.sBattery.Level_u16 & 0xFF;         // doi vi %
                    pData[length++] = 0x00;
                    break;
                case _ALARM_DIR_CHANGE:
                    pData[length++] = 1;     //length
                    pData[length++] = sAppWmInfor.sPulse[0].Direct_u8; 
                    pData[length++] = 0x00;
                    break;
                case _ALARM_DETECT_CUT:
                    pData[length++] = 1;     //length
                    pData[length++] = sAppWmVar.DTCVal_u8; 
                    pData[length++] = 0x00;
                    break;
                default:
                    break;   
            }
        }
        
        //Reset Alarm
        aMARK_ALARM[i] = 0;
    }  
            
    // caculator crc
    length++;
	for (i = 0; i < (length - 1); i++)
		TempCrc ^= pData[i];

    pData[length-1] = TempCrc;
    
    return length;
}


int32_t AppWm_Cacul_Quantity (uint32_t PulseCur, uint32_t PulseOld)
{
    int32_t Qual = 0;
    
    if (PulseCur >=  PulseOld)
	{
		if ((PulseCur - PulseOld) < 0xFFFFFFAF)   //khong the quay nguoc dc 0x50 xung trong 10p va khong the quay tien 0xFFAF
			Qual = (PulseCur - PulseOld);  //binh thuong
		else
			Qual = - (0xFFFFFFFF - PulseCur + PulseOld + 1); //quay nguoc
	} else
	{
		if ((PulseOld - PulseCur) > 0x50)   //khong the quay nguoc dc 0x50 xung trong 10p
			Qual = (0xFFFFFFFF - PulseOld + PulseCur + 1);  //binh thuong
        else
			Qual = - (PulseOld - PulseCur);  // quay ngược
	}
    
    return Qual;
}





/*
    Func: Log Data Event
        + Packet Event Data
        + Save to Flash or ExMem
*/

void AppWm_Log_Data_Event (void)
{
//    uint8_t     aMessData[64] = {0};
//    uint8_t     Length = 0;
//    
//    if (sRTC.year <= 20)
//        return;
//    
//    Length = AppWm_Packet_Event (&aMessData[0]);
//
//#ifdef USING_APP_MEM
//    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_EVENT_A, 0, 
//                      &aMessData[0], Length, sRecEvent.SizeRecord_u16);
//#else
//    #ifdef USING_APP_SIM
//        AppSim_Get_Data_From_Flash(DATA_TSVH_OPERA, aMessData, Length);
//    #endif
//#endif
}

/*
    Func: Log Data TSVH
        + Packet Data TSVH
        + Save to Flash or ExMem
*/

void AppWm_Log_Data_TSVH (void)
{
    uint8_t aMessData[512] = {0};
    sData sTempTSVH = {aMessData, 0};
    
    if (sRTC.year <= 20)
        return;
    
    AppWm_Packet_TSVH (&sTempTSVH);
    
//    AppMem_Write_Data(sAppMem.RecMemType_u8, _MEM_DATA_TSVH_A, 0, 
//                      sTempTSVH.Data_a8, sTempTSVH.Length_u16, sRecTSVH.SizeRecord_u16);
}


void AppWm_Init_Thresh_Measure (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t 	temp = 0xFF;
	uint8_t		Buff_temp[16] = {0};
      
	// Doc gia tri cau hinh threshold. Dung de check cac alarm
    temp = *(__IO uint8_t*) ADDR_THRESH_MEAS;    //2 byte
	if (temp != FLASH_BYTE_EMPTY)   //  Doc cau hinh so lan retry send cua 1 ban tin
    {
        OnchipFlashReadData(ADDR_THRESH_MEAS, &Buff_temp[0], 16);
        //
        sAppWmInfor.sThreshold.FlowHigh    = (Buff_temp[2] << 8) | Buff_temp[3];
        sAppWmInfor.sThreshold.FlowLow     = (Buff_temp[4] << 8) | Buff_temp[5];
        
        sAppWmInfor.sThreshold.PeakHigh    = (Buff_temp[6] << 8) | Buff_temp[7];
        sAppWmInfor.sThreshold.PeakLow     = (Buff_temp[8] << 8) | Buff_temp[9];
        
        sAppWmInfor.sThreshold.LowBatery   = Buff_temp[10];
        
        sAppWmInfor.sThreshold.LevelHigh   = (Buff_temp[11] << 8) | Buff_temp[12];
        sAppWmInfor.sThreshold.LevelLow    = (Buff_temp[13] << 8) | Buff_temp[14];
    } else
    {
        AppWm_Save_Thresh_Measure();
    }    
#endif
}


void AppWm_Save_Thresh_Measure (void)
{
#ifdef USING_INTERNAL_MEM
    uint8_t aTEMP_THRESH[24] = {0};
    
    aTEMP_THRESH[0] = BYTE_TEMP_FIRST;
    aTEMP_THRESH[1] = 13;

    aTEMP_THRESH[2] = (sAppWmInfor.sThreshold.FlowHigh >> 8) & 0xFF;
    aTEMP_THRESH[3] = sAppWmInfor.sThreshold.FlowHigh & 0xFF;
    
    aTEMP_THRESH[4] = (sAppWmInfor.sThreshold.FlowLow >> 8) & 0xFF;
    aTEMP_THRESH[5] = sAppWmInfor.sThreshold.FlowLow & 0xFF;
     
    aTEMP_THRESH[6] = (sAppWmInfor.sThreshold.PeakHigh >> 8) & 0xFF;  
    aTEMP_THRESH[7] = sAppWmInfor.sThreshold.PeakHigh & 0xFF;
    
    aTEMP_THRESH[8] = (sAppWmInfor.sThreshold.PeakLow >> 8) & 0xFF;
    aTEMP_THRESH[9] = sAppWmInfor.sThreshold.PeakLow & 0xFF;
    
    aTEMP_THRESH[10] = sAppWmInfor.sThreshold.LowBatery;
    
    aTEMP_THRESH[11] = (sAppWmInfor.sThreshold.LevelHigh >> 8) & 0xFF;
    aTEMP_THRESH[12] = sAppWmInfor.sThreshold.LevelHigh & 0xFF;
    
    aTEMP_THRESH[13] = (sAppWmInfor.sThreshold.LevelLow >> 8) & 0xFF;
    aTEMP_THRESH[14] = sAppWmInfor.sThreshold.LevelLow & 0xFF;

    OnchipFlashPageErase(ADDR_THRESH_MEAS);
    OnchipFlashWriteData(ADDR_THRESH_MEAS, &aTEMP_THRESH[0], 16);
#endif
}


void AppWm_Init_Infor (void)
{

}


uint8_t AppWm_Save_Infor (void)
{
    return true;
}



uint8_t AppWm_UnMark_Init_Pulse (void)
{
    return true;
}

void LED_Toggle (Led_TypeDef Led)
{
    HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}


void LED_On (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}



void LED_Off (Led_TypeDef Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}


/*
    Dat fun nay sau Init Pulse from flash
*/
void AppWm_Button_Default_Device (void)
{
   
}


void AppWm_Default_Pulse (void)
{
    if (sAppWmVar.pPack_Alarm_Str != NULL)
        sAppWmVar.pPack_Alarm_Str ("u_app_wm: default device!\r\n");  
        
    sAppWmVar.rDefault_u8 = true;
        
    for (uint8_t i = 0; i < MAX_CHANNEL; i++) {    
        sAppWmInfor.sPulse[i].Number_u64 = 0;
        sAppWmInfor.sPulse[i].Start_u64 = 0;   
        
        sAppWmInfor.sPulse[i].NumberInit_u64 = sAppWmInfor.sPulse[i].Number_u64;   
    }
    
    AppWm_Save_Infor();

#ifdef USING_APP_MEM
    sRecTSVH.IndexSend_u16 = 0;
    sRecTSVH.IndexSave_u16 = 0;
    sRecEvent.IndexSend_u16 = 0;
    sRecEvent.IndexSave_u16 = 0;
    sRecLog.IndexSend_u16 = 0;
    sRecLog.IndexSave_u16 = 0;
    sRecGPS.IndexSend_u16 = 0;
    sRecGPS.IndexSave_u16 = 0;
    
    AppMem_Save_Index_Rec();
#endif
}

/*Func Callback serial*/
#ifdef USING_AT_CONFIG

void AppWm_SER_Set_Default_Pulse(sData *strRecei, uint16_t Pos)
{
    sAppWmInfor.sPulse[0].Number_u64 = 0;
    sAppWmInfor.sPulse[1].Number_u64 = 0;
    sAppWmInfor.sPulse[2].Number_u64 = 0;
    sAppWmInfor.sPulse[3].Number_u64 = 0;
    
    sAppWmVar.pRespond_Str(PortConfig, "\r\nreset pulse ok!", 0);
}

void AppWm_SER_Set_Thresh_Flow(sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    int64_t    TemLowThresh = 0;
    int64_t    TemHighThresh = 0;
    
    TemLowThresh = UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    TemHighThresh =  UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    
    if ( (TemLowThresh != -1) && (TemHighThresh != -1) )
    {
        sAppWmInfor.sThreshold.FlowLow  = (uint16_t) TemLowThresh;
        sAppWmInfor.sThreshold.FlowHigh = (uint16_t) TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        sAppWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sAppWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Flow(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nflowthresh: %d,%d\r\n", sAppWmInfor.sThreshold.FlowLow, sAppWmInfor.sThreshold.FlowHigh);
    
    sAppWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Thresh_Quan(sData *str_Receiv, uint16_t Pos)
{
    uint16_t    Posfix = Pos;
    int64_t     TemLowThresh = 0;
    int64_t     TemHighThresh = 0;
    
    TemLowThresh = UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    TemHighThresh = UTIL_Get_Num_From_Str (str_Receiv, &Posfix);
    
    if ( (TemLowThresh != -1) && (TemHighThresh != -1) )
    {
        sAppWmInfor.sThreshold.PeakLow  = (uint16_t) TemLowThresh;
        sAppWmInfor.sThreshold.PeakHigh = (uint16_t) TemHighThresh;
        
        AppWm_Save_Thresh_Measure();
        
        sAppWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sAppWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Quan(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nQuanthresh: %d,%d\r\n", sAppWmInfor.sThreshold.PeakLow, sAppWmInfor.sThreshold.PeakHigh);
    
    sAppWmVar.pRespond_Str(PortConfig, aData, 0);
}


void AppWm_SER_Set_Thresh_Bat(sData *str_Receiv, uint16_t Pos)
{
	uint16_t    Posfix = Pos;
    int64_t    ValTemp = 0;
	  
    ValTemp = UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    
    if (ValTemp != -1) 
    {
        sAppWmInfor.sThreshold.LowBatery  = (uint16_t) ValTemp;
        AppWm_Save_Thresh_Measure();
        
        sAppWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
    }
    
    sAppWmVar.pRespond_Str(PortConfig, "\r\nOVER VALUE U16\r\n", 0);
}

void AppWm_SER_Get_Thresh_Bat(sData *str_Receiv, uint16_t Pos)
{
    char aData[32] = {0};
    
    sprintf(aData, "\r\nBatthresh: %d\r\n", sAppWmInfor.sThreshold.LowBatery);
    
    sAppWmVar.pRespond_Str(PortConfig, aData, 0);
}
    

void AppWm_SER_Set_Pulse(sData *str_Receiv, uint16_t Pos)
{
    uint16_t Posfix = Pos;

    uint8_t chann = (uint8_t ) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
    uint64_t value = (uint64_t ) UTIL_Get_Num_From_Str(str_Receiv, &Posfix);
	//
    if (chann <= MAX_CHANNEL) {
        if (chann == 0) {
        #ifdef LPTIM_ENCODER_MODE 
            sAppWmInfor.sPulse[0].Number_u64   = value * 4;
        #else   
            sAppWmInfor.sPulse[0].Number_u64   = value;
        #endif
            
        #ifdef LPTIM2_ENCODER_MODE 
            sAppWmInfor.sPulse[1].Number_u64  = value * 4;
        #else   
            sAppWmInfor.sPulse[1].Number_u64  = value;
        #endif
                
            sAppWmInfor.sPulse[2].Number_u64      = value;
            sAppWmInfor.sPulse[3].Number_u64      = value;
            
            for (uint8_t i = 0; i < MAX_CHANNEL; i++) {
                sAppWmInfor.sPulse[i].NumberInit_u64 = sAppWmInfor.sPulse[i].Number_u64;
            }
        } else {
            if ( (chann == 1) || (chann == 2) ) {
            #ifdef LPTIM_ENCODER_MODE 
                sAppWmInfor.sPulse[chann - 1].Number_u64 = value * 4;
            #else   
                sAppWmInfor.sPulse[chann - 1].Number_u64 = value;
            #endif
            } else {
                sAppWmInfor.sPulse[chann - 1].Number_u64 = value;
            }
            
            sAppWmInfor.sPulse[chann - 1].NumberInit_u64 = sAppWmInfor.sPulse[chann - 1].Number_u64;
        }
        
        AppWm_Save_Infor();
        sAppWmVar.pRespond_Str(PortConfig, "\r\nOK\r\n", 0);
        
        return;
    }

    sAppWmVar.pRespond_Str(PortConfig, "\r\nERROR\r\n", 0);
}

void AppWm_SER_Get_Pulse(sData *str_Receiv, uint16_t Pos)
{
    char aData[128] = {0};
    uint32_t aVAL_TEMP[4] = {0};
    
#ifdef LPTIM_ENCODER_MODE 
    aVAL_TEMP[0] = sAppWmInfor.sPulse[0].Number_u64 / 4;
#else   
    aVAL_TEMP[0] = sAppWmInfor.sPulse[0].Number_u64;
#endif
    
#ifdef LPTIM2_ENCODER_MODE 
    aVAL_TEMP[1] = sAppWmInfor.sPulse[1].Number_u64 / 4;
#else   
    aVAL_TEMP[1] = sAppWmInfor.sPulse[1].Number_u64;
#endif

    aVAL_TEMP[2] = sAppWmInfor.sPulse[2].Number_u64;
    aVAL_TEMP[3] = sAppWmInfor.sPulse[3].Number_u64;
    
    sprintf(aData, "\r\npulse: %d, %d, %d, %d\r\n", aVAL_TEMP[0], aVAL_TEMP[1],
                                                aVAL_TEMP[2], aVAL_TEMP[3]);
    
    sAppWmVar.pRespond_Str(PortConfig, aData, 0);
}
    
#endif



uint16_t AppWm_Packet_Setting (uint8_t *aData, uint8_t chann)
{
//    sData pData = {aData, 0};
//    uint8_t aSERI_SENSOR[4] = {"0001"};
//    aSERI_SENSOR[3] = chann + 0x31;
//    uint8_t aLIST_OBIS[32] = {0}, cObis = 0;
//
//    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_SERI_SENSOR, &aSERI_SENSOR, 4, 0xAA);
//    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_TYPE, &DcuType, 1, 0xAA);
//    aLIST_OBIS[cObis++] = OBIS_TIME_DEVICE;
//    aLIST_OBIS[cObis++] = OBIS_SERI_SENSOR;
//    aLIST_OBIS[cObis++] = OBIS_WM_PULSE; 
//#if (TSVH_OPERA_OR_FLASH == TSVH_MULTI)
//    aLIST_OBIS[cObis++] = OBIS_WM_DIG_FLOW;
//#else
//    aLIST_OBIS[cObis++] = OBIS_WM_FLOW; 
//#endif
//
//    aLIST_OBIS[cObis++] = OBIS_DEV_VOL1;
//#ifdef USING_APP_SIM
//    aLIST_OBIS[cObis++] = OBIS_RSSI_1;
//#endif
//    aLIST_OBIS[cObis++] = OBIS_DEV_VOL2;
//    aLIST_OBIS[cObis++] = OBIS_FREQ_SEND;
//    aLIST_OBIS[cObis++] = OBIS_CONTACT_STATUS;
//    
//    SV_Protocol_Packet_Data(pData.Data_a8, &pData.Length_u16, OBIS_LIST_OBIS_USE, &aLIST_OBIS, cObis, 0xAA);
//    
//    return pData.Length_u16;
    
    return false;
}




