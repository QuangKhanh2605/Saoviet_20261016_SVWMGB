

#include "user_sensor_xylem.h"
#include "math.h"

typedef enum
{
    _E_PH_S_SENSOR,
    _E_TEMP_S_SENSOR,
    _E_PH_S_VALUE,
    _E_TEMP_S_VALUE,
    _E_PH_VALUE,
    _E_TEMP_VALUE,

    _E_DO_S_SENSOR,
    _E_DO_S_VALUE,
    _E_DO_VALUE,
    
    _E_NH4_S_SENSOR,
    _E_NH4_S_VALUE,
    _E_NH4_VALUE,
    
    _E_TSS_S_SENSOR,
    _E_TSS_S_VALUE,
    _E_TSS_VALUE,
    
    _E_COD_S_SENSOR,
    _E_COD_S_VALUE,
    _E_COD_VALUE,

    _E_MODB_SS_END,
}eKindStateModbReg;

#define XYLEM_REG_INIT_DATA \
/* eKind| Block | State | cmdFC | idDev | cmdLen | Addr | vFormat | vBeLe | vScale | subReg | vReturn | nConnect */ \
  {_E_PH_S_SENSOR,  1,  NULL,    0x04,      NULL,  1,        0,    _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_TEMP_S_SENSOR,1,  NULL,    0x04,      NULL,  1,        0,    _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_PH_S_VALUE,   1,  NULL,    0x04,      NULL,  1,        3,    _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_TEMP_S_VALUE, 1,  NULL,    0x04,      NULL,  1,        3,    _ETYPE_U8, _E_BS,   1,        NULL,     NULL,      NULL},\
  {_E_PH_VALUE,     1,  NULL,    0x04,      NULL,  2,        4,    _ETYPE_F,  _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_TEMP_VALUE,   1,  NULL,    0x04,      NULL,  2,        6,    _ETYPE_F,  _E_BE,   1,        NULL,     NULL,      NULL},\
  \
  {_E_DO_S_SENSOR,  1,  NULL,    0x04,      NULL,   1,       8,    _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_DO_S_VALUE,   1,  NULL,    0x04,      NULL,   1,       11,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_DO_VALUE,     1,  NULL,    0x04,      NULL,   2,       12,   _ETYPE_F,  _E_BE,   1,        NULL,     NULL,      NULL},\
  \
  {_E_NH4_S_SENSOR, 1,  NULL,    0x04,      NULL,  1,        16,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_NH4_S_VALUE,  1,  NULL,    0x04,      NULL,  1,        19,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_NH4_VALUE,    1,  NULL,    0x04,      NULL,  2,        20,   _ETYPE_F,  _E_BE,   1,        NULL,     NULL,      NULL},\
  \
  {_E_TSS_S_SENSOR, 1,  NULL,    0x04,      NULL,  1,        24,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_TSS_S_VALUE,  1,  NULL,    0x04,      NULL,  1,        27,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_TSS_VALUE,    1,  NULL,    0x04,      NULL,  2,        28,   _ETYPE_F,  _E_BE,   1000,     NULL,     NULL,      NULL},\
  \
  {_E_COD_S_SENSOR, 1,  NULL,    0x04,      NULL,  1,        32,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_COD_S_VALUE,  1,  NULL,    0x04,      NULL,  1,        35,   _ETYPE_U8, _E_BE,   1,        NULL,     NULL,      NULL},\
  {_E_COD_VALUE,    1,  NULL,    0x04,      NULL,  2,        36,   _ETYPE_F,  _E_BE,   1,        NULL,     NULL,      NULL},\
  \
  {_E_MODB_SS_END,   0, NULL, 0, NULL, 0, 0,      0,        0,     0, NULL, NULL, NULL}
  
Struct_RegSensor            sRegSS_Xylem_PortTCP[] =
{
    XYLEM_REG_INIT_DATA
};

void       RS485_SS_XyLem_Init_TCP(Struct_RegSensor sReg[], uint8_t Port)
{
    //----------------------------Ph------------------------------
    Config_RegSen_Read(sReg, _E_PH_VALUE, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                    &sMeasureMain[Port][_SS_PH].Value_f, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_PH_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                       &sMeasureMain[Port][_SS_PH].stateSensor, &sMeasureMain[Port][_SS_PH].nConnect_u8);

    Config_RegSen_Read(sReg, _E_PH_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                       &sMeasureMain[Port][_SS_PH].stateValue, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    //----------------------------COD------------------------------
    Config_RegSen_Read(sReg, _E_COD_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].Value_f, &sMeasureMain[Port][_SS_COD].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_COD_S_SENSOR,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].stateSensor, &sMeasureMain[Port][_SS_COD].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_COD_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].stateValue, &sMeasureMain[Port][_SS_COD].nConnect_u8); 
    //----------------------------TSS-----------------------------
    Config_RegSen_Read(sReg, _E_TSS_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].Value_f, &sMeasureMain[Port][_SS_TSS].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TSS_S_SENSOR,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].stateSensor, &sMeasureMain[Port][_SS_TSS].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TSS_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].stateValue, &sMeasureMain[Port][_SS_TSS].nConnect_u8); 
    //----------------------------NH4-----------------------------
    Config_RegSen_Read(sReg, _E_NH4_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].Value_f, &sMeasureMain[Port][_SS_NH4].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_NH4_S_SENSOR,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].stateSensor, &sMeasureMain[Port][_SS_NH4].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_NH4_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].stateValue, &sMeasureMain[Port][_SS_NH4].nConnect_u8); 
    //----------------------------DO------------------------------
    Config_RegSen_Read(sReg, _E_DO_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].Value_f, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_S_SENSOR,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].stateSensor, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].stateValue, &sMeasureMain[Port][_SS_DO].nConnect_u8);   
    //---------------------------TEMP-----------------------------
    Config_RegSen_Read(sReg, _E_TEMP_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].Value_f, &sMeasureMain[Port][_SS_TEMP].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TEMP_S_SENSOR,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].stateSensor, &sMeasureMain[Port][_SS_TEMP].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TEMP_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].stateValue, &sMeasureMain[Port][_SS_TEMP].nConnect_u8); 
}

void    Digital_SS_XyLem_CallCycle(void)
{

}

