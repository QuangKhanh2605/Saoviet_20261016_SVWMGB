
/*
    Firmware giai ma growatt inverter 
*/
#include "growatt_inverter.h"

//#include "user_modbus_rtu.h"

/*------------------Var va struct--------------------*/
/*Bang than ghi modbus*/
sGRO_Register_List sGRO_REGISTER[] = 
{
     //param              Funcode  addr    numreg   scale   unit    format          //obis
    //information
    {   __GRO_MANUF,        3,      34,       8,    0,      NULL,   __FM_STRING,        NULL,   }, 
    {   __GRO_SERIAL,       3,      23,       5,    0,      NULL,   __FM_STRING,        NULL,   }, 
    
    {   __GRO_STIME,        3,      45,       7,    0,      NULL,   __FM_ARRAY,         NULL,   },
    {   __GRO_TYPE,         3,      43,       1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_INVER_TYPE,   3,      44,       1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_INVER_STAT,   4,      0,        1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_INVER_POW,    3,      6,        2,    0xFF,   NULL,   __FM_LONG,          NULL,   }, 
    {   __GRO_ON_OFF,       3,      0,        1,    0,      NULL,   __FM_USHORT,        NULL,   },
      
    //opera
    {   __GRO_EN_TODAY,     4,      53,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_EN_TOTAL,     4,      55,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_Pout,         4,      35,       2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    {   __GRO_Qout,         4,      232,      2,    0xFF,   "VAr",  __FM_LONG,          NULL,   },
    
    {   __GRO_Va,           4,      38,       1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_Vb,           4,      42,       1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_Vc,           4,      46,       1,    0xFF,   "V",    __FM_USHORT,        NULL,   },   
    
    {   __GRO_Ia,           4,      39,       1,    0xFF,   "A",    __FM_USHORT,        NULL,   },
    {   __GRO_Ib,           4,      43,       1,    0xFF,   "A",    __FM_USHORT,        NULL,   }, 
    {   __GRO_Ic,           4,      47,       1,    0xFF,   "A",    __FM_USHORT,        NULL,   }, 
    
    {   __GRO_F,            4,      37,       1,    0xFE,   "Hz",   __FM_USHORT,        NULL,   },
    {   __GRO_PF,           4,      100,      1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    
    {   __GRO_PPV_T,        4,      1,        2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    
    {   __GRO_VPV_1,        4,      3,        1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_CPV_1,        4,      4,        1,    0xFF,   "A",    __FM_USHORT,        NULL,   },
    {   __GRO_PPV_1,        4,      5,        2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    {   __GRO_EPV_D1,       4,      59,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_EPV_T1,       4,      61,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    
    {   __GRO_VPV_2,        4,      7,        1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_CPV_2,        4,      8,        1,    0xFF,   "A",    __FM_USHORT,        NULL,   },
    {   __GRO_PPV_2,        4,      9,        2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    {   __GRO_EPV_D2,       4,      63,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_EPV_T2,       4,      65,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    
    {   __GRO_VPV_3,        4,      11,       1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_CPV_3,        4,      12,       1,    0xFF,   "A",    __FM_USHORT,        NULL,   },
    {   __GRO_PPV_3,        4,      13,       2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    {   __GRO_EPV_D3,       4,      67,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_EPV_T3,       4,      69,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   }, 
    
    {   __GRO_VPV_4,        4,      15,       1,    0xFF,   "V",    __FM_USHORT,        NULL,   },
    {   __GRO_CPV_4,        4,      16,       1,    0xFF,   "A",    __FM_USHORT,        NULL,   },
    {   __GRO_PPV_4,        4,      17,       2,    0xFF,   "W",    __FM_LONG,          NULL,   },
    {   __GRO_EPV_D4,       4,      71,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },
    {   __GRO_EPV_T4,       4,      73,       2,    0xFF,   "kWh",  __FM_LONG,          NULL,   },  
    
    {   __GRO_ALOW_Pout,    0,      0,        1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_SET_Pout,     3,      3,        1,    0,      "%",    __FM_USHORT,        NULL,   }, 
    
    {   __GRO_ALOW_Qout,    0,      0,        1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_SET_Qout,     3,      4,        1,    0,      "%",    __FM_USHORT,        NULL,   },
    
    //event
    {   __GRO_FAULT_CODE,   4,      105,      1,    0,      NULL,   __FM_USHORT,        NULL,   }, 
    {   __GRO_FAULT_BIT,    4,      106,      2,    0,      NULL,   __FM_LONG,          NULL,   }, 
    //hybrid
};

/*--------------------Function------------------------*/
float GRO_Extract_Data (sData *str, uint8_t reg) 
{
    uint32_t TempLong = 0;
    uint16_t TempU16 = 0;
    float  Tempfloat = 0;
    
    switch (sGRO_REGISTER[reg].Format_u8)
    {
        case __FM_USHORT:
            GRO_Get_Data(str, reg, &TempU16); 
            Tempfloat = TempU16 * Convert_Scale(sGRO_REGISTER[reg].Scale_u8);
            break;
        case __FM_LONG:
            GRO_Get_Data(str, reg, &TempLong); 
            Tempfloat = TempLong * Convert_Scale(sGRO_REGISTER[reg].Scale_u8);
            break;
        default:
            break;
    }
    
    if (reg == __GRO_PF) {
        Tempfloat = (TempU16 - 10000) / 10000;  
    }
    
    return Tempfloat;
}


/*
    Func: get data tu buff uart
*/
uint8_t GRO_Get_Data (sData *str, uint8_t reg, void *result) 
{
    uint8_t aTEMP[32] = {0}, i = 0, nByte = sGRO_REGISTER[reg].nReg_u8 * 2;
    uint8_t *pData = str->Data_a8 + 3;   //id func length
    uint16_t tempU16 = 0;
    uint32_t tempU32 = 0;
    char *strResult;
    uint8_t *pArr;
        
    memcpy(aTEMP, pData, nByte);
    
    switch (sGRO_REGISTER[reg].Format_u8)
    {
        case __FM_USHORT:
            tempU16 = aTEMP[0] << 8 | aTEMP[1];
            *((uint16_t *) result) = tempU16; 
            break;
        case __FM_LONG:
            tempU32 = aTEMP[0] << 24 | aTEMP[1] << 16 | aTEMP[2] << 8 | aTEMP[3]; 
            *((uint32_t *) result) = tempU32; 
            break;
        case __FM_STRING:
            strResult = (char *) result;
            
            for (i = 0; i < nByte; i++) {
                *(strResult++) = aTEMP[i];
            }
            break;
        case __FM_ARRAY:
            pArr = (uint8_t *) result;
            for (i = 0; i < nByte; i++) {
                *(pArr++) = aTEMP[i];
            }
            break;
    }
    
    return nByte;
}





