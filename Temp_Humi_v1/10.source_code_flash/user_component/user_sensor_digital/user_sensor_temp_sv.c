

#include "user_sensor_temp_sv.h"
#include "math.h"

typedef enum
{
    _E_TEMP_S_VALUE,
    _E_HUMI_S_VALUE,
    
    _E_MODB_SS_END,
}eKindStateModbReg;

#define TEMPSV_REG_INIT_DATA \
/* eKind| Block | State | cmdFC | idDev | cmdLen | Addr | vFormat | vBeLe | vScale | subReg | vReturn | nConnect */ \
  {_E_TEMP_S_VALUE,1,  NULL,    0x03,      NULL,  1,        0x0002,    _ETYPE_I32, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_HUMI_S_VALUE,1,  NULL,    0x03,      NULL,  1,        0x0005,    _ETYPE_I32, _E_BE,   1,        NULL,     NULL,      NULL},\
  \
  {_E_MODB_SS_END,   0, NULL, 0, NULL, 0, 0,      0,        0,     0, NULL, NULL, NULL}
  
Struct_RegSensor            sRegSS_TempSV_PortRTU[] =
{
    TEMPSV_REG_INIT_DATA
};

void       RS485_SS_TempSV_Init_RTU(Struct_RegSensor sReg[], uint8_t Port)
{
    //---------------------------TEMP-----------------------------
    Config_RegSen_Read(sReg, _E_TEMP_S_VALUE,  &sMeasureMain[Port][_SS_HUMI].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].Value_i32 , &sMeasureMain[Port][_SS_TEMP].nConnect_u8);    
    //----------------------------Ph------------------------------
    Config_RegSen_Read(sReg, _E_HUMI_S_VALUE, &sMeasureMain[Port][_SS_HUMI].ID_Modbus, &sMeasureMain[Port][_SS_HUMI].sUser, 
                                    &sMeasureMain[Port][_SS_HUMI].Value_i32, &sMeasureMain[Port][_SS_HUMI].nConnect_u8);
}

void    Digital_SS_TempSV_CallCycle(void)
{
    sMeasureMain[0][_SS_TEMP].Value_f = (float)(sMeasureMain[0][_SS_TEMP].Value_i32)/100;
    sMeasureMain[0][_SS_HUMI].Value_f = (float)(sMeasureMain[0][_SS_HUMI].Value_i32)/100;
    for(uint8_t j = 0; j<MAX_CHANNEL_SS; j++)
    {
        for(uint8_t i = 0; i<_END_SENSOR; i++)
        {
            sMeasureMain[j][i].stateValue = 0x01;
            sMeasureMain[j][i].stateSensor = 0x02;
        }
    }
}

