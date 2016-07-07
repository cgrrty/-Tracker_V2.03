#ifndef mym10_h
  #define mym10_h


#define  DTR_Hi         GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_SET)
#define  DTR_Lo         GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET)

#define  PWRKEY_Hi      GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET)
#define  PWRKEY_Lo      GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET)


typedef volatile struct
{
  vu8 Phone_Num[12];
  vu8 SMS_Buf[128];
}Device_Info_TypeDef;
extern Device_Info_TypeDef Device_Info;

extern vu8 M10_Step;
extern vu16 M10_Step_Timer;

extern vu8 GSM_CipSend_Flag;    //�յ���'>'
extern vu8 AT_ComOk_Flag;       //�յ�'ok'
extern vu8 Sim_CallReady_Flag;  //�յ�'Call Ready'
extern vu8 NetWork_Flag;        //����ע����Ϣ
extern vu8 ReciveSMS_Flag;      //�յ�SMS
extern vu8 SMS_Readstart_Flag;  //�յ� +CMT:
extern vu8 SMS_Enter_Count;     //�س�����
extern vu8 Phone_Num_Flag;      //SMS�����ֻ������־
extern vu8 SMS_Buf_Flag;        //SMS������Ϣ���ݱ�־
extern vu8 SendSMS_Flag;        //���ڷ���SMS��־

void M10_Init(void);
void M10_Power_ON(void);
void M10_Power_OFF(void);
void M10_LOOP(void);

void gsm_at(void);     //ͨѶ��������
void gsm_csq(void);    //����ź�ǿ��
void gsm_creg(void);   //����Ƿ�����
void gsm_ate(u8 i);    //�����Ƿ�������������
void at_qsclk(u8 i);   //GSMģ��˯����������
void at_cmgf(void);    //ѡ�����Ϣ��ʽ
void at_cscs(void);    //TE�ַ���
void at_cnmi(void);    //����Ϣָʾ��ʽ����

void SMS_SendGSM(void);     //���Ͷ���

#endif
