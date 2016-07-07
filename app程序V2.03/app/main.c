#include "stm32f10x_lib.h"
#include "string.h"
#include "my32x.h"
#include "ini.h"
#include "myusart.h"
#include "mym10.h"

Gps_Info_TypeDef Gps_Info;

vu8 GPSmode_Worke_Flag = FALSE;
vu8 GSMmode_Worke_Flag = FALSE;
vu8 GPSmode_Sleep_Count = 0;
vu8 GSMmode_Sleep_Count = 0;
vu8 Timer_10ms_Count = 0;
vu8 Timer_1Sec_Flag = FALSE;
vu16 SysTick_Count = 0;
vu8 LED1_Flash_Flag = FALSE;
vu8 LED1_Flash_Count = 0;

void main(void)
{
  #ifdef DEBUG
    debug();
  #endif
    
  Init_Clock();
  System_Init();
  EXTI_Configuration();
  NVIC_Configuration();
  NVIC_RESETPRIMASK();//打开总中断
  
  GPS_Pow_OFF;
  LED1_ON;
  LED2_OFF;
  
  while(1)
  {
    //GPS
    if(GPSmode_Worke_Flag)
    {
      GPS_Pow_ON;
      myUsartGPS_GPIO_Cmd(ENABLE);
      if(myUsartGPS.Event_Flag)
      {
        myUsartGPS.Event_Flag = FALSE;
        myUsartGPS_Event();
      }  
    }
    else
    {
      GPS_Pow_OFF;
      myUsartGPS_GPIO_Cmd(DISABLE);
      GPSmode_Sleep_Count = 0;
    }
    //SMS
    if(GSMmode_Worke_Flag)
    {
      if(Phone_Num_Flag && SMS_Buf_Flag)// 发送数据就绪
      {
        Phone_Num_Flag = FALSE;
        SMS_Buf_Flag = FALSE;
        
        SMS_SendGSM();
      }
      Disposal_ReceiveGSM();//解析接收的短息
    }
    //1s Event
    if(Timer_1Sec_Flag)
    {
      Timer_1Sec_Flag = FALSE;
      
      M10_LOOP();//GSM基本配置流程
      
      if(GPSmode_Worke_Flag)
      {
        if(GPSmode_Sleep_Count++ > 90)
        {
          GPSmode_Worke_Flag = FALSE;
          memset((u8 *)Device_Info.SMS_Buf,0,128);
          strncpy((char *)Device_Info.SMS_Buf,(const char *)"No GPS Signal.\0",14);
          SMS_Buf_Flag = TRUE;
        }
      }
      if((GPSmode_Worke_Flag==FALSE) && GSMmode_Worke_Flag && (SendSMS_Flag==FALSE))
      {
        if(GSMmode_Sleep_Count++ > 90) // Mode Sleep
        {
          GSMmode_Sleep_Count = 0;
          DTR_Hi;
          Init_Gpio_Stop();
          PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
          myUsartGSM_GPIO_Cmd(ENABLE);
          Init_Gpio();
          while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
          DTR_Lo;
        }
      }
      else  
      {
        GSMmode_Sleep_Count = 0;
      }
      if(Read_ExtPow == Bit_SET)  LED2_ON;  else  LED2_OFF;
    }
  }
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


void Init_Clock(void)
{
  RCC_DeInit();
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  
  RCC_HCLKConfig(RCC_SYSCLK_Div2);  /* HCLK = 4MHz */
  RCC_PCLK2Config(RCC_HCLK_Div1); /* PCLK2 = 4MHz */
  RCC_PCLK1Config(RCC_HCLK_Div1); /* PCLK1 = 4MHz */
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
  // Enable Timer2 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  // Enable Timer3 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  // Enable GPIO clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
  // Enable USART1 clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  // Enable USART2 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  // Enable PWR clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
  #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  #endif
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  //Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //Enable the USART2 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //外部中断5
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);
  //TIME2 中断配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //TIME3 中断配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure; 
  /*connect to EXTI5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 
  //Config EXTI5
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  EXTI_Init(&EXTI_InitStructure); 
}

void Init_Gpio(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //配置GPIO为输入下拉
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //配置外部中断GPIO为输入上拉
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //配置为推挽输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Init_Gpio_Stop(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = 0xEECF;         //除了PA4\PA5\PA8\12外
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

//系统初始化
void System_Init(void)
{
  Init_SysTick();
  Timer2_Init();
  Timer3_Init();
  Init_Gpio();
  M10_Init();
  myUsartGSM_Init(9600);
  myUsartGPS_Init(9600);
}

void Init_SysTick(void)
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
  SysTick_SetReload(500); 
  SysTick_CounterCmd(SysTick_Counter_Disable);
  SysTick_CounterCmd(SysTick_Counter_Clear);
  SysTick_ITConfig(ENABLE);
}

//定时器2配置10ms
void Timer2_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  
  // Set timer period 10 ms
  TIM_TimeBaseInitStruct.TIM_Prescaler = 39;  // (39+1)/4M = 10us 
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 999;  // 10us*(999+1) = 10 ms
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
  TIM_ClearFlag(TIM2,TIM_FLAG_Update);
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  
  TIM_Cmd(TIM2,ENABLE);
}
//定时器3配置300ms
void Timer3_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  
  // Set timer period 300 ms
  TIM_TimeBaseInitStruct.TIM_Prescaler = 1200;  // 1200/4M = 300us 
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 1000;  // 300us*1000 = 300ms
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
  
  TIM_ClearFlag(TIM3,TIM_FLAG_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
}

void Disposal_ReceiveGSM(void)
{
  u8 data_len=0;
  
  if(!ReciveSMS_Flag) return;
  if(SendSMS_Flag) return;
  
  data_len = strlen((const char *)myUsartGSM.RxBuf);
  if(data_len > 10)  return;
  
  if(strncmp((const char *)myUsartGSM.RxBuf,(const char *)"GPS001\0",6) == 0)
  {
    GPSmode_Worke_Flag = TRUE;
    SendSMS_Flag = TRUE;
  }
  ReciveSMS_Flag = FALSE;
}

u8 Disposal_GpsGSM(u8 *Gps_Buf,u8 Buf_Len)
{
  u8 i=0,ChkXor=0,index=0;
  u8 temp8=0;
  u8 tempbuf[10];
  memset((u8 *)tempbuf,0,10); 
  
  ChkXor = Gps_Buf[0];
  for (i=1;i<Buf_Len-3;i++) ChkXor = ChkXor^Gps_Buf[i];
  if (ChkXor!=strtohex8((u8 *)&Gps_Buf[Buf_Len-2]))
  {
    Gps_Info.Status=FALSE;
    return FALSE;
  }
  if ((Buf_Len <60) || (Buf_Len>80))
  {
    Gps_Info.Status=FALSE;
    return FALSE;
  }
  
  index = strcspn((const char *)Gps_Buf,( const char *)",");  //求出','的索引值后+1
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]);       //将','后的数据备份
  //时分秒
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strncpy((char *)Gps_Info.Time,(const char *)Gps_Buf,index-4);  //减去时间后四位
  
  temp8 = (Gps_Info.Time[0]-0x30)*10;
  temp8 += Gps_Info.Time[1]-0x30;
  Gps_Info.Hour = temp8 + 8;  //字符转换为1字节
  bcdtostr1(Gps_Info.Hour,(u8 *)tempbuf);
  if(Gps_Info.Hour < 10)
  {
    Gps_Info.Time[0] = '0';
    Gps_Info.Time[1] = tempbuf[0];
  }
  else
  {
    Gps_Info.Time[0] = tempbuf[0];
    Gps_Info.Time[1] = tempbuf[1];
  }
  Gps_Info.Time[6] = '\0';
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]);  
  //有效标志
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  Gps_Info.Validity = Gps_Buf[0];
  if(Gps_Info.Validity != 'A')
  {
     Gps_Info.Status=FALSE;
     return FALSE;
  }
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]);  
  //纬度ddmm.mmmm
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strncpy((char *)Gps_Info.Latitude,(const char *)Gps_Buf,index);
  Gps_Info.Latitude[9] = '\0';
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]); 
  //N-S
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  Gps_Info.Latitude_NS = Gps_Buf[0];
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]); 
  //经度dddmm.mmmm
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strncpy((char *)Gps_Info.Longitude,(const char *)Gps_Buf,index);
  Gps_Info.Longitude[10] = '\0';
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]);
  //E-W
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  Gps_Info.Longitude_EW = Gps_Buf[0];
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]); 
  //速度
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]); 
  //方向
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strcpy((char *)Gps_Buf,(const char *)&Gps_Buf[index+1]); 
  //年月日
  index = strcspn((const char *)Gps_Buf,( const char *)",");
  strncpy((char *)tempbuf,(const char *)Gps_Buf,index);
  Gps_Info.Date[0] = '2';
  Gps_Info.Date[1] = '0';
  Gps_Info.Date[2] = tempbuf[4];
  Gps_Info.Date[3] = tempbuf[5];
  Gps_Info.Date[4] = tempbuf[2];
  Gps_Info.Date[5] = tempbuf[3];
  Gps_Info.Date[6] = tempbuf[0];
  Gps_Info.Date[7] = tempbuf[1];
  Gps_Info.Date[8] = '\0';
    
  Gps_Info.Status = TRUE;
  
  return TRUE;
}


void SMS_GPSdata(void)
{
  u8 i=0;
  u16 index=0;
  u8 *p;
  
  memset((u8 *)Device_Info.SMS_Buf,0,128);
  //Longitude
  p = "Long:\0";
  for(i=0;i<5;i++)
  {
    Device_Info.SMS_Buf[index++] = *p;
    p++;
  }
  Device_Info.SMS_Buf[index++] = Gps_Info.Longitude_EW;//WE
  Device_Info.SMS_Buf[index++] = ',';
  Device_Info.SMS_Buf[index++] = Gps_Info.Longitude[0];
  Device_Info.SMS_Buf[index++] = Gps_Info.Longitude[1];
  Device_Info.SMS_Buf[index++] = Gps_Info.Longitude[2];
  Device_Info.SMS_Buf[index++] = 0x27;
  for(i=0;i<7;i++)
  {
    Device_Info.SMS_Buf[index++] = Gps_Info.Longitude[i+3];
  }
  Device_Info.SMS_Buf[index++] = 0x0d;
  
  //Latitude
  p = "Lat:\0";
  for(i=0;i<4;i++)
  {
    Device_Info.SMS_Buf[index++] = *p;
    p++;
  }
  Device_Info.SMS_Buf[index++] = Gps_Info.Latitude_NS;//NS
  Device_Info.SMS_Buf[index++] = ',';
  Device_Info.SMS_Buf[index++] = Gps_Info.Latitude[0];
  Device_Info.SMS_Buf[index++] = Gps_Info.Latitude[1];
  Device_Info.SMS_Buf[index++] = 0x27;
  for(i=0;i<7;i++)
  {
    Device_Info.SMS_Buf[index++] = Gps_Info.Latitude[i+2];
  }
  Device_Info.SMS_Buf[index++] = 0x0d;
  
  //Time
  p = "Time:\0";
  for(i=0;i<5;i++)
  {
    Device_Info.SMS_Buf[index++] = *p;
    p++;
  }
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[0];
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[1];
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[2];
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[3];
  Device_Info.SMS_Buf[index++] = '/';
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[4];
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[5];
  Device_Info.SMS_Buf[index++] = '/';
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[6];
  Device_Info.SMS_Buf[index++] = Gps_Info.Date[7];
  Device_Info.SMS_Buf[index++] = '-';
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[0];
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[1];
  Device_Info.SMS_Buf[index++] = ':';
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[2];
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[3];
  Device_Info.SMS_Buf[index++] = ':';
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[4];
  Device_Info.SMS_Buf[index++] = Gps_Info.Time[5];
  Device_Info.SMS_Buf[index++] = 0;
}

void LED1_Flash(u8 count)
{
  LED1_Flash_Flag = TRUE;
  LED1_Flash_Count = count + count;
  TIM_Cmd(TIM3,ENABLE);
}

void SysTick_Delay_Ms(u16 time_ms)
{
  SysTick_CounterCmd(SysTick_Counter_Enable);
  SysTick_Count = time_ms;
  while(SysTick_Count != 0);
  SysTick_CounterCmd(SysTick_Counter_Disable);
  SysTick_CounterCmd(SysTick_Counter_Clear);
}
