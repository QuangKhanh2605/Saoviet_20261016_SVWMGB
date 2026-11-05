

#ifndef MAG_6000_H
#define MAG_6000_H


#include "user_util.h"

typedef enum
{
    __MAG6_USHORT,
    __MAG6_LONG,
    __MAG6_FLOAT,
    __MAG6_DOUBLE,
    __MAG6_STRING,
    __MAG6_TT_TYPE,
}eMag6000Format;


typedef enum
{
    __MAG_6000_FLOW_U,
    __MAG_6000_TOTAL_U,
    
    __MAG_6000_TOTAL_SET1,
    __MAG_6000_TOTAL_SET2,

    __MAG_6000_FLOW,
    __MAG_6000_TOTAL_1,
    __MAG_6000_TOTAL_2,
        
    __MAG_6000_END,
}sMag6000Parameter;


typedef struct
{
    uint8_t             Param_u8;
    uint16_t            Addr;
    uint16_t            nreg_u8;
    eMag6000Format      Format_u8;        
}sMag6000RegList;


typedef struct
{
    float   Flow_f;
    
    float   Total1_f;
    float   Total2_f;
    
    float   Forward_f;
    float   Revert_f;
    float   Net_f;
    
    uint16_t TotalSetup1_u16;
    uint16_t TotalSetup2_u16;
    
    uint16_t PinPercent_u16;
    
    char    aFLOW_UNIT[10];
    char    aTOTAL_UNIT[10];
    
    float   FlowUnitFactor_f;
    float   TotalUnitFactor_f;
}sMag6000Data;

extern sMag6000RegList sMAG6000_REGISTER[];

/*=========== Function =========*/
uint8_t MAG6000_Decode (uint8_t ireg, sData *pData, void *starget);
void    MAG6000_Extract_Data (sData *pSource, uint8_t Param, void *data);
uint8_t MAG6000_Get_Reg (uint8_t index, uint16_t *addr, uint8_t *nReg);


#endif




