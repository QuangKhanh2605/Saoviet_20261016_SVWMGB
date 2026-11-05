

#ifndef SENSOR_DARUIFUNO_H
#define SENSOR_DARUIFUNO_H


#include "user_util.h"
#include "user_define.h"

extern Struct_RegSensor            sRegSS_Daruifuno_PortRTU[];
/*=============== Function handle ================*/
void        RS485_SS_Daruifuno_Init(Struct_RegSensor sReg[], uint8_t Port);
void        Digital_SS_Daruifuno_CallCycle(void);

#endif
