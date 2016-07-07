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

extern vu8 GSM_CipSend_Flag;    //收到到'>'
extern vu8 AT_ComOk_Flag;       //收到'ok'
extern vu8 Sim_CallReady_Flag;  //收到'Call Ready'
extern vu8 NetWork_Flag;        //网络注册信息
extern vu8 ReciveSMS_Flag;      //收到SMS
extern vu8 SMS_Readstart_Flag;  //收到 +CMT:
extern vu8 SMS_Enter_Count;     //回车计数
extern vu8 Phone_Num_Flag;      //SMS发送手机号码标志
extern vu8 SMS_Buf_Flag;        //SMS发送信息内容标志
extern vu8 SendSMS_Flag;        //正在发送SMS标志

void M10_Init(void);
void M10_Power_ON(void);
void M10_Power_OFF(void);
void M10_LOOP(void);

void gsm_at(void);     //通讯测试命令
void gsm_csq(void);    //检查信号强度
void gsm_creg(void);   //检查是否入网
void gsm_ate(u8 i);    //决定是否回显输入的命令
void at_qsclk(u8 i);   //GSM模块睡眠允许命令
void at_cmgf(void);    //选择短消息格式
void at_cscs(void);    //TE字符集
void at_cnmi(void);    //新消息指示方式设置

void SMS_SendGSM(void);     //发送短信

#endif
