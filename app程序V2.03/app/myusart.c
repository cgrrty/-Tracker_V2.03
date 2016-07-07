#include "stm32f10x_lib.h"
#include "string.h"
#include "my32x.h"
#include "ini.h"
#include "myusart.h"
#include "mym10.h"

myUsart1_TypeDef myUsart1;
myUsart2_TypeDef myUsart2;

//////////////////////////////////串口1初始化///////////////////////////////////
/* 串口GPIO初始化*/
void myUsart1_GPIO_Cmd(FunctionalState NewState)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if(NewState != DISABLE)
  {
    // Configure USART1 Tx as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // Configure USART1 Rx as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  else
  {
    // Configure USART1 Tx/Rx as AIN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
}

void myUsart1_Init(u16 BaudRate)
{ 
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1,&USART_InitStructure);
  
  myUsart1_GPIO_Cmd(ENABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}
//从发送缓存区发送一个字节
void myUsart1_Pop()
{
  u16 temp16=0;
  
  temp16 = myUsart1.TxBuf_PushPos;
  
  if(myUsart1.TxBuf_PopPos != temp16)
  {
    myUsart1.Timer_Count = 0;
    USART_SendData(USART1,myUsart1.TxBuf[myUsart1.TxBuf_PopPos]);
    myUsart1.TxBuf_PopPos++;
    if(myUsart1.TxBuf_PopPos >= myUsart1_Tx_BufLen) myUsart1.TxBuf_PopPos = 0;
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  }
  else
  {
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  }
}
//向发送缓存区写入一个字节
void myUsart1_Push_Byte(u8 i)
{
  u16 temp16=0;
  
  temp16 = myUsart1.TxBuf_PopPos;
  
  if(myUsart1.TxBuf_PushPos >= temp16)
  {
    myUsart1.TxBuf[myUsart1.TxBuf_PushPos] = i;
    myUsart1.TxBuf_PushPos++;
    if(myUsart1.TxBuf_PushPos >= myUsart1_Tx_BufLen) myUsart1.TxBuf_PushPos = 0;
  }
  else if((temp16-myUsart1.TxBuf_PushPos) > 1)
  {
     myUsart1.TxBuf[myUsart1.TxBuf_PushPos] = i;
     myUsart1.TxBuf_PushPos++;
  }
}
//向发送缓存区写入一个字符串
void myUsart1_Push_String(u8 *String)
{
  u8 *Data = String;
  
  while(*Data != 0)
  {
    myUsart1_Push_Byte(*Data);
    Data++;
  }
}

//向发送缓存区写入一个数字
void myUsart1_Push_Num(u32 n,u8 BCDorHEX)
{
  u8 i=0,temp_flag=FALSE;
  u8 temp_buf[11];
  memset((u8 *)temp_buf,0,11);
  
  if (n==0)
  {
    myUsart1_Push_Byte('0');
    return;
  }
  
  temp_flag=FALSE;
  
  if (BCDorHEX==16)
  {
     hextostr4(n,(u8 *)temp_buf);
    for (i=0;i<8;i++)
    {
      if (temp_flag)
      myUsart1_Push_Byte(temp_buf[i]);
      else if (temp_buf[i]!='0')
      {
	 temp_flag=TRUE;
	 myUsart1_Push_Byte(temp_buf[i]);
      }
    }
    return;
  }
  
  if (BCDorHEX==10)
  {
    bcdtostr4(n,(u8 *)temp_buf);
    for (i=0;i<10;i++)
    {
       if (temp_flag)
	 myUsart1_Push_Byte(temp_buf[i]);
       else if (temp_buf[i]!='0')
       {
	 temp_flag=TRUE;
	 myUsart1_Push_Byte(temp_buf[i]);
       }
    }
    return;
  }
}
//向发送缓存区写一个块
void myUsart1_Push_Block(u8 *pbuf,u16 len)
{
  u16 i16=0;
  
  for(i16=0;i16<len;i16++) myUsart1_Push_Byte(pbuf[i16]);
}
//////////////////////////////////串口2初始化///////////////////////////////////
/* 串口GPIO初始化*/
void myUsart2_GPIO_Cmd(FunctionalState NewState)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  if(NewState != DISABLE)
  {
    // Configure USART2 Tx as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // Configure USART2 Rx as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
  else
  {
    // Configure USART2 Tx/Rx as AIN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
}

void myUsart2_Init(u16 BaudRate)
{ 
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2,&USART_InitStructure);
  
  myUsart2_GPIO_Cmd(ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART2, ENABLE);
}
//从发送缓存区发送一个字节
void myUsart2_Pop()
{
  u16 temp16=0;
  
  temp16 = myUsart2.TxBuf_PushPos;
  
  if(myUsart2.TxBuf_PopPos != temp16)
  {
    myUsart2.Timer_Count = 0;
    USART_SendData(USART2,myUsart2.TxBuf[myUsart2.TxBuf_PopPos]);
    myUsart2.TxBuf_PopPos++;
    if(myUsart2.TxBuf_PopPos >= myUsart2_Tx_BufLen) myUsart2.TxBuf_PopPos = 0;
    USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
  }
  else
  {
    USART_ITConfig(USART2,USART_IT_TXE, DISABLE);
  }
}
//向发送缓存区写入一个字节
void myUsart2_Push_Byte(u8 i)
{
  u16 temp16=0;
  
  temp16 = myUsart2.TxBuf_PopPos;
  
  if(myUsart2.TxBuf_PushPos >= temp16)
  {
    myUsart2.TxBuf[myUsart2.TxBuf_PushPos] = i;
    myUsart2.TxBuf_PushPos++;
    if(myUsart2.TxBuf_PushPos >= myUsart2_Tx_BufLen) myUsart2.TxBuf_PushPos = 0;
  }
  else if((temp16-myUsart2.TxBuf_PushPos)>1)
  {
     myUsart2.TxBuf[myUsart2.TxBuf_PushPos] = i;
     myUsart2.TxBuf_PushPos++;
  }
}
//向发送缓存区写入一个字符串
void myUsart2_Push_String(u8 *String)
{
  u8 *Data = String;
  
  while(*Data != 0)
  {
    myUsart2_Push_Byte(*Data);
    Data++;
  }
}

//向发送缓存区写入一个数字
void myUsart2_Push_Num(u32 n,u8 BCDorHEX)
{
  u8 i=0,temp_flag=FALSE;
  u8 temp_buf[11];
  memset((u8 *)temp_buf,0,11);
  
  if (n==0)
  {
    myUsart2_Push_Byte('0');
    return;
  }
  
  temp_flag=FALSE;
  
  if (BCDorHEX==16)
  {
     hextostr4(n,(u8 *)temp_buf);
    for (i=0;i<8;i++)
    {
      if (temp_flag)
      myUsart2_Push_Byte(temp_buf[i]);
      else if (temp_buf[i]!='0')
      {
	 temp_flag=TRUE;
	 myUsart2_Push_Byte(temp_buf[i]);
      }
    }
    return;
  }
  
  if (BCDorHEX==10)
  {
    bcdtostr4(n,(u8 *)temp_buf);
    for (i=0;i<10;i++)
    {
       if (temp_flag)
	 myUsart2_Push_Byte(temp_buf[i]);
       else if (temp_buf[i]!='0')
       {
	 temp_flag=TRUE;
	 myUsart2_Push_Byte(temp_buf[i]);
       }
    }
    return;
  }
}
//向发送缓存区写一个块
void myUsart2_Push_Block(u8 *pbuf,u16 len)
{
  u16 i16=0;
  
  for(i16=0;i16<len;i16++) myUsart2_Push_Byte(pbuf[i16]);
}


////////////////////处理串口GSM/////////////////////////////
void myUsartGSM_Event()
{
}
////////////////////处理串口GPS/////////////////////////////
void myUsartGPS_Event(void)
{ 
  myUsartGPS.RxBuf[0] = 'G';
  myUsartGPS.RxBuf[1] = 'P';
  myUsartGPS.RxBuf[2] = 'R';
  myUsartGPS.RxBuf[3] = 'M';
  myUsartGPS.RxBuf[4] = 'C';

  if(Disposal_GpsGSM((u8 *)myUsartGPS.RxBuf,myUsartGPS.RXDPos) == TRUE)//提取GPS有效数据成功
  {
    GPSmode_Worke_Flag = FALSE;     //关闭GPS
    SMS_GPSdata();
    SMS_Buf_Flag = TRUE;
    LED1_Flash(2);
  }
  
}

void USART_GSM_INT(void)
{
  u8 temp_Rx=0;
  
  if(USART_GetITStatus(USARTGSM, USART_IT_TXE) == SET)
  {
     myUsartGSM_Pop();
  }
  if(USART_GetITStatus(USARTGSM, USART_IT_RXNE) == SET)
  {
     temp_Rx = USART_ReceiveData(USARTGSM);      
     USART_ClearFlag(USARTGSM,USART_FLAG_RXNE);
     USART_ClearITPendingBit(USARTGSM,USART_IT_RXNE);
     
     myUsartGSM.CmdBuf0 = myUsartGSM.CmdBuf1;
     myUsartGSM.CmdBuf1 = myUsartGSM.CmdBuf2;
     myUsartGSM.CmdBuf2 = myUsartGSM.CmdBuf3;
     myUsartGSM.CmdBuf3 = myUsartGSM.CmdBuf4;
     myUsartGSM.CmdBuf4 = myUsartGSM.CmdBuf5;
     myUsartGSM.CmdBuf5 = myUsartGSM.CmdBuf6;
     myUsartGSM.CmdBuf6 = myUsartGSM.CmdBuf7;
     myUsartGSM.CmdBuf7 = myUsartGSM.CmdBuf8;
     myUsartGSM.CmdBuf8 = myUsartGSM.CmdBuf9;
     myUsartGSM.CmdBuf9 = myUsartGSM.CmdBuf10;
     myUsartGSM.CmdBuf10 = myUsartGSM.CmdBuf11;
     myUsartGSM.CmdBuf11 = myUsartGSM.CmdBuf12;
     myUsartGSM.CmdBuf12 = myUsartGSM.CmdBuf13;
     myUsartGSM.CmdBuf13 = myUsartGSM.CmdBuf14;
     myUsartGSM.CmdBuf14 = myUsartGSM.CmdBuf15;
     myUsartGSM.CmdBuf15 = myUsartGSM.CmdBuf16;
     myUsartGSM.CmdBuf16 = myUsartGSM.CmdBuf17;
     myUsartGSM.CmdBuf17 = myUsartGSM.CmdBuf18;
     myUsartGSM.CmdBuf18 = myUsartGSM.CmdBuf19;
     myUsartGSM.CmdBuf19 = myUsartGSM.CmdBuf20;
     myUsartGSM.CmdBuf20 = myUsartGSM.CmdBuf21;
     myUsartGSM.CmdBuf21 = temp_Rx;
     
     if(temp_Rx == '>') GSM_CipSend_Flag = TRUE;
     if ((myUsartGSM.CmdBuf18 == 'O') && (myUsartGSM.CmdBuf19 == 'K'))
     {
       AT_ComOk_Flag = TRUE;
     }
     if((myUsartGSM.CmdBuf14==' ') && (myUsartGSM.CmdBuf15=='R') && (myUsartGSM.CmdBuf16=='e') 
     && (myUsartGSM.CmdBuf17=='a') && (myUsartGSM.CmdBuf18=='d') && (myUsartGSM.CmdBuf19=='y'))
     {
       Sim_CallReady_Flag = TRUE;
     }
     if((myUsartGSM.CmdBuf11=='C') && (myUsartGSM.CmdBuf12=='R') && (myUsartGSM.CmdBuf13=='E')
     && (myUsartGSM.CmdBuf14=='G') && (myUsartGSM.CmdBuf15==':') && (myUsartGSM.CmdBuf19=='1'))
     {
       NetWork_Flag = TRUE; //本地注册成功
     }
     if((myUsartGSM.CmdBuf11=='C') && (myUsartGSM.CmdBuf12=='R') && (myUsartGSM.CmdBuf13=='E')
     && (myUsartGSM.CmdBuf14=='G') && (myUsartGSM.CmdBuf15==':') && (myUsartGSM.CmdBuf19=='5')) 
     {
       NetWork_Flag = TRUE; //漫游注册成功
     }
     if((myUsartGSM.CmdBuf0=='+') && (myUsartGSM.CmdBuf1=='C') && (myUsartGSM.CmdBuf2=='M')
     && (myUsartGSM.CmdBuf3=='T') && (myUsartGSM.CmdBuf4==':') && (myUsartGSM.CmdBuf5==' ')
     && (myUsartGSM.CmdBuf6=='"') && (myUsartGSM.CmdBuf18=='"'))
     {
       Device_Info.Phone_Num[0] = myUsartGSM.CmdBuf7;
       Device_Info.Phone_Num[1] = myUsartGSM.CmdBuf8;
       Device_Info.Phone_Num[2] = myUsartGSM.CmdBuf9;
       Device_Info.Phone_Num[3] = myUsartGSM.CmdBuf10;
       Device_Info.Phone_Num[4] = myUsartGSM.CmdBuf11;
       Device_Info.Phone_Num[5] = myUsartGSM.CmdBuf12;
       Device_Info.Phone_Num[6] = myUsartGSM.CmdBuf13;
       Device_Info.Phone_Num[7] = myUsartGSM.CmdBuf14;
       Device_Info.Phone_Num[8] = myUsartGSM.CmdBuf15;
       Device_Info.Phone_Num[9] = myUsartGSM.CmdBuf16;
       Device_Info.Phone_Num[10] = myUsartGSM.CmdBuf17;
       Device_Info.Phone_Num[11] = 0;
       Phone_Num_Flag = TRUE;
       SMS_Readstart_Flag = TRUE;
     }
     if((myUsartGSM.CmdBuf0=='+') && (myUsartGSM.CmdBuf1=='C') && (myUsartGSM.CmdBuf2=='M')
     && (myUsartGSM.CmdBuf3=='T') && (myUsartGSM.CmdBuf4==':') && (myUsartGSM.CmdBuf5==' ')
     && (myUsartGSM.CmdBuf6=='"') && (myUsartGSM.CmdBuf21=='"')&& (myUsartGSM.CmdBuf7=='+'))
     {
       Device_Info.Phone_Num[0] = myUsartGSM.CmdBuf10;
       Device_Info.Phone_Num[1] = myUsartGSM.CmdBuf11;
       Device_Info.Phone_Num[2] = myUsartGSM.CmdBuf12;
       Device_Info.Phone_Num[3] = myUsartGSM.CmdBuf13;
       Device_Info.Phone_Num[4] = myUsartGSM.CmdBuf14;
       Device_Info.Phone_Num[5] = myUsartGSM.CmdBuf15;
       Device_Info.Phone_Num[6] = myUsartGSM.CmdBuf16;
       Device_Info.Phone_Num[7] = myUsartGSM.CmdBuf17;
       Device_Info.Phone_Num[8] = myUsartGSM.CmdBuf18;
       Device_Info.Phone_Num[9] = myUsartGSM.CmdBuf19;
       Device_Info.Phone_Num[10] = myUsartGSM.CmdBuf20;
       Device_Info.Phone_Num[11] = 0;
       Phone_Num_Flag = TRUE;
       SMS_Readstart_Flag = TRUE;
     }
     
     if(SMS_Readstart_Flag == FALSE) return;
     if(ReciveSMS_Flag) return;
     
     if(temp_Rx == 0x0D)
     {
       if(++SMS_Enter_Count == 2)
       {
         SMS_Enter_Count = 0;
         SMS_Readstart_Flag = FALSE;
         myUsartGSM.Head_Flag = FALSE;
         ReciveSMS_Flag = TRUE;
       }
     }
     else if(temp_Rx == 0x0A)
     {
       myUsartGSM.RXDPos = 0;
       myUsartGSM.Head_Flag = TRUE;
     }
     else if(myUsartGSM.Head_Flag)
     {
       if (myUsartGSM.RXDPos < myUsartGSM_Rx_BufLen) 
       {
         myUsartGSM.RxBuf[myUsartGSM.RXDPos] = temp_Rx;
         myUsartGSM.RXDPos++;
       }
       else
       {
         myUsartGSM.Head_Flag = FALSE;
         SMS_Enter_Count = 0;
       }
     }
  }
}

void USART_GPS_INT(void)
{    
  if(USART_GetITStatus(USARTGPS, USART_IT_TXE) == SET)
  {
     myUsartGPS_Pop();
  }
  if(USART_GetITStatus(USARTGPS, USART_IT_RXNE) == SET)
  {
     u8 temp_Rx = 0;
     
     temp_Rx = USART_ReceiveData(USARTGPS);
     USART_ClearFlag(USARTGPS,USART_FLAG_RXNE);
     USART_ClearITPendingBit(USARTGPS,USART_IT_RXNE);
     
     myUsartGPS.CmdBuf0 = myUsartGPS.CmdBuf1;
     myUsartGPS.CmdBuf1 = myUsartGPS.CmdBuf2;
     myUsartGPS.CmdBuf2 = myUsartGPS.CmdBuf3;
     myUsartGPS.CmdBuf3 = myUsartGPS.CmdBuf4;
     myUsartGPS.CmdBuf4 = temp_Rx;
     
     if(myUsartGPS.Event_Flag) return;
     if ((myUsartGPS.CmdBuf0=='G') && (myUsartGPS.CmdBuf1=='P') && (myUsartGPS.CmdBuf2=='R')
         && (myUsartGPS.CmdBuf3=='M') && (myUsartGPS.CmdBuf4=='C'))
     {
       myUsartGPS.RXDPos = 5;
       myUsartGPS.Head_Flag = TRUE;
     }
     else if(temp_Rx == 0x0d)
     {
       if(myUsartGPS.Head_Flag)
       {
         myUsartGPS.RxBuf[myUsartGPS.RXDPos] = 0;
         myUsartGPS.Head_Flag = FALSE;
         myUsartGPS.Event_Flag = TRUE;
       }
     }
     else
     {
       if(myUsartGPS.Head_Flag)
       {
          if(myUsartGPS.RXDPos < myUsartGPS_Rx_BufLen) 
          {
             myUsartGPS.RxBuf[myUsartGPS.RXDPos] = temp_Rx;
             myUsartGPS.RXDPos++;
          }
          else myUsartGPS.Head_Flag = FALSE;
       }
     }
  }
}
