



#ifndef USER_APP_WM_H_
#define USER_APP_WM_H_


#include "user_util.h"
#include "event_driven.h"

#include "user_adc.h"
#include "user_lptim.h"
#include "user_rs485.h"
#include "user_wm_digital.h"

#define USING_APP_WM


#define BUTTON_DEFAULT_DEV

/*============= Define =====================*/
#define VDD_OUT_MAX             12000     
#define VDD_OUT_MIN             9000

#define VDD_BAT                 3600     
#define VDD_MIN                 2800

#define V12_IN_ON               //HAL_GPIO_WritePin(ON_OFF_12V_GPIO_Port, ON_OFF_12V_Pin, GPIO_PIN_SET)
#define V12_IN_OFF              //HAL_GPIO_WritePin(ON_OFF_12V_GPIO_Port, ON_OFF_12V_Pin, GPIO_PIN_RESET)

#define V_PIN_ON                HAL_GPIO_WritePin(BOOST_ON_OFF_GPIO_Port, BOOST_ON_OFF_Pin, GPIO_PIN_SET)
#define V_PIN_OFF               HAL_GPIO_WritePin(BOOST_ON_OFF_GPIO_Port, BOOST_ON_OFF_Pin, GPIO_PIN_RESET)

#define BLE_IRQ_READ            HAL_GPIO_ReadPin(BLE_WAKEUP_GPIO_Port, BLE_WAKEUP_Pin)  

#define SLAVE_ID_DEFAULT        0x01

#define MAX_POINT_CALIB         14
#define DEFAULT_POINT_CALIB     11
#define REAL                    0
#define SAMP                    1

#define MAX_PORT_INPUT          4
#define MAX_CHANNEL             1
 
#define PRESSURE_RESISTOR       100

#define PRESSURE_HIGH_CURR      20          //mA     
#define PRESSURE_LOW_CURR       4           //mA

#define PRESSURE_DIVIDE_VOL     5           //he so qua tro treo

#define PRESSURE_HIGH_VOL2      10000       //mV  
#define PRESSURE_LOW_VOL2       0

#define PRESSURE_HIGH_VOL3      4500     
#define PRESSURE_LOW_VOL3       500

#define PRESSURE_HIGH_BAR       10     
#define PRESSURE_LOW_BAR        0  

#define MAX_BUTTON_DEF_DEV      10000000    //clock  20M 
#define BUTTON_DEFAULT          ( (HAL_GPIO_ReadPin(BUTTON_1_GPIO_Port, BUTTON_1_Pin) == GPIO_PIN_SET) ? 0U : 1U )

#define MAX_PERIOD_ALARM        300000

#define MAX_TIME_MODBUS         120000
#define PERIOD_READ_MODBUS      10000

#define MEASURE_PIN_ON          HAL_GPIO_WritePin(PIN_ON_OFF_GPIO_Port, PIN_ON_OFF_Pin, GPIO_PIN_SET)
#define MEASURE_PIN_OFF         HAL_GPIO_WritePin(PIN_ON_OFF_GPIO_Port, PIN_ON_OFF_Pin, GPIO_PIN_RESET)
 
#define MAX_BLOCK               6
#define MAX_DISCONN_RS485       1

#define PULSE_ON                HAL_GPIO_WritePin(PULSE_ON_OFF_GPIO_Port, PULSE_ON_OFF_Pin, GPIO_PIN_SET)
#define PULSE_OFF               HAL_GPIO_WritePin(PULSE_ON_OFF_GPIO_Port, PULSE_ON_OFF_Pin, GPIO_PIN_RESET)  

/*================ var struct =================*/
typedef enum
{
    _EVENT_ENTRY_WM,            //0
    
    _EVENT_LOG_TSVH,            //1
    _EVENT_EXT_IRQ,             //2
    _EVENT_DIR_CHANGE,          //3
    
    _EVENT_MEAS_PRESSURE,       //4
    _EVENT_SCAN_ALARM,          //5
    _EVENT_CONTROL_LED1,        //6
    
    _EVENT_RS485_RECV,          //7
    _EVENT_RS485_MODBUS,        //8

    _EVENT_PRO_BLE_UART,        //9    
    _EVENT_IRQ_BLE,             //10
    
    _EVENT_READ_I2C,
    
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


typedef struct
{
    ST_TIME_FORMAT  sTime;
    uint32_t        Pulse_u32; 
}sLastRecordInfor;


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
    
    _ALARM_LOSS_DEVICE,
    _ALARM_MAGN_DETECT,
    _ALARM_HARD_RESET,
    
	_ALARM_END,
}Struct_Type_Alarm;


typedef struct
{
    uint8_t     status_u8;
    
    uint16_t    tStart_u16;
    uint16_t    tEnd_u16;
    
    float       ValMin_f;
    float       ValMax_f;
}sThreshold;


typedef struct
{
    sThreshold     Quan[MAX_BLOCK];
    sThreshold     Flow[MAX_BLOCK];
    sThreshold     Press[MAX_BLOCK];
    sThreshold     Level[MAX_BLOCK];
     
    uint16_t       LowBatery;
}sThresholdVariable;


/* struct value linear interpolation pressure: mV-mV input*/
typedef struct
{
    uint16_t InMin_u16;      //Dau vao
    uint16_t InMax_u16;
    uint8_t  InUnit_u8;
    uint16_t OutMin_u16;     //Dau ra
    uint16_t OutMax_u16; 
    uint8_t  OutUnit_u8;
    uint16_t Factor_u16;      //he so nhan
    uint8_t  FactorDec_u8;    //he so thap phan cua factor     
    uint8_t  Type_u8;         //0: do ap suat;  1: do muc nuoc
}sPressureLinearInter;


typedef struct
{
    uint8_t                 Mode_u8;
    uint32_t                VolAdc_u32;         //gia tri ADC do duoc: theo doi
    int32_t                 mVol_i32;           //gia tri thuc te mV do duoc
    int32_t                 Val_i32;            //gia tri dau ra
    sPressureLinearInter    sLinearInter;       //gia tri noi suy tuyen tinh
    int16_t                 Calib_i16;          //gia tri bu op-am
}sAppWmPressureInfor;

typedef struct
{
    uint8_t StaQuanHi_u8;
    uint8_t StaQuanLow_u8;
    uint8_t StaFlowHi_u8;
    uint8_t StaFlowLow_u8;
    uint8_t StaBatLow_u8;
    uint8_t StaDir_u8;
    uint8_t StaLostDev_u8;
    uint8_t StaMagne_u8;
    uint8_t StaHardRs_u8;
    
    uint8_t aMARK_ALARM[_ALARM_END];
}sStatusAlarm;


typedef struct
{
    uint8_t             IrqPowUp_u8;
    uint16_t            aINPUT_VAL[MAX_PORT_INPUT];
    sAppWmPressureInfor aPRESSURE[MAX_CHANNEL + MAX_SLAVE_MODBUS];
    uint8_t             nChannel_u8;
    uint8_t             rDefault_u8;
    uint8_t             rIntan_u8;
    uint8_t             rPressCalib_u8;
    uint8_t             *pModePower_u8;  
    uint8_t             rTestsPulse_u8;  
    uint8_t             DTCVal_u8;
    uint8_t             *pHardware_u8;
    
    void    (*pRespond) (uint8_t portNo, uint8_t *data, uint16_t length, uint8_t ack);
    void    (*pRespond_Str) (uint8_t portNo, const char *str, uint8_t ack);
    
    void    (*pRq_Instant) (void);
    void    (*pPack_Alarm_Str) (const char *str);
    void    (*pCtrl_Sim) (uint8_t allow);    
    char*  (*pGet_ID) (void); 
    
    sStatusAlarm    sAlarm[MAX_CHANNEL];
    uint8_t         iCaculOK[MAX_CHANNEL];
    uint8_t         aOUTPUT_STATE[10];
}sAppWmVariable;



extern Struct_Battery_Status    sBattery;
extern Struct_Battery_Status    sVout;
extern Struct_Battery_Status    sPressure;

extern sThresholdVariable       sMeterThreshold[MAX_CHANNEL];
extern Struct_Pulse			    sPulse[MAX_CHANNEL];
extern sLastRecordInfor         sLastPackInfor;    

//Variable
extern uint32_t *pMarkSoftResetMCU;
extern uint32_t *pPulseLPTIM;

extern sEvent_struct        sEventAppWM[];
extern sAppWmVariable       sWmVar;

extern char AnalogType[2][10];
extern char aUnitWm[5][10]; 
/*================ Function =================*/

//Function handler
int32_t     AppWm_Pressure_ADC_Vol(uint32_t Channel);
uint32_t    AppWm_Convert_mV_To_Press_Unit (uint32_t mVol, uint8_t mode);

void        AppWm_Plus_5Pulse (Struct_Pulse *sPulse);
uint8_t     AppWm_Check_Change_Mode (uint8_t *Mode, uint32_t VoutmV);
void        AppWm_Check_Mode_Power (void);

uint8_t     AppWm_Packet_TSVH_Channel (sData *pData, uint8_t channel);
uint8_t     AppWm_Packet_Event (uint8_t *pData, uint8_t chann);
int32_t     AppWm_Cacul_Quantity (uint32_t PulseCur, uint32_t PulseOld);

void        AppWm_Init (void);
uint8_t     AppWm_Task(void);

void        AppWm_Init_Pulse_Infor (void);
uint8_t     AppWm_Save_Pulse (void);

void        AppWm_Init_Thresh_Measure (void);
void        AppWm_Save_Thresh_Measure (void);

void        AppWm_Init_Level_Calib (void);
void        AppWm_Save_Level_Calib (void);

uint8_t     AppWm_Scan_Alarm (void);

uint16_t    AppWm_Get_VBat(void);
uint16_t    AppWm_Get_VOut(void);

uint8_t     AppWm_Get_Mode_Led (void);
void        AppWm_Log_Data_Event (uint8_t chann);

uint8_t     AppWm_Packet_Mod_Channel (sData *pData, uint8_t chann);

uint8_t     AppWm_Analys_Vout (void);
void        AppWm_Off_Energy_Consumption (void);

void        LED_Toggle (Led_TypeDef Led);
void        LED_On (Led_TypeDef Led);
void        LED_Off (Led_TypeDef Led);

uint8_t     AppWm_UnMark_Init_Pulse (void);
void        AppWm_Button_Default_Device (void);

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
void    AppWm_SER_Set_Num_Channel(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Num_Channel(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Modbus_Infor(sData *pData, uint16_t Pos);
void    AppWm_SER_Get_Modbus_Infor(sData *str_Receiv, uint16_t Pos);

void    AppWm_SER_Set_Pressure_Infor(sData *str_Receiv, uint16_t Pos);
uint8_t AppWm_Set_Pressure_Infor (uint8_t channel, sPressureLinearInter press_sett);
void    AppWm_SER_Set_Pressure_Calib(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Pressure_Calib(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Ctrl_Output (sData *str_Receiv, uint16_t Pos);

void    AppWm_SER_Get_Pressure_Infor(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Pressure_Val(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Level_Infor(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Level_Infor(sData *str_Receiv, uint16_t Pos);

void    AppWm_Init_WM_Dig_Infor (void);
void    AppWm_Save_WM_Dig_Infor (void);

void    AppWm_Init_Set_output (void);
void    AppWm_Save_Set_Output (void);

void    AppWm_Init_Press_Infor (void);
void    AppWm_Save_Press_Infor (void);

void    AppWm_Init_Default_Pressure (void);

double  AppWm_Linear_Interpolation(double InVal,
                                   double InMin, double InMax,
                                   double OutMin, double OutMax);

uint8_t AppWm_Meas_Pressure_Process (uint32_t *delaynext);

uint16_t AppWm_Packet_Setting (uint8_t *aData, uint8_t chann);

void    AppWm_Cacul_Param (void);
uint8_t AppWm_Cacul_Flow (uint8_t chann);
void    AppWm_Default_Pulse (void);

uint8_t AppWm_Packet_Intan (sData *pData);
void    AppWm_Packet_TSVH (void);
void    AppWm_Packet_Modbus (void);
void    AppWm_Get_Pulse (void);

void    AppWm_SER_Set_Pulse_Factor(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Pulse_Factor(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Set_Pulse_Start(sData *str_Receiv, uint16_t Pos);
void    AppWm_SER_Get_Pulse_Start(sData *str_Receiv, uint16_t Pos);

uint8_t AppWm_Scan_Alarm_Channel (uint8_t chann);
uint8_t AppWm_Over_5Pulse (void);

void    AppWm_RS485_Rx_Done (void);
uint16_t AppWm_DIG_Packet_Setting (uint8_t *aData, uint8_t chann);
void    AppWm_Set_Output (void);


#endif
