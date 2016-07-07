#ifndef ini_h
  #define ini_h

#define  LED1_ON        GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET)
#define  LED1_OFF       GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET)
#define  Read_LED1      GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define  Flash_LED1     GPIO_WriteBit(GPIOA,GPIO_Pin_6,(BitAction)(1-Read_LED1))

#define  LED2_ON        GPIO_WriteBit(GPIOA,GPIO_Pin_7,Bit_RESET)
#define  LED2_OFF       GPIO_WriteBit(GPIOA,GPIO_Pin_7,Bit_SET)

#define  GPS_Pow_ON     GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_SET)
#define  GPS_Pow_OFF    GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_RESET)

#define  Read_ExtPow    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)


typedef volatile struct
{
  vu8 Validity; //有效性
  vu8 Time[7];
  vu8 Date[10];  //年月日时分秒
  vu8 Latitude[10];  //GPS纬度
  vu8 Latitude_NS;
  vu8 Longitude[11];//GPS经度
  vu8 Longitude_EW;
  vu8 Speed[10];     //速率
  vu8 Direct[10];    //方向
  
  vu8  Hour;
  vu8 Status;         //是否定位，TRUE or FALSE
}Gps_Info_TypeDef;
extern Gps_Info_TypeDef Gps_Info;

extern vu8 GPSmode_Worke_Flag;  //GPS工作状态标志
extern vu8 GSMmode_Worke_Flag;  //GSM工作状态标志
extern vu8 GPSmode_Sleep_Count; //GPS睡眠延时计数
extern vu8 GSMmode_Sleep_Count; //GSM睡眠延时计数

extern vu8 LED1_Flash_Flag;
extern vu8 LED1_Flash_Count;

extern vu8 Timer_10ms_Count;
extern vu8 Timer_1Sec_Flag;
extern vu16 SysTick_Count;


void Init_Clock(void);
void NVIC_Configuration(void);
void EXTI_Configuration(void);
void System_Init(void);
void Init_Gpio(void);
void Init_Gpio_Stop(void);
void Init_SysTick(void);
void Timer2_Init(void);
void Timer3_Init(void);

void Disposal_ReceiveGSM(void);

//处理GPS
u8 Disposal_GpsGSM(u8 *Gps_Buf,u8 Buf_Len);
//发送GPS数据
void SMS_GPSdata(void);

void LED1_Flash(u8 count);
void SysTick_Delay_Ms(u16 time_ms);

#endif
