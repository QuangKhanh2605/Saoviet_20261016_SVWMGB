

#ifndef SENSOR_TEMP_SV_H
#define SENSOR_TEMP_SV_H


#include "user_util.h"
#include "user_define.h"

extern Struct_RegSensor            sRegSS_TempSV_PortRTU[];
/*=============== Function handle ================*/
void        RS485_SS_TempSV_Init_RTU(Struct_RegSensor sReg[], uint8_t Port);
void        Digital_SS_TempSV_CallCycle(void);


#endif
