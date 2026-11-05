


#include "siemen_fmt020.h"
#include "math.h"
/*
    float:  45 61
             0000
                    = 3600
 */

/*Bang than ghi modbus*/
sFMT020RegList sFMT020_REGISTER[] = 
{
    {   __FMT_020_FLOW,            3000,       2,          __FMT_FLOAT   },
    {   __FMT_020_TOTAL_1,         8300,       2,          __FMT_FLOAT   },
    {   __FMT_020_TOTAL_2,         8400,       2,          __FMT_FLOAT   },
};



static uint8_t REG_LIST[3] = {
    __FMT_020_FLOW,
    __FMT_020_TOTAL_1,
    __FMT_020_TOTAL_2,
};

/*============= Function ==================*/
uint8_t FMT020_Decode (uint8_t ireg, sData *pData, void *starget)
{
    uint8_t result = pending;
    uint8_t reg = REG_LIST[ireg]; 
    sFMT020Data *pFMT020 = starget;
    
    switch (reg)
    {
        case __FMT_020_FLOW:
            FMT020_Extract_Data (pData, reg, &pFMT020->Flow_f); 
            break;
        case __FMT_020_TOTAL_1:
            FMT020_Extract_Data (pData, reg, &pFMT020->Total1_f); 
            break;
        case __FMT_020_TOTAL_2:
            FMT020_Extract_Data (pData, reg, &pFMT020->Total2_f); 
            result = true;
            break;
        default:
            break;
    }
    
    return result;
}



void FMT020_Extract_Data (sData *pSource, uint8_t Param, void *data)
{
    uint8_t aTEMP[8] = {0};
    uint32_t TempU32 = 0;
    uint16_t TempU16 = 0;
    float Tempf = 0, TempDecF = 0;
    double Tempdb = 0;
    
    if (pSource->Length_u16 < 4) {
        UTIL_Printf_Str (DBLEVEL_M, "wm_wo_ultra: not value\r\n" );
        return ;
    }
    
    switch (sFMT020_REGISTER[Param].Format_u8)
    {
        case __FMT_USHORT:
            for (uint8_t i = 0; i < 2; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU16 = aTEMP[0] << 8 | aTEMP[1];
                
            *(uint16_t *) data = ( TempU16 ); 
            break;
        case __FMT_LONG:
            for (uint8_t i = 0; i < 4; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];

            *(int32_t *) data = (int32_t) TempU32; 
            break;
        case __FMT_FLOAT:
            for (uint8_t i = 0; i < 4; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];

            *(float *)data = Convert_FloatPoint_2Float(TempU32);
            break;
        case __FMT_DOUBLE:
            for (uint8_t i = 0; i < 8; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);

            Tempdb = *((double*)aTEMP);
            
            *(float *)data = Tempdb;  
            break;
        case __FMT_STRING:
            aTEMP[0] = strlen((char *)pSource->Data_a8);
            
            if ( (aTEMP[0] > 0) && (aTEMP[0] < 10) ) {
                sprintf((char *) data, " (%s)", (char *) pSource->Data_a8);
            }
            break;
        case __FMT_TT_TYPE:
            for (uint8_t i = 0; i < 4; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];

            Tempf = (int32_t) TempU32; 
            
            for (uint8_t i = 4; i < 8; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];
            
            TempDecF = (int32_t) TempU32; 
            TempDecF = TempDecF / pow (10, 9);
            
            Tempf += TempDecF;  
            *(float *)data = Tempf;
            break;
        default:
            break;
    }   
}


uint8_t FMT020_Get_Reg (uint8_t index, uint16_t *addr, uint8_t *nReg)
{
    uint8_t reg = REG_LIST[index];
    
    *addr = sFMT020_REGISTER[reg].Addr;
    *nReg = sFMT020_REGISTER[reg].nreg_u8;
        
    return false;
}











