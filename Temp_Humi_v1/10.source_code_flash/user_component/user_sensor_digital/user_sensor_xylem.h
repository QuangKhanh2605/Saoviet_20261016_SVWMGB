

#ifndef SENSOR_XYLEM_H
#define SENSOR_XYLEM_H


#include "user_util.h"
#include "user_define.h"

extern Struct_RegSensor            sRegSS_Xylem_PortTCP[];
/*=============== Function handle ================*/
void        RS485_SS_XyLem_Init_TCP(Struct_RegSensor sReg[], uint8_t Port);
void        Digital_SS_XyLem_CallCycle(void);


#endif
