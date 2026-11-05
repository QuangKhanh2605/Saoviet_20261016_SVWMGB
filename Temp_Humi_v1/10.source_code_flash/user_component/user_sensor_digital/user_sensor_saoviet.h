

#ifndef SENSOR_SAOVIET_H
#define SENSOR_SAOVIET_H


#include "user_util.h"
#include "user_define.h"

typedef struct
{
    float pH;
    float Salt_PSU;
}Struct_Modb_SubReg_SaoViet;

extern Struct_RegSensor            sRegSS_SaoViet_Port1[];
extern Struct_RegSensor            sRegSS_SaoViet_Port2[];
extern Struct_RegSensor            sRegSS_SaoViet_PortTCP[];
/*=============== Function handle ================*/
void        RS485_SS_SaoViet_Init(Struct_RegSensor sReg[], uint8_t Port);
void        RS485_SS_SaoViet_TCP_Init(Struct_RegSensor sReg[], uint8_t Port);
void        Digital_SS_SaoViet_CallCycle(void);


#endif
