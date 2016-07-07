#ifndef myusart_h
  #define myusart_h


#define  USARTGSM                USART1
#define  myUsartGSM              myUsart1
#define  myUsartGSM_GPIO_Cmd     myUsart1_GPIO_Cmd
#define  myUsartGSM_Init         myUsart1_Init
#define  myUsartGSM_Pop          myUsart1_Pop
#define  myUsartGSM_Push_Byte    myUsart1_Push_Byte
#define  myUsartGSM_Push_String  myUsart1_Push_String
#define  myUsartGSM_Push_Num     myUsart1_Push_Num
#define  myUsartGSM_Push_Block   myUsart1_Push_Block
#define  myUsartGSM_Event        myUsart1_Event
#define  myUsartGSM_Rx_BufLen    myUsart1_Rx_BufLen
#define  myUsartGSM_Tx_BufLen    myUsart1_Tx_BufLen
#define  myUsart1_Rx_BufLen      128
#define  myUsart1_Tx_BufLen      256
#define  myUsartGSM_Timer_Max    1

#define  USARTGPS                USART2
#define  myUsartGPS              myUsart2
#define  myUsartGPS_GPIO_Cmd     myUsart2_GPIO_Cmd
#define  myUsartGPS_Init         myUsart2_Init
#define  myUsartGPS_Pop          myUsart2_Pop
#define  myUsartGPS_Push_Byte    myUsart2_Push_Byte
#define  myUsartGPS_Push_String  myUsart2_Push_String
#define  myUsartGPS_Push_Num     myUsart2_Push_Num
#define  myUsartGPS_Push_Block   myUsart2_Push_Block
#define  myUsartGPS_Event        myUsart2_Event
#define  myUsartGPS_Rx_BufLen    myUsart2_Rx_BufLen
#define  myUsartGPS_Tx_BufLen    myUsart2_Tx_BufLen
#define  myUsart2_Rx_BufLen      128
#define  myUsart2_Tx_BufLen      32
#define  myUsartGPS_Timer_Max    1

void USART_GSM_INT(void);
void USART_GPS_INT(void);


void myUsartGSM_Event(void);
void myUsartGPS_Event(void);


typedef volatile struct
{
  vu8  RxBuf[myUsart1_Rx_BufLen];
  vu8  TxBuf[myUsart1_Tx_BufLen];
  vu16 RXDPos;
  vu16 TXDPos;
  vu16 TXDLen;
  vu8  Event_Flag;
  vu8  Head_Flag;
  vu8  ChkSum;
  vu16 TxBuf_PushPos;
  vu16 TxBuf_PopPos;
  vu8  Timer_Count;
  vu8  CmdBuf0;
  vu8  CmdBuf1;
  vu8  CmdBuf2;
  vu8  CmdBuf3;
  vu8  CmdBuf4;
  vu8  CmdBuf5;
  vu8  CmdBuf6;
  vu8  CmdBuf7;
  vu8  CmdBuf8;
  vu8  CmdBuf9;
  vu8  CmdBuf10;
  vu8  CmdBuf11;
  vu8  CmdBuf12;
  vu8  CmdBuf13;
  vu8  CmdBuf14;
  vu8  CmdBuf15;
  vu8  CmdBuf16;
  vu8  CmdBuf17;
  vu8  CmdBuf18;
  vu8  CmdBuf19;
  vu8  CmdBuf20;
  vu8  CmdBuf21;
}myUsart1_TypeDef;
extern myUsart1_TypeDef myUsart1;

void myUsart1_GPIO_Cmd(FunctionalState NewState); /* 串口GPIO初始化*/
void myUsart1_Init(u16 BaudRate);                 /* 串口1初始化*/
void myUsart1_Pop(void);                          /* 从发送缓存区发送一个字节 */
void myUsart1_Push_Byte(u8 i);                    /* 向发送缓存区写入一个字节 */
void myUsart1_Push_String(u8 *String);            /* 向发送缓存区写入一个字符串 */
void myUsart1_Push_Num(u32 n,u8 BCDorHEX);        /* 向发送缓存区写入一个数字 */
void myUsart1_Push_Block(u8 *pbuf,u16 len);       /* 向发送缓存区写一个块 */



typedef volatile struct
{
  vu8  RxBuf[myUsart2_Rx_BufLen];
  vu8  TxBuf[myUsart2_Tx_BufLen];
  vu16 RXDPos;
  vu16 TXDPos;
  vu16 TXDLen;
  vu8  Event_Flag;
  vu8  Head_Flag;
  vu8  ChkSum;
  vu16 TxBuf_PushPos;
  vu16 TxBuf_PopPos;
  vu8  Timer_Count;
  vu8  CmdBuf0;
  vu8  CmdBuf1;
  vu8  CmdBuf2;
  vu8  CmdBuf3;
  vu8  CmdBuf4;
  vu8  CmdBuf5;
  vu8  CmdBuf6;
  vu8  CmdBuf7;
  vu8  CmdBuf8;
  vu8  CmdBuf9;
  vu8  CmdBuf10;
}myUsart2_TypeDef;
extern myUsart2_TypeDef myUsart2;

void myUsart2_GPIO_Cmd(FunctionalState NewState); /* 串口GPIO初始化*/
void myUsart2_Init(u16 BaudRate);                 /* 串口1初始化*/
void myUsart2_Pop(void);                          /* 从发送缓存区发送一个字节 */
void myUsart2_Push_Byte(u8 i);                    /* 向发送缓存区写入一个字节 */
void myUsart2_Push_String(u8 *String);            /* 向发送缓存区写入一个字符串 */
void myUsart2_Push_Num(u32 n,u8 BCDorHEX);        /* 向发送缓存区写入一个数字 */
void myUsart2_Push_Block(u8 *pbuf,u16 len);       /* 向发送缓存区写一个块 */

#endif
