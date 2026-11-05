#include "user_app_modb_m.h"
#include "user_modbus_rtu.h"
#include "user_internal_mem.h"
#include "user_rs485.h"
#include "user_app_wm.h"
#include "user_define.h"
#include "user_modem.h"
#include "user_sensor_saoviet.h"
#include "user_sensor_xylem.h"
#include "user_sensor_daruifuno.h"
#include "user_sensor_temp_sv.h"
/*=========================Fucntion Static=========================*/
static uint8_t fevent_modb_entry(uint8_t event);

static uint8_t fevent_modb_rs485_1_refresh(uint8_t event);
static uint8_t fevent_modb_rs485_2_refresh(uint8_t event);

static uint8_t fevent_modb_rs485_1_handle(uint8_t event);
static uint8_t fevent_modb_rs485_2_handle(uint8_t event);
static uint8_t fevent_modb_tcp_handle(uint8_t event);

static uint8_t fevent_call_cycle(uint8_t event);
/*==============================Struct=============================*/
sEvent_struct               sEventAppModb[]=
{
  {_EVENT_MODB_ENTRY,              0, 5, 30000,            fevent_modb_entry},            //Doi slave khoi dong moi truyen opera
  
  {_EVENT_MODB_RS485_1_REFRESH,    0, 5, 60000,            fevent_modb_rs485_1_refresh},
  {_EVENT_MODB_RS485_2_REFRESH,    0, 5, 60000,            fevent_modb_rs485_2_refresh},
  
  {_EVENT_MODB_RS485_1_HANDLE,     0, 5, 100,              fevent_modb_rs485_1_handle},
  {_EVENT_MODB_RS485_2_HANDLE,     0, 5, 100,              fevent_modb_rs485_2_handle},
  {_EVENT_MODB_TCP_HANDLE,         0, 5, 100,              fevent_modb_tcp_handle},
  
  {_EVENT_CALL_CYCLE,              0, 5, 100,              fevent_call_cycle},
};
uint16_t CountBufferHandleRecv = 0;

static uint8_t aDATA_RECV_MODBUS_TCP [2048];
sData   sDataRecvTCP = {(uint8_t *) &aDATA_RECV_MODBUS_TCP[0], 0};

extern sData sUart485;
//extern sData sUart485_2;

Struct_TransModbusTCP       sTransModTCP = {0};

Struct_RegSensor            *sRegSensor_Port1;
Struct_RegSensor            *sRegSensor_Port2;
Struct_RegSensor            *sRegSensor_PortTCP;
/*========================Function Handle========================*/


void       RS485_Para_Init(void)
{
#ifdef MODBUS_SENSOR_SAOVIET
    sRegSensor_Port1 = sRegSS_SaoViet_Port1;
    sRegSensor_Port2 = sRegSS_SaoViet_Port2;
    RS485_SS_SaoViet_Init(sRegSensor_Port1, 0);
    RS485_SS_SaoViet_Init(sRegSensor_Port2, 1);
#elif defined(MODBUS_SENSOR_XYLEM)
    sRegSensor_PortTCP = sRegSS_Xylem_PortTCP;
    RS485_SS_XyLem_Init_TCP(sRegSensor_PortTCP, 0);
#elif defined(MODBUS_SENSOR_DARUIFUNO)
    sRegSensor_Port1 = sRegSS_Daruifuno_PortRTU;
    RS485_SS_Daruifuno_Init(sRegSensor_Port1, 0);
#elif defined(MODBUS_SENSOR_TEMPSV)
    sRegSensor_Port1 = sRegSS_TempSV_PortRTU;
    RS485_SS_TempSV_Init_RTU(sRegSensor_Port1, 0);
#else

#endif
}

static uint8_t fevent_modb_entry(uint8_t event)
{
//    fevent_enable(sEventAppRs485, _EVENT_RS485_TRANSMIT);
//    fevent_enable(sEventAppModb, _EVENT_MODB_RS485_1_REFRESH);
//    fevent_enable(sEventAppModb, _EVENT_MODB_RS485_2_REFRESH);
    return 1;
}

static uint8_t fevent_modb_rs485_1_refresh(uint8_t event)
{
//    Init_UartRs485();
//    fevent_enable(sEventAppModb, event);
    return 1;
}

static uint8_t fevent_modb_rs485_2_refresh(uint8_t event)
{
//    Init_UartRs485_2();
//    fevent_enable(sEventAppModb, event);
    return 1;
}

static uint8_t fevent_modb_rs485_1_handle(uint8_t event)
{
    static uint8_t step = 0;
    static Struct_CtrlModbM     sCtrlModbM = {0};
    uint8_t aFrame[48] = {0};
    sData   strFrame = {(uint8_t *) &aFrame[0], 0};
    
    switch(step)
    {
        case 0:
            if(Handle_Trans_Modb(sRegSensor_Port1, &sCtrlModbM, &strFrame) == 1)
            {
                RS485_1_Trans(strFrame.Data_a8, strFrame.Length_u16);
                sEventAppModb[_EVENT_MODB_RS485_1_HANDLE].e_period = TIMEOUT_MODB_RTU;
                step = 1;
            }
            break;
          
        case 1:
            if(Handle_Recv_Modb(_PORT_RS485_1, sRegSensor_Port1, &sCtrlModbM, sUart485) >= 2)
            {
                step = 0;
                fevent_active(sEventAppModb, _EVENT_CALL_CYCLE);
                return 1;
            }
            else if(Handle_Recv_Modb(_PORT_RS485_1, sRegSensor_Port1, &sCtrlModbM, sUart485) == 0)
            {
                Init_UartRs485();
            }
            else
            {

            }
            
            sEventAppModb[_EVENT_MODB_RS485_1_HANDLE].e_period = 100;
            step = 0;
            break;
          
        default:
            break;
    }

    fevent_enable(sEventAppModb, event);
    return 1;
}

static uint8_t fevent_modb_rs485_2_handle(uint8_t event)
{
//    static uint8_t step = 0;
//    static Struct_CtrlModbM     sCtrlModbM = {0};
//    uint8_t aFrame[48] = {0};
//    sData   strFrame = {(uint8_t *) &aFrame[0], 0};
//    
//    switch(step)
//    {
//        case 0:
//            if(Handle_Trans_Modb(sRegSensor_Port2, &sCtrlModbM, &strFrame) == 1)
//            {
//                RS485_2_Trans(strFrame.Data_a8, strFrame.Length_u16);
//                sEventAppModb[_EVENT_MODB_RS485_2_HANDLE].e_period = TIMEOUT_MODB_RTU;
//                step = 1;
//            }
//            break;
//          
//        case 1:
//            if(Handle_Recv_Modb(_PORT_RS485_2, sRegSensor_Port2, &sCtrlModbM, sUart485_2) == 1)
//                fevent_enable(sEventAppModb, _EVENT_MODB_RS485_2_REFRESH);
//            
//            sEventAppModb[_EVENT_MODB_RS485_2_HANDLE].e_period = 100;
//            step = 0;
//            break;
//          
//        default:
//            break;
//    }
//
//    fevent_enable(sEventAppModb, event);
    return 1;
}

static uint8_t fevent_modb_tcp_handle(uint8_t event)
{
//    static uint8_t step = 0;
//    static Struct_CtrlModbM     sCtrlModbM = {0};
//    uint8_t aFrame[48] = {0};
//    sData   strFrame = {(uint8_t *) &aFrame[0], 0};
//    
//    switch(step)
//    {
//        case 0:
//            if(Handle_Trans_Modb(sRegSensor_PortTCP, &sCtrlModbM, &strFrame) == 1)
//            {
//                memset(sTransModTCP.aData, 0xAA, sizeof(sTransModTCP.aData));
//                sTransModTCP.length = 0;
//                
//                sTransModTCP.aData[sTransModTCP.length++] = sCtrlModbM.Transaction_TCP >> 8;
//                sTransModTCP.aData[sTransModTCP.length++] = sCtrlModbM.Transaction_TCP;
//                sTransModTCP.aData[sTransModTCP.length++] = 0x00 >> 8;
//                sTransModTCP.aData[sTransModTCP.length++] = 0x00;
//                sTransModTCP.aData[sTransModTCP.length++] = (strFrame.Length_u16 - 2) >> 8;
//                sTransModTCP.aData[sTransModTCP.length++] = (strFrame.Length_u16 - 2);
//                
//                for(uint8_t i = 0; i < strFrame.Length_u16 - 2; i++)
//                    sTransModTCP.aData[sTransModTCP.length++] = strFrame.Data_a8[i];
//
//                Reset_Buff(&sDataRecvTCP);
//                sTransModTCP.Flag = TRUE;
//                
//                sEventAppModb[_EVENT_MODB_TCP_HANDLE].e_period = TIMEOUT_MODB_TCP;
//                step = 1;
//            }
//            break;
//          
//        case 1:
//            Handle_Recv_Modb(_PORT_ETH_TCP, sRegSensor_PortTCP, &sCtrlModbM, sDataRecvTCP);
//            
//            sEventAppModb[_EVENT_MODB_TCP_HANDLE].e_period = 100;
//            step = 0;
//            break;
//          
//        default:
//            break;
//    }
//
//    fevent_enable(sEventAppModb, event);
    return 1;
}

static uint8_t fevent_call_cycle(uint8_t event)
{
#ifdef MODBUS_SENSOR_SAOVIET
    Digital_SS_SaoViet_CallCycle();
#elif defined(MODBUS_SENSOR_XYLEM)
    Digital_SS_XyLem_CallCycle();
#elif defined(MODBUS_SENSOR_DARUIFUNO)
    Digital_SS_Daruifuno_CallCycle();
#elif defined(MODBUS_SENSOR_TEMPSV)
    Digital_SS_TempSV_CallCycle();
#else
     
#endif
    
    fevent_active(sEventAppSensor, _EVENT_SENSOR_DATA_MEASURE);
//    fevent_enable(sEventAppModb, event);
    return 1;
}
/*======================Modbus TCP Check====================*/
uint8_t ModbusTCP_Check_Format(uint8_t SlaveID, uint16_t nRegis,
                               sData *pSource, sData *Content)
{
    uint16_t Pos = 0;
    uint8_t ModFunc = 0, ModLength = 0, ModSlave = 0;
    uint16_t LengthField = 0;

    //MBAP + FUNC
    if (pSource->Length_u16 < 8)
        return false;

    // bo qua Transaction ID + Protocol ID
    Pos = 4;

    // lay Length field
    LengthField = (pSource->Data_a8[Pos] << 8) | pSource->Data_a8[Pos + 1];
    Pos += 2;

    // kiem tra do dai
    if (LengthField + 6 != pSource->Length_u16)
        return false;

    // Unit ID
    ModSlave = pSource->Data_a8[Pos++];

    // Function
    ModFunc = pSource->Data_a8[Pos++];
    
    if(ModSlave != SlaveID)
        return false;

    switch (ModFunc)
    {
        case FUN_READ_BYTE:
        case FUN_READ_REGIS:
            ModLength = pSource->Data_a8[Pos++];

            if (ModLength != (nRegis * 2))
                return false;

            // tro vao data
            Content->Data_a8 = &pSource->Data_a8[Pos];
            Content->Length_u16 = ModLength;
            break;

        case FUN_WRITE_BYTE:
        case FUN_WRITE_MULTI:
            break;
      
        default:
            return false;
            break;
    }

    return true;
}

uint8_t Modbus_RTU_Check_Format (uint8_t SlaveID, uint16_t nRegis,
                                   sData *pSource, sData *Content)
{
    uint16_t CrcCalcu = 0;
    uint8_t aCRC_GET[2] = {0};
    uint16_t Pos = 0;
    uint8_t ModFunc = 0, ModLength = 0, ModSlave = 0;
    
    if (pSource->Length_u16 < 4)
        return false;
    
    CrcCalcu = ModRTU_CRC(pSource->Data_a8, pSource->Length_u16 - 2);
            
    aCRC_GET[0] = (uint8_t) (CrcCalcu & 0x00FF);
    aCRC_GET[1] = (uint8_t) ( (CrcCalcu >> 8) & 0x00FF );
    
    if ( (aCRC_GET[0] != *(pSource->Data_a8 + pSource->Length_u16 - 2)) 
        || (aCRC_GET[1] != *(pSource->Data_a8 + pSource->Length_u16 - 1)) )
        return false;
            
    ModSlave = *(pSource->Data_a8 + Pos++);
    ModFunc = *(pSource->Data_a8 + Pos++);
    
    if(ModSlave != SlaveID)
        return false;
            
    switch (ModFunc)
    {
        case FUN_READ_BYTE:
        case FUN_READ_REGIS:
            ModLength = *(pSource->Data_a8 + Pos++);
            //check frame
            if (ModLength != (nRegis * 2))
                return false;
            
            //tro content vao data
            Content->Data_a8 = pSource->Data_a8 + Pos;
            Content->Length_u16 = ModLength;   
            break;
        case FUN_WRITE_BYTE:
        case FUN_WRITE_MULTI:
            break;
      
        default:
            return false;
            break;
    }
    
    return true;
}

/*==========================Handle Tran and Recv=========================*/
uint8_t Handle_Trans_Modb(Struct_RegSensor  sReg[], Struct_CtrlModbM  *sCtrl, sData *sFrame)
{
    uint32_t hex_Data = 0;
    uint16_t numKind = 0;
    if (!sReg || !sCtrl || !sFrame) return 0;

    while(sReg[numKind].Block != NULL && numKind < 1000)  //Gioi han doc 1000 thanh ghi
    {
        numKind++;
    }
    
    if(sCtrl->iHandle == numKind)
      sCtrl->iHandle = 0;
    
    while(1)
    {
        if(*sReg[sCtrl->iHandle].State != 0)
            break;
          
        if(sReg[sCtrl->iHandle].nConnect != NULL && *sReg[sCtrl->iHandle].State == 0)
            *sReg[sCtrl->iHandle].nConnect = 0;
        
        if(++sCtrl->iHandle == numKind) 
        {
            return 0;
        }
    }
    
    sCtrl->iStartBlock = sCtrl->iHandle;
    sCtrl->iEndBlock   = sCtrl->iHandle;
    sCtrl->iReg        = sReg[sCtrl->iHandle].cmdLen;
    
    if (sReg[sCtrl->iStartBlock].cmdFC == 0x06 || sReg[sCtrl->iStartBlock].cmdFC == 0x10)
    {
        uint8_t aData[50] = {0};
        uint8_t Length = 0;
        uint8_t i = sCtrl->iHandle;
        while(1)
        {
            if(sReg[i].vFormat == _ETYPE_F)
                hex_Data = Decode_Data_Type_f_to_u32(*(float*)sReg[i].subReg, sReg[i].vFormat);
            else 
                hex_Data = (uint32_t)sReg[i].subReg;

            hex_Data = Endian_Format(hex_Data, sReg[i].cmdLen * 2, sReg[i].vBeLe);

            for (uint8_t j = 0; j < sReg[i].cmdLen * 2; j++)
                aData[Length++] = (uint8_t)(hex_Data >> (8 * ((sReg[i].cmdLen * 2 - 1) - j)));
            
            i++;
            if((i < numKind) &&
               (sReg[i].Block == sReg[i-1].Block) && 
               (sReg[i].cmdAddr == sReg[i-1].cmdAddr + sReg[i-1].cmdLen))
            {
                sCtrl->iEndBlock = i;
                sCtrl->iReg = sReg[i].cmdAddr - sReg[sCtrl->iStartBlock].cmdAddr + sReg[i].cmdLen;
            }
            else
              break;

        }

        uint8_t fun_code = (sCtrl->iReg == 1) ? FUN_WRITE_BYTE : FUN_WRITE_MULTI;
        ModRTU_Master_Write_Frame(sFrame, *sReg[sCtrl->iStartBlock].idDev, fun_code, sReg[sCtrl->iStartBlock].cmdAddr, sCtrl->iReg, aData);
    }
    else
    {
        uint8_t i = sCtrl->iHandle;
        while ((i + 1) < numKind)
        {
            if (*sReg[i + 1].State != 0 &&
                sReg[i + 1].Block == sReg[i].Block && 
                sReg[i + 1].cmdAddr >= sReg[i].cmdAddr && 
                sReg[i + 1].idDev == sReg[i].idDev &&
                sReg[i + 1].cmdFC == sReg[i].cmdFC)
            {
                sCtrl->iEndBlock = i + 1;
                sCtrl->iReg = sReg[i + 1].cmdAddr - sReg[sCtrl->iStartBlock].cmdAddr + sReg[i + 1].cmdLen;
                i++; 
            }
            else 
              break;
        }
        ModRTU_Master_Read_Frame(sFrame, *sReg[sCtrl->iStartBlock].idDev, sReg[sCtrl->iStartBlock].cmdFC, sReg[sCtrl->iStartBlock].cmdAddr, sCtrl->iReg);
    }
    return 1;
}

uint8_t Handle_Recv_Modb(uint8_t Port, Struct_RegSensor  sReg[], Struct_CtrlModbM  *sCtrl, sData sRecv)
{
    uint8_t Result_Recv = false;
    uint32_t hex_Recv = 0;
    sData ModContent;
    uint16_t numKind = 0;
    
    if (!sReg || !sCtrl || !sReg[sCtrl->iHandle].idDev) return 0;

    if (Port == _PORT_ETH_TCP)
    {
        Result_Recv = ModbusTCP_Check_Format(*sReg[sCtrl->iHandle].idDev, sCtrl->iReg, &sRecv, &ModContent);
        
        sCtrl->Transaction_TCP++;
    }
    else
    {
        Result_Recv = Modbus_RTU_Check_Format(*sReg[sCtrl->iStartBlock].idDev, sCtrl->iReg, &sRecv, &ModContent);
    }
    
    while(sCtrl->iHandle <= sCtrl->iEndBlock)
    {
        if(Result_Recv)
        {     
            sCtrl->iAddr = sReg[sCtrl->iHandle].cmdAddr - sReg[sCtrl->iStartBlock].cmdAddr;
            if(sReg[sCtrl->iHandle].cmdFC == 0x03 || sReg[sCtrl->iHandle].cmdFC == 0x04)
            {
                if(sReg[sCtrl->iHandle].cmdLen == 2)
                {
                    hex_Recv =  ((uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 0] << 24) | 
                                ((uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 1] << 16) | 
                                ((uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 2] << 8)  | 
                                (uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 3];
                }
                else
                    hex_Recv =  ((uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 0] << 8) | 
                                (uint32_t)ModContent.Data_a8[sCtrl->iAddr*2 + 1];
                
                hex_Recv = Endian_Format(hex_Recv, sReg[sCtrl->iHandle].cmdLen*2, sReg[sCtrl->iHandle].vBeLe);
            }
            
            if(sReg[sCtrl->iHandle].vReturn != NULL)
            {
                switch (sReg[sCtrl->iHandle].vFormat)
                {
                    case _ETYPE_F:
                    {
                        float value;
                        value = Decode_Data_Type_u32_to_f(hex_Recv, sReg[sCtrl->iHandle].vFormat);
                        value *= sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn, &value, sizeof(float));
                        break;
                    }

                    case _ETYPE_U32:
                    {
                        uint32_t value;
                        value = hex_Recv;
                        value *= (uint32_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn,&value,sizeof(uint32_t));
                        break;
                    }

                    case _ETYPE_I32:
                    {
                        int32_t value;
                        value = (int32_t)hex_Recv;
                        value *= (int32_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn,&value,sizeof(int32_t));
                        break;
                    }

                    case _ETYPE_U16:
                    {
                        uint16_t value;
                        value = (uint16_t)hex_Recv;
                        value *= (uint16_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn, &value, sizeof(uint16_t));
                        break;
                    }

                    case _ETYPE_I16:
                    {
                        int16_t value;
                        value = (int16_t)hex_Recv;
                        value *= (int16_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn, &value, sizeof(int16_t));
                        break;
                    }

                    case _ETYPE_U8:
                    {
                        uint8_t value;
                        value = (uint8_t)hex_Recv;
                        value *= (uint8_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn, &value, sizeof(uint8_t));
                        break;
                    }

                    case _ETYPE_I8:
                    {
                        int8_t value;
                        value = (int8_t)hex_Recv;
                        value *= (int8_t)sReg[sCtrl->iHandle].vScale;
                        memcpy(sReg[sCtrl->iHandle].vReturn, &value, sizeof(int8_t));
                        break;
                    }

                    default:
                        break;
                }
            }
            
            if(*sReg[sCtrl->iHandle].nConnect < MAX_COUNT_DISCONNECT)
                *sReg[sCtrl->iHandle].nConnect +=1;
        }
        else
        {
            if(*sReg[sCtrl->iHandle].nConnect > 0)
                *sReg[sCtrl->iHandle].nConnect -=1;
        }
        sCtrl->iHandle++;
    }
    
    while(sReg[numKind].Block != NULL && numKind < 1000)  //Gioi han doc 1000 thanh ghi
    {
        numKind++;
    }
    
    if(Result_Recv == true)
    {
        if(sCtrl->iEndBlock >= (numKind - 1))
            return 3;
        else
            return 1;
    }
    else 
    {
        if(sCtrl->iEndBlock >= (numKind - 1))
            return 2;
        else
            return 0;
    }
}

void Config_RegSen_Read(Struct_RegSensor sRegSensor[],
                        uint8_t Kind,
                        uint8_t *ID,
                        uint8_t *User,
                        void *Return,
                        uint8_t *nConnect)
{
    sRegSensor[Kind].idDev = ID;
    sRegSensor[Kind].State = User;
    sRegSensor[Kind].vReturn = Return;
    sRegSensor[Kind].nConnect = nConnect;
}

void Config_RegSen_Write(Struct_RegSensor sRegSensor[],
                         uint8_t Kind,
                         uint8_t *ID, 
                         uint8_t *User, 
                         void *subReg, 
                         uint8_t *nConnect)
{
    sRegSensor[Kind].idDev = ID;
    sRegSensor[Kind].State = User;
    sRegSensor[Kind].subReg = subReg ;
    sRegSensor[Kind].nConnect = nConnect;
}

/*==========================Handle==========================*/
void        Init_Parameter_Sensor(void)
{

}

/*==================Handle Define AT command=================*/
#ifdef USING_AT_CONFIG

#endif

/*========================AT Command======================*/
void Modem_SER_Set_Mod_TCP_Main (sData *strRecei, uint16_t Pos)
{
//    sServerInfor sServerTemp = {0};
//
//    if (Modem_Extract_Server((char *) (strRecei->Data_a8 + Pos), &sServerTemp) == true) {
//        UTIL_MEM_set(&sModemInfor.sServerModTCP, 0, sizeof(sModemInfor.sServerModTCP));
//        UTIL_MEM_cpy(&sModemInfor.sServerModTCP, &sServerTemp, sizeof(sServerTemp));
//        sModemInfor.sServerModTCP.KeepAlive_u32 = 60;
//
//        //Convert lai IP       
//        if (UTIL_Convert_IP_To_Buff(sModemInfor.sServerModTCP.aIP, sModemInfor.sServerModTCP.IPnum) == true) {
//            sModemInfor.sServerModTCP.DomainOrIp_u8 = __SERVER_IP;
//        } else {
//            sModemInfor.sServerModTCP.DomainOrIp_u8 = __SERVER_DOMAIN;
//        }
//        sModemInfor.sServerModTCP.Port_u16 = (uint16_t) UtilStringToInt(sModemInfor.sServerModTCP.aPORT);
//        
//        Modem_Save_Var();
//        Modem_Respond_Str(PortConfig, "OK", 0);
//        return;
//    }
//        
//    Modem_Respond_Str(PortConfig, "ERROR", 0);
}


void Modem_SER_Get_Mod_TCP_Main (sData *strRecei, uint16_t Pos)
{
//    char aData[128] = {0};
//    
//    sprintf((char*) aData, "%s,%s,%s,%s\r\n", sModemInfor.sServerModTCP.aIP,
//                                        sModemInfor.sServerModTCP.aPORT,
//                                        sModemInfor.sServerModTCP.aUSER,
//                                        sModemInfor.sServerModTCP.aPASS);  
//    
//    Modem_Respond_Str(PortConfig, aData, 0);
}
/*==================Handle Task and Init app=================*/
void Init_UartRs485(void)
{
    RS485_Stop_RX_Mode();
    WM_DIG_Init_Uart(&uart_rs485, __MET_WOTECK);
    RS485_Init_RX_Mode();
}

void Init_UartRs485_2(void)
{
//    RS485_2_Stop_RX_Mode();
//    WM_DIG_Init_Uart(&uart_rs485_2, sWmDigVar.sModbInfor[0].MType_u8);
//    RS485_2_Init_RX_Mode();
}

void       Init_AppModb(void)
{
    Init_UartRs485();
    Init_UartRs485_2();
    Init_Parameter_Sensor();
#ifdef USING_AT_CONFIG
    /* regis cb serial */
//    sATCmdList[_GET_MOD_TCP_MAIN].CallBack = Modem_SER_Get_Mod_TCP_Main;
//    sATCmdList[_SET_MOD_TCP_MAIN].CallBack = Modem_SER_Set_Mod_TCP_Main;
#endif
    RS485_Para_Init();
}

uint8_t        AppModb_Task(void)
{
    uint8_t i = 0;
    uint8_t Result =  false;
    
    for(i = 0; i < _EVENT_MODB_END; i++)
    {
        if(sEventAppModb[i].e_status == 1)
        {
            Result = true; 
            
            if((sEventAppModb[i].e_systick == 0) ||
               ((HAL_GetTick() - sEventAppModb[i].e_systick) >= sEventAppModb[i].e_period))
            {
                sEventAppModb[i].e_status = 0; //Disable event
                sEventAppModb[i].e_systick= HAL_GetTick();
                sEventAppModb[i].e_function_handler(i);
            }
        }
    }
    
    return Result;
}



