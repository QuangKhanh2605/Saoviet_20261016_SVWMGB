



#ifndef USER_APP_WM_H_
#define USER_APP_WM_H_


#include "user_util.h"
#include "event_driven.h"

#include "user_adc.h"
#include "user_lptim.h"


#define USING_APP_WM


#define BUTTON_DEFAULT_DEV

/*============= Define =====================*/
#define VDD_OUT_MAX             3000     
#define VDD_OUT_MIN             1800

#define VDD_BAT                 3600     
#define VDD_MIN                 2800

#define MAX_CHANNEL             6

#define TIME_CHECK_ALARM        5000
#define MAX_PERIOD_ALARM        300000

/*================ var struct =================*/
typedef enum
{
    _EVENT_ENTRY_WM,            //0
    
    _EVENT_LOG_TSVH,            //1
    _EVENT_EXT_IRQ,             //2

    _EVENT_SCAN_ALARM,          //3
    _EVENT_CONTROL_LED1,        //4
    
	_EVENT_END_WM,
}eKindEventWm;

typedef enum
{
    _LED_STATUS,
    _LED_GPS,
    _LED_TEMH,
} Led_TypeDef;

typedef enum
{
    _UNIT_MILIVOL,
    _UNIT_MILIAM,
    _UNIT_VOL,
    _UNIT_MET,
    _UNIT_BAR,
}sUnitWM;


typedef enum
{
    _LED_MODE_DEFAULT_DEV,
    _LED_MODE_ONLINE_INIT,
    _LED_MODE_CONNECT_SERVER,
    _LED_MODE_UPDATE_FW,
    _LED_MODE_POWER_SAVE,
    _LED_MODE_TEST_PULSE,
}sKindModeLed;

typedef enum
{
	_ALARM_FLOW_LOW = 2,
	_ALARM_FLOW_HIGH,
	_ALARM_PEAK_LOW,
	_ALARM_PEAK_HIGH,
    _ALARM_VBAT_LOW,
    _ALARM_DIR_CHANGE,
    _ALARM_DETECT_CUT,
    
	_ALARM_END,
}Struct_Type_Alarm;


typedef struct
{
    uint16_t    FlowHigh;
    uint16_t    FlowLow;
    
    uint16_t    PeakHigh;
    uint16_t    PeakLow;
    
    uint8_t     LowBatery;
    
    int16_t     LevelHigh;
    int16_t     LevelLow;
    
    uint16_t    ExBatVol;
}sThresholdInformation;

typedef struct
{
    sThresholdInformation   sThreshold;  
    Struct_Pulse		    sPulse[MAX_CHANNEL];
    Struct_Battery_Status   sBattery;
    Struct_Battery_Status   sVout;
} sAppWmInformation;


typedef struct
{ 
    uint8_t nChannel_u8;
    uint8_t rDefault_u8;
    uint8_t rIntan_u8;
    uint8_t *pModePower_u8;  
    uint8_t rTestsPulse_u8;  
    uint8_t DTCVal_u8;
    uint8_t *pHardware_u8;
    
    void    (*pRespond) (uint8_t portNo, uint8_t *data, uint16_t length, uint8_t ack);
    void    (*pRespond_Str) (uint8_t portNo, const char *str, uint8_t ack);
    
    void    (*pRq_Instant) (void);
    void    (*pPack_Alarm) (uint8_t *pData, uint16_t length);
    void    (*pPack_Alarm_Str) (const char *str);
    void    (*pCtrl_Sim) (uint8_t allow);    
    sData*  (*pGet_ID) (void); 
}sAppWmVariable;

extern sEvent_struct  sEventAppWM[];
extern sAppWmVariable sAppWmVar;
extern sAppWmInformation    sAppWmInfor;
/*================ Function =================*/

//Function handler
void    AppWm_Plus_5Pulse (Struct_Pulse *sPulse);

uint8_t AppWm_Packet_TSVH (sData *pData);
uint8_t AppWm_Packet_Event (uint8_t *pData);
int32_t AppWm_Cacul_Quantity (uint32_t PulseCur, uint32_t PulseOld);

void    AppWm_Init (void);
uint8_t AppWm_Task(void);

void    AppWm_Init_Infor (void);
uint8_t AppWm_Save_Infor (void);

uint8_t     AppWm_Scan_Alarm (void);
uint8_t     AppWm_Check_Replay_Alarm (uint8_t Index);

void        AppWm_Log_Data_TSVH (void);

uint16_t AppWm_Get_VBat(void);
uint16_t AppWm_Get_VOut(void);

uint8_t     AppWm_Get_Mode_Led (void);
void        AppWm_Log_Data_Event (void);

void        AppWm_Off_Energy_Consumption (void);

void        LED_Toggle (Led_TypeDef Led);
void        LED_On (Led_TypeDef Led);
void        LED_Off (Led_TypeDef Led);

void AppWm_Save_Thresh_Measure (void);
void AppWm_Init_Thresh_Measure (void);

void AppWm_Cacal_Param (void);

/*Func callback serial*/
void    AppWm_SER_Set_Default_Pulse(sData *strRecei, uint16_t Pos);
void    AppWm_SER_Set_Thresh_Flow(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Thresh_Flow(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Thresh_Quan(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Thresh_Quan(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Thresh_Bat(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Thresh_Bat(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Pulse(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Pulse(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Level_Calib(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Level_Calib(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Level_Thresh(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Level_Thresh(sData *str_Receiv, uint16_t Pos);

void    AppWm_Default_Pulse (void);

#endif
