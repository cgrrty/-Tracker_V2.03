#include "stm32f10x_lib.h"
#include "string.h"
#include "my32x.h"



void delay_ms(u16 ms_count)
{ 
  u32 value;
  
  value = ms_count * 400;
  while(value--)  asm("nop");
}


/* 1字节转16进制字符串 */
void hextostr1(u8 val8,u8 *pbuf)
{
  u8 i=0;
  
  i = val8 >> 4;
  if (i<10) pbuf[0]=0x30+i;else pbuf[0]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[1]=0x30+i;else pbuf[1]=55+i;
}
/* 4字节转16进制字符串 */
void hextostr4(u32 val32,u8 *pbuf)
{
  u8 val8=0,i=0;

  val8=val32>>24;
  i = val8 >> 4;
  if (i<10) pbuf[0]=0x30+i;else pbuf[0]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[1]=0x30+i;else pbuf[1]=55+i;

  val8=val32>>16;
  i = val8 >> 4;
  if (i<10) pbuf[2]=0x30+i;else pbuf[2]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[3]=0x30+i;else pbuf[3]=55+i;


  val8=val32>>8;
  i = val8 >> 4;
  if (i<10) pbuf[4]=0x30+i;else pbuf[4]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[5]=0x30+i;else pbuf[5]=55+i;

  val8=val32;
  i = val8 >> 4;
  if (i<10) pbuf[6]=0x30+i;else pbuf[6]=55+i;
  i = (val8 & 0x0f);
  if (i<10) pbuf[7]=0x30+i;else pbuf[7]=55+i;

}
/* 2位16进制字符串转1字节 */
u8 strtohex8(u8 *pbuf)
{
  u8 i=0,j=0,k=0;
  if (pbuf[0] < 0x3a) i=pbuf[0]-0x30;else i=pbuf[0]-55;
  if (pbuf[1] < 0x3a) j=pbuf[1]-0x30;else j=pbuf[1]-55;
  k = (i<<4);
  k = k+j;
  return(k);
} 
/* 1字节转10进制字符串 */
void bcdtostr1(u8 val,u8 *pbuf)
{
  u8 i=0;
  
  if (val>99)
  {
    i=val/100;
    pbuf[0]=0x30+i;
    val=val%100;
    i=val/10;
    pbuf[1]=0x30+i;
    i=val%10;
    pbuf[2]=0x30+i;
    pbuf[3]=0;
  }
  else if(val>9)
  {
    i=val/10;
    pbuf[0]=0x30+i;
    i=val%10;
    pbuf[1]=0x30+i;
    pbuf[2]=0;
  }
  else
  {
    pbuf[0]=0x30+val;
    pbuf[1]=0;
  }
}
/* 2字节转10进制字符串 */
void bcdtostr2(u16 val16,u8 *pbuf)
{
  u8 i=0,val=0;
  
  if (val16>9999)
  {
    i=val16/10000;
    pbuf[0]=0x30+i;
    val16=val16%10000;
    i=val16/1000;
    pbuf[1]=0x30+i;
    val16=val16%1000;
    i=val16/100;
    pbuf[2]=0x30+i;
    val=val16%100;
    i=val/10;
    pbuf[3]=0x30+i;
    i=val%10;
    pbuf[4]=0x30+i;
    pbuf[5]=0;
  }
  else if (val16>999)
  {
    i=val16/1000;
    pbuf[0]=0x30+i;
    val16=val16%1000;
    i=val16/100;
    pbuf[1]=0x30+i;
    val=val16%100;
    i=val/10;
    pbuf[2]=0x30+i;
    i=val%10;
    pbuf[3]=0x30+i;
    pbuf[4]=0;
  }
  else if (val16>99)
  {
    i=val16/100;
    pbuf[0]=0x30+i;
    val=val16%100;
    i=val/10;
    pbuf[1]=0x30+i;
    i=val%10;
    pbuf[2]=0x30+i;
    pbuf[3]=0;
  }
  else if(val16>9)
  {
    val=val16;
    i=val/10;
    pbuf[0]=0x30+i;
    i=val%10;
    pbuf[1]=0x30+i;
    pbuf[2]=0;
  }
  else
  {
    val=val16;
    pbuf[0]=0x30+val;
    pbuf[1]=0;
  }
}
/* 4字节转10进制字符串 */
void bcdtostr4(u32 val32,u8 *pbuf)
{
  u8 i=0,val=0;
  u16 val16=0;
  
  i=val32/1000000000;
  pbuf[0]=0x30+i;
  val32=val32%1000000000;
  
  i=val32/100000000;
  pbuf[1]=0x30+i;
  val32=val32%100000000;
  
  i=val32/10000000;
  pbuf[2]=0x30+i;
  val32=val32%10000000;

  i=val32/1000000;
  pbuf[3]=0x30+i;
  val32=val32%1000000;

  i=val32/100000;
  pbuf[4]=0x30+i;
  val32=val32%100000;
  
  i=val32/10000;
  pbuf[5]=0x30+i;
  val16=val32%10000;

  i=val16/1000;
  pbuf[6]=0x30+i;
  val16=val16%1000;
  i=val16/100;
  pbuf[7]=0x30+i;
  val=val16%100;
  i=val/10;
  pbuf[8]=0x30+i;
  i=val%10;
  pbuf[9]=0x30+i;
  pbuf[10]=0;
}
