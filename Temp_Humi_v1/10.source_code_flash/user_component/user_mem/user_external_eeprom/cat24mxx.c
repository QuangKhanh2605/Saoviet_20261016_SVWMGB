

#include "cat24mxx.h"

static uint8_t _Cb_CAT24Mxx_Write (uint8_t event);
static uint8_t _Cb_CAT24Mxx_Read (uint8_t event);

/*======== struct ===============*/
sEvent_struct sEventCAT24[] =
{
	{ _EVENT_CAT24Mxx_WRITE, 	    0, 0, 200,  			_Cb_CAT24Mxx_Write  },
	{ _EVENT_CAT24Mxx_READ, 	    0, 0, 200,  			_Cb_CAT24Mxx_Read   },
};

static uint8_t          aDATA_CAT24_READ[MAX_MEM_DATA];
sMemoryVariable         sCAT24Var;
sFROM_Manager_Struct    sFROM_Manager;
/*======================= Function =======================*/

/*
    - Neu ghi vao cuoi cua Page. Data tiep tuc ghi vao dau cua page do?
    - Khi doc thi se tu dong tang len vao k bi gioi han bo end page (ma la end mem)
*/

uint8_t CAT24Mxx_Write_Buff (uint32_t addr, uint8_t *pdata, uint16_t length)
{
    uint16_t nByte = 0;
    uint16_t MAX_WRITE = 0;
    uint8_t *pWrite = pdata;
    
    while (length != 0)
    {
        //ghi vao 1 page nguyen
        MAX_WRITE = CAT_PAGE_SIZE - (addr % CAT_PAGE_SIZE);
        
        if (length > MAX_WRITE) {
            nByte = MAX_WRITE;
        } else {
            nByte = length;
        }

        if (CAT24Mxx_Write_Array (addr, pWrite, nByte) == 0) {
            UTIL_Printf_Str( DBLEVEL_M, "u_ex_eeprom: write eeprom fail!\r\n" ); 
            return false;
        }
        
        length -= nByte;
        pWrite += nByte;
        addr += nByte;
    }
    
    return true;
}


uint8_t CAT24Mxx_Write_Array (uint32_t writeAddr, uint8_t *dataBuff, uint16_t dataLength)
{
    uint8_t reVal       = 0U, bank = 0;
    uint8_t addrA16bit  = 0U;
    uint8_t catBuff[CAT_PAGE_SIZE];
    uint16_t index, addr = 0;

    sFROM_Manager.Count_retry_u8 = NUMBER_RETRY_R_W;
    /* check input parameters */
    if ((NULL == dataBuff) || (0U == dataLength)) {
        sFROM_Manager.status_u8 = 1U;
        return 0U;
    }

    /* MAX 64kb dia chi. them bit A17 A16 cho flash M24M02E_U*/
    bank = writeAddr / 0xFFFF;
    addr = (uint16_t) writeAddr;  
    addrA16bit = I2C_CAT24Mxx_ADDR_7BIT | (bank << 1);

    do
    {
        if (HAL_I2C_Mem_Write(&I2C_CAT24, addrA16bit, addr, I2C_MEMADD_SIZE_16BIT, dataBuff, dataLength, 1000) == HAL_OK)
        {
            HAL_Delay(CAT24_DELAY);
            if (HAL_I2C_Mem_Read(&I2C_CAT24, addrA16bit, addr, I2C_MEMADD_SIZE_16BIT, catBuff, dataLength, 1000) == HAL_OK)
            {
                for (index = 0; index < dataLength; index++) {
                    if (catBuff[index] != *(dataBuff + index)) {
                        sFROM_Manager.Count_retry_u8--;
                        reVal = 0U;
                        break;
                    } else {
                        if (1U != reVal) {
                            reVal = 1U;
                        }
                    }
                }

                if (0U != reVal) {
                    sFROM_Manager.Count_retry_u8 = 0;
                }
            } else {
                sFROM_Manager.Count_retry_u8--;
            }
        } else {
            sFROM_Manager.Count_retry_u8--;
        }
    } while (sFROM_Manager.Count_retry_u8 > 0);

    if (0U == reVal) {
        sFROM_Manager.status_u8 = 1U;
    } else {
        sFROM_Manager.status_u8 = 0;
        sFROM_Manager.Count_err_u8 = 0;
    }
    
    return reVal;
}



/* return 1 is SUCCESS or 0 is error */
uint8_t CAT24Mxx_Read_Array(uint32_t readAddr, uint8_t *dataBuff, uint16_t dataLength)
{
    uint8_t reVal       = 0U, bank = 0;
    uint8_t addrA16bit  = 0U;
    uint16_t addr = 0;
    
    sFROM_Manager.Count_retry_u8 = NUMBER_RETRY_R_W;
    
    bank = readAddr / 0xFFFF;
    addr = (uint16_t) readAddr;  
    addrA16bit = I2C_CAT24Mxx_ADDR_7BIT | (bank << 1);

    do
    {
        if (HAL_I2C_Mem_Read(&I2C_CAT24, addrA16bit, addr, I2C_MEMADD_SIZE_16BIT, dataBuff, dataLength, 1000) == HAL_OK)
        {
            sFROM_Manager.status_u8 = 0U;
            sFROM_Manager.Count_retry_u8 = 0U;
            sFROM_Manager.Count_err_u8 = 0U;
            reVal = 1U;
        } else {
            sFROM_Manager.Count_retry_u8--;
            sFROM_Manager.status_u8 = 1U; /* ERROR notif */
        }
    } while (sFROM_Manager.Count_retry_u8 > 0);

    return reVal;
}



int32_t TestEEPROM(void)
{
    uint16_t i = 0;
    uint32_t  addr_start = 0;
    char aTEMP[CAT_PAGE_SIZE - 5] = {0};

    for (i = CAT24_START_LOG; i < CAT24_END_LOG / CAT_PAGE_SIZE; i++)
    {
        HAL_Delay(2);
        UTIL_MEM_set(aTEMP, 0, sizeof(aTEMP));
        sprintf(aTEMP, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890 Ban tin test so %d \r\n", i);
             
        if (CAT24Mxx_Write_Buff(addr_start + i * CAT_PAGE_SIZE, (uint8_t *) aTEMP, CAT_PAGE_SIZE) != 1) {
            return i + 1;
        }
    }

    HAL_Delay(2000);
    
    //doc ra
    for (i = CAT24_START_LOG; i < CAT24_END_LOG / CAT_PAGE_SIZE; i++)
    {
        HAL_Delay(2);
        UTIL_MEM_set(aTEMP, 0, sizeof(aTEMP));
        if (CAT24Mxx_Read_Array(addr_start + i * CAT_PAGE_SIZE, (uint8_t *) aTEMP, CAT_PAGE_SIZE) != 1) {
            return i+1;
        }
        
        pFunc_UTIL_Debug_Send((uint8_t *) aTEMP , strlen(aTEMP));
    }
    
    UTIL_MEM_set(aTEMP, 0xFF, sizeof(aTEMP));

    for (i = CAT24_START_LOG; i < CAT24_END_LOG / CAT_PAGE_SIZE; i++)
    {
        HAL_Delay(2);
        if (CAT24Mxx_Write_Buff(addr_start + i * CAT_PAGE_SIZE, (uint8_t *) aTEMP, CAT_PAGE_SIZE) != 1) {
            return i+1;
        }
    }

    if (sFROM_Manager.status_u8 == 1) {
        return -1; // loi
    }
    
    return 0;
}


/* */
uint8_t CAT24Mxx_Erase(void)
{
    uint8_t Data[CAT_PAGE_SIZE] = {0};
    uint16_t i = 0;
  
    /* Clean all */
    for (i = 0; i < CAT_PAGE_SIZE; i++)
        Data[i] = 0xFF;
    
    for (i = CAT24_START_LOG; i < CAT24_END_LOG / CAT_PAGE_SIZE; i++)
    {
        HAL_Delay(10);
        if (CAT24Mxx_Write_Buff(i * CAT_PAGE_SIZE, &Data[0], CAT_PAGE_SIZE) != 1)
            return i+1;
    }
    
    return 0;
}
/* This function use to write a byte */
uint8_t CAT24Mxx_Write_Byte(uint32_t writeAddr, uint8_t *dataBuff)
{
    return CAT24Mxx_Write_Buff (writeAddr,dataBuff,1);
}
/* This function uses to write a word */
uint8_t CAT24Mxx_Write_Word(uint32_t writeAddr, uint16_t *dataBuff)
{
    return CAT24Mxx_Write_Buff (writeAddr,(uint8_t *)dataBuff,2);
}
/* This function uses to write 4 word */
uint8_t CAT24Mxx_Write_DoubleWord(uint32_t writeAddr, uint32_t *dataBuff)
{
    return CAT24Mxx_Write_Buff (writeAddr,(uint8_t *)dataBuff,4);
}
/* This function uses to write 8 word */
uint8_t CAT24Mxx_Write_QuadWord(uint32_t writeAddr, uint64_t *dataBuff)
{
    return CAT24Mxx_Write_Buff (writeAddr,(uint8_t *)dataBuff,8);
}
/* Read 1 byte from the EEROM memory */
uint8_t CAT24Mxx_Read_Byte(uint32_t writeAddr, uint8_t *dataBuff)
{
    return CAT24Mxx_Read_Array (writeAddr,dataBuff,1);
}
/* Read 2 bytes from the EEROM memory */
uint8_t CAT24Mxx_Read_Word(uint32_t writeAddr, uint16_t *dataBuff)
{
    return CAT24Mxx_Read_Array (writeAddr,(uint8_t *)dataBuff,2);
}
/* Read 4 bytes from the EEROM memory */
uint8_t CAT24Mxx_Read_DoubleWord(uint32_t writeAddr, uint32_t *dataBuff)
{
    return CAT24Mxx_Read_Array (writeAddr,(uint8_t *)dataBuff,4);
}
/* Read 8 bytes from the EEROM memory */
uint8_t CAT24Mxx_Read_QuadWord(uint32_t writeAddr, uint64_t *dataBuff)
{
    return CAT24Mxx_Read_Array (writeAddr,(uint8_t *)dataBuff,8);
}
/* This function uses to check the CAT24 busy or idle */
uint8_t CAT24Mxx_Check_Busy(uint8_t  addrA16bit, uint8_t Retry, uint8_t Timeout)
{
    if (HAL_I2C_IsDeviceReady(&I2C_CAT24, addrA16bit, Retry, Timeout) == HAL_OK)
    {
        return 1;
    }
    else
    {
        sFROM_Manager.Count_err_u8++;/* save error memory */
        return 0;
    }
}


/*------------ Func Callback --------------*/
static uint8_t _Cb_CAT24Mxx_Write (uint8_t event)
{
    static uint8_t status = false;
    
    if (CAT24Mxx_Write_Buff(sCAT24Var.sHWrite.Addr_u32, sCAT24Var.sHWrite.aData, sCAT24Var.sHWrite.Length_u16) == false) {
        if (sCAT24Var.pMem_Wrtie_ERROR != NULL) {
            sCAT24Var.pMem_Wrtie_ERROR(sCAT24Var.sHWrite.Addr_u32, &sCAT24Var.sHWrite.aData[0], sCAT24Var.sHWrite.Length_u16);
        }
    }
         
    //Write OK: tang index save
    if (sCAT24Var.pMem_Write_OK != NULL)
        sCAT24Var.pMem_Write_OK(sCAT24Var.sHWrite.Addr_u32);
    
    if (status == false) {
        status = true;
        UTIL_Printf_Str( DBLEVEL_M, "u_ex_eeprom: write eeprom ok!\r\n" ); 
    }
    
    return 1;
}


static uint8_t _Cb_CAT24Mxx_Read (uint8_t event)
{
    if (CAT24Mxx_Read_Array(sCAT24Var.sHRead.Addr_u32, aDATA_CAT24_READ, sCAT24Var.sHRead.Length_u16) != 1)
    {
        //Set EEPROM Loi and check again
        UTIL_Printf_Str( DBLEVEL_M, "u_ex_eeprom: read eeprom fail!\r\n" );  
        
        if (sCAT24Var.pMem_Read_Fail != NULL)
            sCAT24Var.pMem_Read_Fail (sCAT24Var.sHRead.Addr_u32);
        
        return 0;
    }
        
    if (sCAT24Var.pMem_Read_OK != NULL)
        sCAT24Var.pMem_Read_OK(sCAT24Var.sHRead.Kind_u8, sCAT24Var.sHRead.Addr_u32, aDATA_CAT24_READ, sCAT24Var.sHRead.Length_u16);
    
    return 1;
}


/*------------ Func Handle ------------*/

void CAT24Mxx_Init (void)
{

}


uint8_t CAT24Mxx_Task (void)
{
    uint8_t i = 0;
	uint8_t Result = false;

	for (i = 0; i < _EVENT_CAT24Mxx_END; i++)
	{
		if (sEventCAT24[i].e_status == 1)
		{
            Result = true;
            
			if ((sEventCAT24[i].e_systick == 0) ||
					((HAL_GetTick() - sEventCAT24[i].e_systick)  >=  sEventCAT24[i].e_period))
			{
                sEventCAT24[i].e_status = 0; 
				sEventCAT24[i].e_systick = HAL_GetTick();
				sEventCAT24[i].e_function_handler(i);
			}
		}
	}

	return Result;
}
