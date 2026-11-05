

#ifndef FMT_020_H
#define FMT_020_H


#include "user_util.h"

typedef enum
{
    __FMT_USHORT,
    __FMT_LONG,
    __FMT_FLOAT,
    __FMT_DOUBLE,
    __FMT_STRING,
    __FMT_TT_TYPE,
}eFMT020Format;


typedef enum
{
    __FMT_020_FLOW,
    __FMT_020_TOTAL_1,
    __FMT_020_TOTAL_2,
        
    __FMT_020_END,
}sFMT020Parameter;


typedef struct
{
    uint8_t             Param_u8;
    uint16_t            Addr;
    uint16_t            nreg_u8;
    eFMT020Format       Format_u8;        
}sFMT020RegList;


typedef struct
{
    float   Flow_f;
    
    float   Total1_f;
    float   Total2_f;
}sFMT020Data;

extern sFMT020RegList sFMT020_REGISTER[];

/*=========== Function =========*/
uint8_t FMT020_Decode (uint8_t ireg, sData *pData, void *starget);
void    FMT020_Extract_Data (sData *pSource, uint8_t Param, void *data);
uint8_t FMT020_Get_Reg (uint8_t index, uint16_t *addr, uint8_t *nReg);


#endif




