
#include "user_util.h"
   

#ifndef _GROWATT_INVERTER_H_
#define _GROWATT_INVERTER_H_



typedef enum
{
    __FM_USHORT,
    __FM_LONG,
    __FM_STRING,
    __FM_ARRAY,
}eGROFormat;


typedef enum
{
    __GRO_MANUF,
    __GRO_SERIAL,
    
    __GRO_STIME,
    __GRO_TYPE,
    __GRO_INVER_TYPE,
    __GRO_INVER_STAT,
    __GRO_INVER_POW,
    __GRO_ON_OFF,
    
    __GRO_EN_TODAY,
    __GRO_EN_TOTAL,
    __GRO_Pout,
    __GRO_Qout,
    
    __GRO_Va,  
    __GRO_Vb,
    __GRO_Vc,
    
    __GRO_Ia,  
    __GRO_Ib,
    __GRO_Ic,
    
    __GRO_F,
    __GRO_PF,
    
    __GRO_PPV_T,
    
    __GRO_VPV_1,
    __GRO_CPV_1,
    __GRO_PPV_1,
    __GRO_EPV_D1,
    __GRO_EPV_T1,
    
    __GRO_VPV_2,
    __GRO_CPV_2,
    __GRO_PPV_2,
    __GRO_EPV_D2,
    __GRO_EPV_T2,
    
    __GRO_VPV_3,
    __GRO_CPV_3,
    __GRO_PPV_3,
    __GRO_EPV_D3,
    __GRO_EPV_T3,

    __GRO_VPV_4,
    __GRO_CPV_4,
    __GRO_PPV_4,
    __GRO_EPV_D4,
    __GRO_EPV_T4,
    
    __GRO_ALOW_Pout,
    __GRO_SET_Pout,
    
    __GRO_ALOW_Qout,
    __GRO_SET_Qout,
    
    __GRO_FAULT_CODE,
    __GRO_FAULT_BIT,
    
    //hybrid
    __GRO_REG_END,
    
}sGROParameter;


typedef struct
{
    uint8_t         Param_u8;
    uint8_t         Funcode_u8;
    uint16_t        Addr;
    uint8_t         nReg_u8;
    uint8_t         Scale_u8;
    char            *pUnit;
    eGROFormat      Format_u8;  
    char            *Obis;
}sGRO_Register_List;

extern sGRO_Register_List sGRO_REGISTER[];

//-------------------------Function--------------------
uint8_t GRO_Get_Data (sData *str, uint8_t reg, void *result) ;
float GRO_Extract_Data (sData *str, uint8_t reg); 


#endif /* 2_START_INC_READ_METER_H_ */



