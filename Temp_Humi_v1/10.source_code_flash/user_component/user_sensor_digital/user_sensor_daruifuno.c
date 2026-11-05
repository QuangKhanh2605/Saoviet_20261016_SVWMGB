

#include "user_sensor_daruifuno.h"
#include "math.h"

typedef enum
{
    _E_PH_S_SENSOR,
    _E_TEMP_S_SENSOR,
    _E_DO_S_SENSOR,
    _E_TSS_S_SENSOR,
    _E_COD_S_SENSOR,
    _E_NH4_S_SENSOR,
    
    _E_PH_VALUE,
    _E_TEMP_VALUE,

    _E_DO_VALUE,
    
    _E_TSS_VALUE,
    
    _E_COD_VALUE,
    
    _E_NH4_VALUE,

    _E_MODB_SS_END,
}eKindStateModbReg;

#define DARUIFUNO_REG_INIT_DATA \
/* eKind| Block | State | cmdFC | idDev | cmdLen | Addr | vFormat | vBeLe | vScale | subReg | vReturn | nConnect */ \
  {_E_PH_S_SENSOR,   2,  NULL,    0x03,      NULL,  1,        141,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  {_E_TEMP_S_SENSOR, 2,  NULL,    0x03,      NULL,  1,        141,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  {_E_DO_S_SENSOR,   2,  NULL,    0x03,      NULL,  1,        142,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  {_E_TSS_S_SENSOR,  2,  NULL,    0x03,      NULL,  1,        143,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  {_E_COD_S_SENSOR,  2,  NULL,    0x03,      NULL,  1,        144,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  {_E_NH4_S_SENSOR,  2,  NULL,    0x03,      NULL,  1,        145,    _ETYPE_U8,  _E_BE,   1,      NULL,     NULL,      NULL},\
  \
  {_E_PH_VALUE,     1,  NULL,    0x04,      NULL,  2,        4,    _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},\
  {_E_TEMP_VALUE,   1,  NULL,    0x04,      NULL,  2,        6,    _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},\
  \
  {_E_DO_VALUE,     1,  NULL,    0x04,      NULL,  2,       12,   _ETYPE_F,  _E_WS,   1,         NULL,     NULL,      NULL},\
  \
  {_E_TSS_VALUE,    1,  NULL,    0x04,      NULL,  2,        28,   _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},\
  \
  {_E_COD_VALUE,    1,  NULL,    0x04,      NULL,  2,        36,   _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},\
      \
  {_E_NH4_VALUE,    1,  NULL,    0x04,      NULL,  2,        20,   _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},\
  \
  {_E_MODB_SS_END,   0, NULL, 0, NULL, 0, 0,      0,        0,     0, NULL, NULL, NULL}
  
Struct_RegSensor            sRegSS_Daruifuno_PortRTU[] =
{
    DARUIFUNO_REG_INIT_DATA
};

void       RS485_SS_Daruifuno_Init(Struct_RegSensor sReg[], uint8_t Port)
{
    Config_RegSen_Read(sReg, _E_PH_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                       &sMeasureMain[Port][_SS_PH].stateSensor, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_TEMP_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].stateSensor, &sMeasureMain[Port][_SS_TEMP].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].stateSensor, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_TSS_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].stateSensor, &sMeasureMain[Port][_SS_TSS].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_COD_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].stateSensor, &sMeasureMain[Port][_SS_COD].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_NH4_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].stateSensor, &sMeasureMain[Port][_SS_NH4].nConnect_u8);
    //----------------------------Ph------------------------------
    Config_RegSen_Read(sReg, _E_PH_VALUE, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                    &sMeasureMain[Port][_SS_PH].Value_f, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    //----------------------------COD------------------------------
    
    Config_RegSen_Read(sReg, _E_COD_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].Value_f, &sMeasureMain[Port][_SS_COD].nConnect_u8); 
    //----------------------------TSS-----------------------------
    
    Config_RegSen_Read(sReg, _E_TSS_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].Value_f, &sMeasureMain[Port][_SS_TSS].nConnect_u8); 
    //----------------------------NH4-----------------------------
    
    Config_RegSen_Read(sReg, _E_NH4_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].Value_f, &sMeasureMain[Port][_SS_NH4].nConnect_u8); 
    //----------------------------DO------------------------------
    
    Config_RegSen_Read(sReg, _E_DO_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].Value_f, &sMeasureMain[Port][_SS_DO].nConnect_u8);   
    //---------------------------TEMP----------------------------- 
    
    Config_RegSen_Read(sReg, _E_TEMP_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].Value_f, &sMeasureMain[Port][_SS_TEMP].nConnect_u8); 
    
    if(sMeasureMain[Port][_SS_PH].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_PH_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_PH].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_PH_VALUE].cmdAddr = (sMeasureMain[Port][_SS_PH].sUser - 1) * 10 + 0;
    }
    
    if(sMeasureMain[Port][_SS_COD].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_COD_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_COD].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_COD_VALUE].cmdAddr = (sMeasureMain[Port][_SS_COD].sUser - 1) * 10 + 0;
    }
    
    if(sMeasureMain[Port][_SS_TSS].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_TSS_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_TSS].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_TSS_VALUE].cmdAddr = (sMeasureMain[Port][_SS_TSS].sUser - 1) * 10 + 2;
    }
    
    if(sMeasureMain[Port][_SS_DO].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_DO_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_DO].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_DO_VALUE].cmdAddr = (sMeasureMain[Port][_SS_DO].sUser - 1) * 10 + 0;
    }
    
    if(sMeasureMain[Port][_SS_NH4].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_NH4_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_NH4].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_NH4_VALUE].cmdAddr = (sMeasureMain[Port][_SS_NH4].sUser - 1) * 10 + 6;
    }
    
    if(sMeasureMain[Port][_SS_TEMP].sUser != 0)
    {
        sRegSS_Daruifuno_PortRTU[_E_TEMP_S_SENSOR].cmdAddr = (sMeasureMain[Port][_SS_TEMP].sUser - 1) + 141;
        sRegSS_Daruifuno_PortRTU[_E_TEMP_VALUE].cmdAddr = (sMeasureMain[Port][_SS_TEMP].sUser - 1) * 10 + 2;
    }
}

void    Digital_SS_Daruifuno_CallCycle(void)
{
    for(uint8_t i = 0; i<_END_SENSOR; i++)
    {
        sMeasureMain[0][i].stateValue = 0x01;
    }
}

