


#include "user_lptim.h"
#include "lptim.h"

#include "main.h"


#define PERIOD_COUNTER     0xFFFF
/*================Function=====================*/

void LPTIM_Init_Feature (void)
{
#ifdef LPTIM_ENCODER_MODE
    HAL_LPTIM_Encoder_Start_IT(&hlptim1, PERIOD_COUNTER);
#else  
    HAL_LPTIM_Counter_Start(&hlptim1, PERIOD_COUNTER);
#endif
}



/*
    Feature LpTimer.
*/

void LPTIM_Encoder_Get_Pulse (LPTIM_HandleTypeDef *hlptim, Struct_Pulse *sPulse)
{
    uint16_t LPTRegisValue_u32 = 0;
    static uint16_t LasValue_u16[2] = {0};
    uint8_t Index = 0;
    
    if (hlptim->Instance == LPTIM1)
    {
        Index = 0;
    } else
    {
        Index = 1;
    }
    
	LPTRegisValue_u32 = (uint16_t) HAL_LPTIM_ReadCounter(hlptim);

	if (LPTRegisValue_u32 >=  LasValue_u16[Index])
	{
		if ((LPTRegisValue_u32 - LasValue_u16[Index]) < 0xFFAF)   //khong the quay nguoc dc 0x50 xung trong 10p va khong the quay tien 0xFFAF
		{
			sPulse->Number_i64 	    += (LPTRegisValue_u32 - LasValue_u16[Index]);  //binh thuong
			LasValue_u16[Index]   = LPTRegisValue_u32;
		} else
		{
			sPulse->Number_i64 		-= (PERIOD_COUNTER - LPTRegisValue_u32 + LasValue_u16[Index] + 1); //quay nguoc
			LasValue_u16[Index]   = LPTRegisValue_u32;
		}
	} else
	{
		if ((LasValue_u16[Index] - LPTRegisValue_u32) > 0x50)   //khong the quay nguoc dc 0x50 xung trong 10p
		{
			sPulse->Number_i64  += (PERIOD_COUNTER - LasValue_u16[Index] + LPTRegisValue_u32 + 1);  //binh thuong
			LasValue_u16[Index]		= LPTRegisValue_u32;
		} else
		{
			sPulse->Number_i64  -= (LasValue_u16[Index] - LPTRegisValue_u32);  //binh thuong
			LasValue_u16[Index]		= LPTRegisValue_u32;
		}
	}
    
    sPulse->Total_lf = sPulse->Start_lf + sPulse->Number_i64 / 4 * sPulse->FactorInt_i16   \
                                                    * Convert_Scale(sPulse->FactorDec_u8);
}


/*
    Func: Callback Lptim Interupt HAL
*/
void HAL_LPTIM_DirectionUpCallback(LPTIM_HandleTypeDef *hlptim)
{
//    UTIL_Printf_Str( DBLEVEL_M, "u_lptim: dir change to forward\r\n" );
//    pLPTIM_Func_Direct_Change_Handler (_DIRECT_FORWARD);
}


void HAL_LPTIM_DirectionDownCallback(LPTIM_HandleTypeDef *hlptim)
{
//    UTIL_Printf_Str( DBLEVEL_M, "u_lptim: dir change to reverse\r\n" ); 
//    pLPTIM_Func_Direct_Change_Handler (_DIRECT_REVERSE);
}

                     

void LPTIM_Counter_Get_Pulse (LPTIM_HandleTypeDef *hlptim, Struct_Pulse *sPulse)
{
    uint16_t LPTRegisValue_u16 = 0;
    static uint16_t LasValue_u16[2] = {0};
    uint8_t Index = 0;
    
    if (hlptim->Instance == LPTIM1) {
        Index = 0;
    } else {
        Index = 1;
    }
    
    LPTRegisValue_u16 = (uint16_t) HAL_LPTIM_ReadCounter(hlptim);

    if (sPulse->Direct_u8 == 1) {
        sPulse->Number_i64 = sPulse->Number_i64 - LPTIM_Check_Pulse_Period( LPTRegisValue_u16, LasValue_u16[Index]);
    } else {
        sPulse->Number_i64 = sPulse->Number_i64 + LPTIM_Check_Pulse_Period(LPTRegisValue_u16, LasValue_u16[Index]);
    }
    
    sPulse->Total_lf = sPulse->Start_lf + sPulse->Number_i64 * sPulse->FactorInt_i16 \
                                                    * Convert_Scale(sPulse->FactorDec_u8);

    LasValue_u16[Index]      = LPTRegisValue_u16;
    
    LPTIM_Check_DIR_PIN (hlptim, sPulse);
}


void LPTIM_Check_DIR_PIN (LPTIM_HandleTypeDef *hlptim, Struct_Pulse *sPulse)
{
    if (hlptim->Instance == LPTIM1) {
        sPulse->Direct_u8 = !HAL_GPIO_ReadPin (PULSE_DIR_GPIO_Port, PULSE_DIR_Pin);
    } 
}

uint16_t LPTIM_Check_Pulse_Period (uint16_t Last_Pulse , uint16_t First_Pulse)
{
    if (Last_Pulse < First_Pulse)
		return (PERIOD_COUNTER - First_Pulse + Last_Pulse + 1);
	return (Last_Pulse - First_Pulse);
}














