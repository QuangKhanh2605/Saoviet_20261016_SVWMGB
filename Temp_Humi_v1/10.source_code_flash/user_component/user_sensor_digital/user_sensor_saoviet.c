

#include "user_sensor_saoviet.h"
#include "math.h"

typedef enum
{
    _E_PH_VALUE=0,
    _E_PH_S_SENSOR,
    _E_PH_S_VALUE,
    
    _E_CLO_SEND_PH,
    _E_CLO_VALUE,
    _E_CLO_S_SENSOR,
    _E_CLO_S_VALUE,
    
    _E_EC_VALUE,
    _E_EC_S_SENSOR,
    _E_EC_S_VALUE,
    
    _E_TURB_VALUE,
    _E_TURB_S_SENSOR,
    _E_TURB_S_VALUE,
    
    _E_COD_VALUE,
    _E_COD_S_SENSOR,
    _E_COD_S_VALUE,
    
    _E_TSS_VALUE,
    _E_TSS_S_SENSOR,
    _E_TSS_S_VALUE,
    
    _E_NH4_VALUE,
    _E_NH4_S_SENSOR,
    _E_NH4_S_VALUE,
    
    _E_DO_SALT,
    _E_DO_VALUE,
    _E_DO_S_SENSOR,
    _E_DO_S_VALUE,
    
    _E_SALT_VALUE,
    _E_SALT_S_SENSOR,
    _E_SALT_S_VALUE,
    
    _E_TDS_VALUE,
    _E_TDS_S_SENSOR,
    _E_TDS_S_VALUE,
    
    _E_NO3_VALUE,
    _E_NO3_S_SENSOR,
    _E_NO3_S_VALUE,
    
    _E_NH4_N_VALUE,
    _E_NH4_N_S_SENSOR,
    _E_NH4_N_S_VALUE,
    
    _E_NO3_N_VALUE,
    _E_NO3_N_S_SENSOR,
    _E_NO3_N_S_VALUE,
    
    _E_TEMP_VALUE,
    _E_TEMP_S_SENSOR,
    _E_TEMP_S_VALUE,
  
    _E_MODB_SS_END,
}eKindStateModbReg;

typedef enum
{
    /*------Kênh Modbus TCP-----*/
    _E_TURB1_WRITE,
    _E_PH1_WRITE,

    _E_TURB2_WRITE,
    _E_PH2_WRITE,
    
    _E_MODB_TCP_END,
}eKindStateModbRegTCP;

Struct_Modb_SubReg_SaoViet          sModbSubReg[2] ={0};

uint8_t Connect_Test = 0;

uint8_t ID_Default = 1;
uint8_t User_True = 1;
uint8_t User_False = 0;

#define SAOVIET_REG_INIT_DATA \
/* eKind| Block | State | cmdFC | idDev | cmdLen | Addr | vFormat | vBeLe | vScale | subReg | vReturn | nConnect */ \
  {_E_PH_VALUE,     1, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_PH_S_SENSOR,  1, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_PH_S_VALUE,   1, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_CLO_SEND_PH,  2, NULL, 0x10, NULL, 2, 0x0006, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_CLO_VALUE,    3, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_CLO_S_SENSOR, 3, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_CLO_S_VALUE,  3, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_EC_VALUE,     4, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_EC_S_SENSOR,  4, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_EC_S_VALUE,   4, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_TURB_VALUE,   5, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_TURB_S_SENSOR,5, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_TURB_S_VALUE, 5, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_COD_VALUE,    6, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_COD_S_SENSOR, 6, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_COD_S_VALUE,  6, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_TSS_VALUE,    7, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_TSS_S_SENSOR, 7, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_TSS_S_VALUE,  7, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_NH4_VALUE,    8, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_NH4_S_SENSOR, 8, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_NH4_S_VALUE,  8, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_DO_SALT,      9, NULL, 0x10, NULL, 2, 0x0008, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_DO_VALUE,    10, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_DO_S_SENSOR, 10, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_DO_S_VALUE,  10, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_SALT_VALUE,   11, NULL, 0x03, NULL, 2, 0x0008, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_SALT_S_SENSOR,11, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_SALT_S_VALUE, 11, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_TDS_VALUE,    12, NULL, 0x03, NULL, 2, 0x0006, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_TDS_S_SENSOR, 12, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_TDS_S_VALUE,  12, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_NO3_VALUE,    13, NULL, 0x03, NULL, 2, 0x0002, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_NO3_S_SENSOR, 13, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_NO3_S_VALUE,  13, NULL, 0x03, NULL, 1, 0x000B, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_NH4_N_VALUE,    8, NULL, 0x03, NULL, 2, 0x0008, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_NH4_N_S_SENSOR, 8, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_NH4_N_S_VALUE,  8, NULL, 0x03, NULL, 1, 0x000E, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_NO3_N_VALUE,    13, NULL, 0x03, NULL, 2, 0x0008, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_NO3_N_S_SENSOR, 13, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_NO3_N_S_VALUE,  13, NULL, 0x03, NULL, 1, 0x000E, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_TEMP_VALUE,   14, NULL, 0x03, NULL, 2, 0x0004, _ETYPE_F,  _E_WS, 1, NULL, NULL, NULL}, \
  {_E_TEMP_S_SENSOR,14, NULL, 0x03, NULL, 1, 0x000A, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  {_E_TEMP_S_VALUE, 14, NULL, 0x03, NULL, 1, 0x000C, _ETYPE_U8, _E_BE, 1, NULL, NULL, NULL}, \
  \
  {_E_MODB_SS_END,   0, NULL, 0, NULL, 0, 0,      0,        0,     0, NULL, NULL, NULL}

Struct_RegSensor            sRegSS_SaoViet_Port1[] =
{
    SAOVIET_REG_INIT_DATA
};

Struct_RegSensor            sRegSS_SaoViet_Port2[] =
{
    SAOVIET_REG_INIT_DATA
};

Struct_RegSensor            sRegSS_SaoViet_PortTCP[] =
{
  /*-------------------Kênh Modbus TCP------------------*/
  {_E_TURB1_WRITE,   15,    NULL,       0x03,     NULL,     2,      0x0000,  _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},
  {_E_PH1_WRITE,     15,    NULL,       0x03,     NULL,     2,      0x0002,  _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},

  {_E_TURB2_WRITE,   15,    NULL,       0x03,     NULL,     2,      0x0006,  _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},
  {_E_PH2_WRITE,     15,    NULL,       0x03,     NULL,     2,      0x0008,  _ETYPE_F,  _E_WS,   1,        NULL,     NULL,      NULL},

  {_E_MODB_TCP_END,   0, NULL, 0, NULL, 0, 0,      0,        0,     0, NULL, NULL, NULL},
};

void       RS485_SS_SaoViet_Init(Struct_RegSensor sReg[], uint8_t Port)
{
    //PH
    Config_RegSen_Read(sReg, _E_PH_VALUE, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                    &sMeasureMain[Port][_SS_PH].Value_f, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_PH_S_SENSOR, &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                       &sMeasureMain[Port][_SS_PH].stateSensor, &sMeasureMain[Port][_SS_PH].nConnect_u8);

    Config_RegSen_Read(sReg, _E_PH_S_VALUE,  &sMeasureMain[Port][_SS_PH].ID_Modbus, &sMeasureMain[Port][_SS_PH].sUser, 
                                       &sMeasureMain[Port][_SS_PH].stateValue, &sMeasureMain[Port][_SS_PH].nConnect_u8);
    //CLO
    Config_RegSen_Write(sReg, _E_CLO_SEND_PH,  &sMeasureMain[Port][_SS_CLO].ID_Modbus, &sMeasureMain[Port][_SS_CLO].sUser, 
                                        &sModbSubReg[Port].pH, &sMeasureMain[Port][_SS_CLO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_CLO_VALUE,  &sMeasureMain[Port][_SS_CLO].ID_Modbus, &sMeasureMain[Port][_SS_CLO].sUser, 
                                       &sMeasureMain[Port][_SS_CLO].Value_f, &sMeasureMain[Port][_SS_CLO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_CLO_S_SENSOR,  &sMeasureMain[Port][_SS_CLO].ID_Modbus, &sMeasureMain[Port][_SS_CLO].sUser, 
                                       &sMeasureMain[Port][_SS_CLO].stateSensor, &sMeasureMain[Port][_SS_CLO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_CLO_S_VALUE,  &sMeasureMain[Port][_SS_CLO].ID_Modbus, &sMeasureMain[Port][_SS_CLO].sUser, 
                                       &sMeasureMain[Port][_SS_CLO].stateValue, &sMeasureMain[Port][_SS_CLO].nConnect_u8);    
    
    //EC
    Config_RegSen_Read(sReg, _E_EC_VALUE,  &sMeasureMain[Port][_SS_EC].ID_Modbus, &sMeasureMain[Port][_SS_EC].sUser, 
                                       &sMeasureMain[Port][_SS_EC].Value_f, &sMeasureMain[Port][_SS_EC].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_EC_S_SENSOR,  &sMeasureMain[Port][_SS_EC].ID_Modbus, &sMeasureMain[Port][_SS_EC].sUser, 
                                       &sMeasureMain[Port][_SS_EC].stateSensor, &sMeasureMain[Port][_SS_EC].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_EC_S_VALUE,  &sMeasureMain[Port][_SS_EC].ID_Modbus, &sMeasureMain[Port][_SS_EC].sUser, 
                                       &sMeasureMain[Port][_SS_EC].stateValue, &sMeasureMain[Port][_SS_EC].nConnect_u8); 
    //TURB
    Config_RegSen_Read(sReg, _E_TURB_VALUE,  &sMeasureMain[Port][_SS_TURB].ID_Modbus, &sMeasureMain[Port][_SS_TURB].sUser, 
                                       &sMeasureMain[Port][_SS_TURB].Value_f, &sMeasureMain[Port][_SS_TURB].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TURB_S_SENSOR,  &sMeasureMain[Port][_SS_TURB].ID_Modbus, &sMeasureMain[Port][_SS_TURB].sUser, 
                                       &sMeasureMain[Port][_SS_TURB].stateSensor, &sMeasureMain[Port][_SS_TURB].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TURB_S_VALUE,  &sMeasureMain[Port][_SS_TURB].ID_Modbus, &sMeasureMain[Port][_SS_TURB].sUser, 
                                       &sMeasureMain[Port][_SS_TURB].stateValue, &sMeasureMain[Port][_SS_TURB].nConnect_u8); 
    //COD
    Config_RegSen_Read(sReg, _E_COD_VALUE,  &sMeasureMain[Port][_SS_COD].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].Value_f, &sMeasureMain[Port][_SS_COD].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_COD_S_SENSOR,  &sMeasureMain[Port][_SS_COD].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].stateSensor, &sMeasureMain[Port][_SS_COD].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_COD_S_VALUE,  &sMeasureMain[Port][_SS_COD].ID_Modbus, &sMeasureMain[Port][_SS_COD].sUser, 
                                       &sMeasureMain[Port][_SS_COD].stateValue, &sMeasureMain[Port][_SS_COD].nConnect_u8); 
    //TSS
    Config_RegSen_Read(sReg, _E_TSS_VALUE,  &sMeasureMain[Port][_SS_TSS].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].Value_f, &sMeasureMain[Port][_SS_TSS].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TSS_S_SENSOR,  &sMeasureMain[Port][_SS_TSS].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].stateSensor, &sMeasureMain[Port][_SS_TSS].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TSS_S_VALUE,  &sMeasureMain[Port][_SS_TSS].ID_Modbus, &sMeasureMain[Port][_SS_TSS].sUser, 
                                       &sMeasureMain[Port][_SS_TSS].stateValue, &sMeasureMain[Port][_SS_TSS].nConnect_u8); 
    //NH4
    Config_RegSen_Read(sReg, _E_NH4_VALUE,  &sMeasureMain[Port][_SS_NH4].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].Value_f, &sMeasureMain[Port][_SS_NH4].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_NH4_S_SENSOR,  &sMeasureMain[Port][_SS_NH4].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].stateSensor, &sMeasureMain[Port][_SS_NH4].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_NH4_S_VALUE,  &sMeasureMain[Port][_SS_NH4].ID_Modbus, &sMeasureMain[Port][_SS_NH4].sUser, 
                                       &sMeasureMain[Port][_SS_NH4].stateValue, &sMeasureMain[Port][_SS_NH4].nConnect_u8); 
    //DO
    Config_RegSen_Write(sReg, _E_DO_SALT,  &sMeasureMain[Port][_SS_DO].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                        &sModbSubReg[Port].Salt_PSU, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_VALUE,  &sMeasureMain[Port][_SS_DO].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].Value_f, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_S_SENSOR,  &sMeasureMain[Port][_SS_DO].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].stateSensor, &sMeasureMain[Port][_SS_DO].nConnect_u8);
    
    Config_RegSen_Read(sReg, _E_DO_S_VALUE,  &sMeasureMain[Port][_SS_DO].ID_Modbus, &sMeasureMain[Port][_SS_DO].sUser, 
                                       &sMeasureMain[Port][_SS_DO].stateValue, &sMeasureMain[Port][_SS_DO].nConnect_u8);    
    
    //SALT
    Config_RegSen_Read(sReg, _E_SALT_VALUE,  &sMeasureMain[Port][_SS_SALT].ID_Modbus, &sMeasureMain[Port][_SS_SALT].sUser, 
                                       &sMeasureMain[Port][_SS_SALT].Value_f, &sMeasureMain[Port][_SS_SALT].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_SALT_S_SENSOR,  &sMeasureMain[Port][_SS_SALT].ID_Modbus, &sMeasureMain[Port][_SS_SALT].sUser, 
                                       &sMeasureMain[Port][_SS_SALT].stateSensor, &sMeasureMain[Port][_SS_SALT].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_SALT_S_VALUE,  &sMeasureMain[Port][_SS_SALT].ID_Modbus, &sMeasureMain[Port][_SS_SALT].sUser, 
                                       &sMeasureMain[Port][_SS_SALT].stateValue, &sMeasureMain[Port][_SS_SALT].nConnect_u8); 
    //TDS
    Config_RegSen_Read(sReg, _E_TDS_VALUE,  &sMeasureMain[Port][_SS_TDS].ID_Modbus, &sMeasureMain[Port][_SS_TDS].sUser, 
                                       &sMeasureMain[Port][_SS_TDS].Value_f, &sMeasureMain[Port][_SS_TDS].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TDS_S_SENSOR,  &sMeasureMain[Port][_SS_TDS].ID_Modbus, &sMeasureMain[Port][_SS_TDS].sUser, 
                                       &sMeasureMain[Port][_SS_TDS].stateSensor, &sMeasureMain[Port][_SS_TDS].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TDS_S_VALUE,  &sMeasureMain[Port][_SS_TDS].ID_Modbus, &sMeasureMain[Port][_SS_TDS].sUser, 
                                       &sMeasureMain[Port][_SS_TDS].stateValue, &sMeasureMain[Port][_SS_TDS].nConnect_u8); 
    //NO3
    Config_RegSen_Read(sReg, _E_NO3_VALUE,  &sMeasureMain[Port][_SS_NO3].ID_Modbus, &sMeasureMain[Port][_SS_NO3].sUser, 
                                       &sMeasureMain[Port][_SS_NO3].Value_f, &sMeasureMain[Port][_SS_NO3].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_NO3_S_SENSOR,  &sMeasureMain[Port][_SS_NO3].ID_Modbus, &sMeasureMain[Port][_SS_NO3].sUser, 
                                       &sMeasureMain[Port][_SS_NO3].stateSensor, &sMeasureMain[Port][_SS_NO3].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_NO3_S_VALUE,  &sMeasureMain[Port][_SS_NO3].ID_Modbus, &sMeasureMain[Port][_SS_NO3].sUser, 
                                       &sMeasureMain[Port][_SS_NO3].stateValue, &sMeasureMain[Port][_SS_NO3].nConnect_u8); 
    
    //NH4_N
    Config_RegSen_Read(sReg, _E_NH4_N_VALUE,  &sMeasureMain[Port][_SS_NH4_N].ID_Modbus, &sMeasureMain[Port][_SS_NH4_N].sUser, 
                                       &sMeasureMain[Port][_SS_NH4_N].Value_f, &sMeasureMain[Port][_SS_NH4_N].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_NH4_N_S_SENSOR,  &sMeasureMain[Port][_SS_NH4_N].ID_Modbus, &sMeasureMain[Port][_SS_NH4_N].sUser, 
                                       &sMeasureMain[Port][_SS_NH4_N].stateSensor, &sMeasureMain[Port][_SS_NH4_N].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_NH4_N_S_VALUE,  &sMeasureMain[Port][_SS_NH4_N].ID_Modbus, &sMeasureMain[Port][_SS_NH4_N].sUser, 
                                       &sMeasureMain[Port][_SS_NH4_N].stateValue, &sMeasureMain[Port][_SS_NH4_N].nConnect_u8); 
    
    //NO3_N
    Config_RegSen_Read(sReg, _E_NO3_N_VALUE,  &sMeasureMain[Port][_SS_NO3_N].ID_Modbus, &sMeasureMain[Port][_SS_NO3_N].sUser, 
                                       &sMeasureMain[Port][_SS_NO3_N].Value_f, &sMeasureMain[Port][_SS_NO3_N].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_NO3_N_S_SENSOR,  &sMeasureMain[Port][_SS_NO3_N].ID_Modbus, &sMeasureMain[Port][_SS_NO3_N].sUser, 
                                       &sMeasureMain[Port][_SS_NO3_N].stateSensor, &sMeasureMain[Port][_SS_NO3_N].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_NO3_N_S_VALUE,  &sMeasureMain[Port][_SS_NO3_N].ID_Modbus, &sMeasureMain[Port][_SS_NO3_N].sUser, 
                                       &sMeasureMain[Port][_SS_NO3_N].stateValue, &sMeasureMain[Port][_SS_NO3_N].nConnect_u8); 
    //TEMP
    Config_RegSen_Read(sReg, _E_TEMP_VALUE,  &sMeasureMain[Port][_SS_TEMP].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].Value_f, &sMeasureMain[Port][_SS_TEMP].nConnect_u8);    
    
    Config_RegSen_Read(sReg, _E_TEMP_S_SENSOR,  &sMeasureMain[Port][_SS_TEMP].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].stateSensor, &sMeasureMain[Port][_SS_TEMP].nConnect_u8); 
    
    Config_RegSen_Read(sReg, _E_TEMP_S_VALUE,  &sMeasureMain[Port][_SS_TEMP].ID_Modbus, &sMeasureMain[Port][_SS_TEMP].sUser, 
                                       &sMeasureMain[Port][_SS_TEMP].stateValue, &sMeasureMain[Port][_SS_TEMP].nConnect_u8); 
}

void       RS485_SS_SaoViet_TCP_Init(Struct_RegSensor sReg[], uint8_t Port)
{
    //Modbus TCP
    Config_RegSen_Write(sReg, _E_TURB1_WRITE,  &ID_Default, &User_True, 
                                       &sMeasureMain[0][_SS_TURB].Value_f, &Connect_Test);    
    
    Config_RegSen_Write(sReg, _E_PH1_WRITE,  &ID_Default, &User_True, 
                                       &sMeasureMain[0][_SS_PH].Value_f, &Connect_Test);    

    Config_RegSen_Write(sReg, _E_TURB2_WRITE,  &ID_Default, &User_True, 
                                       &sMeasureMain[1][_SS_TURB].Value_f, &Connect_Test); 
    Config_RegSen_Write(sReg, _E_PH2_WRITE,  &ID_Default, &User_True, 
                                       &sMeasureMain[1][_SS_PH].Value_f, &Connect_Test); 
}

void    Digital_SS_SaoViet_CallCycle(void)
{
    for(uint8_t i = _SS_PH; i<_END_SENSOR; i++)
    {
        if(sMeasureMain[0][i].sUser != 0)
        {
            sRegSS_SaoViet_Port1[_E_TEMP_VALUE].idDev = &sMeasureMain[0][i].ID_Modbus;
            sRegSS_SaoViet_Port1[_E_TEMP_S_SENSOR].idDev = &sMeasureMain[0][i].ID_Modbus;
            sRegSS_SaoViet_Port1[_E_TEMP_S_VALUE].idDev = &sMeasureMain[0][i].ID_Modbus;
            break;
        }
    }
    
    for(uint8_t i = _SS_PH; i<_END_SENSOR; i++)
    {
        if(sMeasureMain[1][i].sUser != 0)
        {
            sRegSS_SaoViet_Port2[_E_TEMP_VALUE].idDev = &sMeasureMain[1][i].ID_Modbus;
            sRegSS_SaoViet_Port2[_E_TEMP_S_SENSOR].idDev = &sMeasureMain[1][i].ID_Modbus;
            sRegSS_SaoViet_Port2[_E_TEMP_S_VALUE].idDev = &sMeasureMain[1][i].ID_Modbus;
            break;
        }
    }
    
    for(uint8_t i = 0; i< 2; i++)
    {
        //Handle SubReg pH
        if(sMeasureMain[i][_SS_PH].Value_f == 0)
          sModbSubReg[i].pH = 7;
        else if(sMeasureMain[i][_SS_PH].Value_f < 5)
          sModbSubReg[i].pH = 5;
        else if(sMeasureMain[i][_SS_PH].Value_f > 9)
          sModbSubReg[i].pH = 9;
        else
          sModbSubReg[i].pH = sMeasureMain[i][_SS_PH].Value_f;
        
        //Handle SubReg PSU
        sModbSubReg[i].Salt_PSU = sMeasureMain[i][_SS_SALT].Value_f * 10;
    }
}

