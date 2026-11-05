


#include "siemen_mag6000.h"
#include "math.h"
/*
    float:  45 61
             0000
                    = 3600
 */

/*Bang than ghi modbus*/
sMag6000RegList sMAG6000_REGISTER[] = 
{
           //param           Register(2byte)    nreg        format
    {   __MAG_6000_FLOW_U,          2912,       6,          __MAG6_USHORT    },
    {   __MAG_6000_TOTAL_U,         2914,       6,          __MAG6_USHORT    },
    
    {   __MAG_6000_TOTAL_SET1,      2101,       1,          __MAG6_USHORT    },
    {   __MAG_6000_TOTAL_SET2,      2103,       1,          __MAG6_USHORT    },
    
    {   __MAG_6000_FLOW,            3002,       2,          __MAG6_FLOAT     },
    {   __MAG_6000_TOTAL_1,         3022,       2,          __MAG6_FLOAT   },
    {   __MAG_6000_TOTAL_2,         3024,       2,          __MAG6_FLOAT   },
};



static uint8_t REG_LIST[5] = {
    __MAG_6000_TOTAL_SET1,
    __MAG_6000_TOTAL_SET2,
    
    __MAG_6000_FLOW,
    __MAG_6000_TOTAL_1,
    __MAG_6000_TOTAL_2,
};

/*============= Function ==================*/
uint8_t MAG6000_Decode (uint8_t ireg, sData *pData, void *starget)
{
    uint8_t result = pending, mark = false;
    uint8_t reg = REG_LIST[ireg]; 
    sMag6000Data *pMag6000 = starget;
    
    switch (reg)
    {
        case __MAG_6000_TOTAL_SET1:
            MAG6000_Extract_Data (pData, reg, &pMag6000->TotalSetup1_u16); 
            break; 
        case __MAG_6000_TOTAL_SET2:
            MAG6000_Extract_Data (pData, reg, &pMag6000->TotalSetup2_u16); 
            break; 
        case __MAG_6000_FLOW:
            MAG6000_Extract_Data (pData, reg, &pMag6000->Flow_f); 
            break;
        case __MAG_6000_TOTAL_1:
            MAG6000_Extract_Data (pData, reg, &pMag6000->Total1_f); 
            break;
        case __MAG_6000_TOTAL_2:
            MAG6000_Extract_Data (pData, reg, &pMag6000->Total2_f); 
            
            pMag6000->Net_f = 0;
            switch (pMag6000->TotalSetup1_u16)
            {
                case 1:
                    pMag6000->Forward_f = pMag6000->Total1_f;
                    break;
                case 0:
                    pMag6000->Revert_f = pMag6000->Total1_f;
                    break;
                case 2:
                    mark = true;
                    pMag6000->Net_f = pMag6000->Total1_f;
                    break;
                default:
                    break;
            }
                   
            switch (pMag6000->TotalSetup2_u16)
            {
                case 1:
                    pMag6000->Forward_f = pMag6000->Total2_f;
                    break;
                case 0:
                    pMag6000->Revert_f = pMag6000->Total2_f;
                    break;
                case 2:
                    mark = true;
                    pMag6000->Net_f = pMag6000->Total2_f;
                    break;
                default:
                    break;
            }
            
            if (mark == false) {
                pMag6000->Net_f = pMag6000->Forward_f - pMag6000->Revert_f;
            }
            
            result = true;
            break;
        default:
            break;
    }
    
    return result;
}



void MAG6000_Extract_Data (sData *pSource, uint8_t Param, void *data)
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
    
    switch (sMAG6000_REGISTER[Param].Format_u8)
    {
        case __MAG6_USHORT:
            for (uint8_t i = 0; i < 2; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU16 = aTEMP[0] << 8 | aTEMP[1];
                
            *(uint16_t *) data = ( TempU16 ); 
            break;
        case __MAG6_LONG:
            for (uint8_t i = 0; i < 4; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];

            *(int32_t *) data = (int32_t) TempU32; 
            break;
        case __MAG6_FLOAT:
            for (uint8_t i = 0; i < 4; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);
            
            TempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3];

            *(float *)data = Convert_FloatPoint_2Float(TempU32);
            break;
        case __MAG6_DOUBLE:
            for (uint8_t i = 0; i < 8; i++)
                aTEMP[i] = *(pSource->Data_a8 + i);

            Tempdb = *((double*)aTEMP);
            
            *(float *)data = Tempdb;  
            break;
        case __MAG6_STRING:
            aTEMP[0] = strlen((char *)pSource->Data_a8);
            
            if ( (aTEMP[0] > 0) && (aTEMP[0] < 10) ) {
                sprintf((char *) data, " (%s)", (char *) pSource->Data_a8);
            }
            break;
        case __MAG6_TT_TYPE:
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


uint8_t MAG6000_Get_Reg (uint8_t index, uint16_t *addr, uint8_t *nReg)
{
    uint8_t reg = REG_LIST[index];
    
//    if (index < MaxReg) {
        *addr = sMAG6000_REGISTER[reg].Addr;
        *nReg = sMAG6000_REGISTER[reg].nreg_u8;
//        return true;
//    }
//    
    return false;
}











