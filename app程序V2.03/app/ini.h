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
  vu8 Validity; //��Ч��
  vu8 Time[7];
  vu8 Date[10];  //������ʱ����
  vu8 Latitude[10];  //GPSγ��
  vu8 Latitude_NS;
  vu8 Longitude[11];//GPS����
  vu8 Longitude_EW;
  vu8 Speed[10];     //����
  vu8 Direct[10];    //����
  
  vu8  Hour;
  vu8 Status;         //�Ƿ�λ��TRUE or FALSE
}Gps_Info_TypeDef;
extern Gps_Info_TypeDef Gps_Info;

extern vu8 GPSmode_Worke_Flag;  //GPS����״̬��־
extern vu8 GSMmode_Worke_Flag;  //GSM����״̬��־
extern vu8 GPSmode_Sleep_Count; //GPS˯����ʱ����
extern vu8 GSMmode_Sleep_Count; //GSM˯����ʱ����

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

//����GPS
u8 Disposal_GpsGSM(u8 *Gps_Buf,u8 Buf_Len);
//����GPS����
void SMS_GPSdata(void);

void LED1_Flash(u8 count);
void SysTick_Delay_Ms(u16 time_ms);

#endif
