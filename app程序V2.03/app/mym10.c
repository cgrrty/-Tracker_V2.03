#include "stm32f10x_lib.h"
#include "string.h"
#include "my32x.h"
#include "ini.h"
#include "mym10.h"
#include "myusart.h"


Device_Info_TypeDef Device_Info;

vu8 M10_Step = 0x01;
vu16 M10_Step_Timer = 15;

vu8 GSM_CipSend_Flag = FALSE;
vu8 AT_ComOk_Flag = FALSE;
vu8 Sim_CallReady_Flag = FALSE;
vu8 NetWork_Flag = FALSE;

vu8 ReciveSMS_Flag = FALSE;
vu8 SMS_Readstart_Flag = FALSE;
vu8 SMS_Enter_Count =0;

vu8 Phone_Num_Flag = FALSE;
vu8 SMS_Buf_Flag = FALSE;
vu8 SendSMS_Flag = FALSE;


void M10_Init(void)
{
  PWRKEY_Hi;
  DTR_Lo;
}

void M10_Power_ON(void)
{
  PWRKEY_Hi;
  PWRKEY_Lo;
  //t > 2000ms
  SysTick_Delay_Ms(2400);
  PWRKEY_Hi;
}

void M10_Power_OFF(void)
{
  PWRKEY_Hi;
  PWRKEY_Lo;
  //600ms< t < 1000ms
  SysTick_Delay_Ms(800);
  PWRKEY_Hi;
}

void M10_LOOP(void)
{
  switch(M10_Step)
  {
    case 0x01:  //开机
                M10_Power_ON();
                M10_Step_Timer = 15;
                GSMmode_Worke_Flag = FALSE;
                M10_Step = 0x03;
      break;
    case 0x02:  //关机
                M10_Power_OFF();
                M10_Step = 0x01;
      break;
    case 0x03:  //等待通信成功
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x04;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else gsm_at();
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x02;
      break;
    case 0x04:  //设置AT命令回显
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x11;
                  M10_Step_Timer = 60;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else gsm_ate('0');
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x11:  //检查SIM
                if(Sim_CallReady_Flag)
                {
                  M10_Step = 0x12;
                  M10_Step_Timer = 3600;
                  break;
                }
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x12:  //等待注册网络
                if(NetWork_Flag)
                {
                  M10_Step = 0x21;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else gsm_creg();
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x21:  //设置短消息为TEXT模式
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x22;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else at_cmgf();    
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
       break;
    case 0x22:  //TE 字符集为GSM格式
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x23;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else at_cscs(); 
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x23:  //新消息指示方式设置
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x24;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else at_cnmi();
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x24:  //检查信号强度
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x25;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  break;
                }
                else gsm_csq();
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
    case 0x25:  //模块睡眠允许功能
                if(AT_ComOk_Flag)
                {
                  M10_Step = 0x00;
                  M10_Step_Timer = 15;
                  AT_ComOk_Flag = FALSE;
                  GSMmode_Worke_Flag = TRUE;  //完成GSM模块初始化
                  LED1_Flash(6);
                  break;
                }
                else at_qsclk('1');
                if(M10_Step_Timer > 0) M10_Step_Timer--;
                else  M10_Step = 0x01;
      break;
  default : return;
  }
}

void gsm_at(void)
{
  myUsartGSM_Push_String("AT");
  myUsartGSM_Push_Byte(0x0d);
}

void gsm_csq(void)
{
  myUsartGSM_Push_String("AT+CSQ");
  myUsartGSM_Push_Byte(0x0d);
}

void gsm_creg(void)
{
  myUsartGSM_Push_String("AT+CREG?");
  myUsartGSM_Push_Byte(0x0d);
}

void gsm_ate(u8 i)
{
  myUsartGSM_Push_String("ATE");
  myUsartGSM_Push_Byte(i);
  myUsartGSM_Push_Byte(0x0d);
}
/////////////////SMS操作函数//////////////////
void at_cmgf()
{
  myUsartGSM_Push_String("AT+CMGF=1"); 
  myUsartGSM_Push_Byte(0x0d);
}

void at_cscs()
{
  myUsartGSM_Push_String("AT+CSCS=");
  myUsartGSM_Push_Byte('"');
  myUsartGSM_Push_String("GSM");
  myUsartGSM_Push_Byte('"');
  myUsartGSM_Push_Byte(0x0d);
}

void at_cnmi()
{
  myUsartGSM_Push_String("AT+CNMI=2,2,0,0,0");
  myUsartGSM_Push_Byte(0x0d);
}

void at_qsclk(u8 i)
{
  myUsartGSM_Push_String("AT+QSCLK=");
  myUsartGSM_Push_Byte(i);
  myUsartGSM_Push_Byte(0x0d);
}

void SMS_SendGSM(void)
{
  GSM_CipSend_Flag = FALSE;
  
  myUsartGSM_Push_String("AT+CMGS=\0");
  myUsartGSM_Push_Byte('"');
  myUsartGSM_Push_String((u8 *)Device_Info.Phone_Num);
  myUsartGSM_Push_Byte('"');
  myUsartGSM_Push_Byte(0x0d);
  
  while(!GSM_CipSend_Flag);
  myUsartGSM_Push_String((u8 *)Device_Info.SMS_Buf);
  myUsartGSM_Push_Byte(0x1a);
  SendSMS_Flag = FALSE;
}
