#ifndef my32x_h
#define my32x_h

void delay_ms(u16 value);

/* 1�ֽ�ת16�����ַ��� */
void hextostr1(u8 val8,u8 *pbuf);
/* 4�ֽ�ת16�����ַ��� */
void hextostr4(u32 val32,u8 *pbuf);
/* 2λ16�����ַ���ת1�ֽ� */
u8 strtohex8(u8 *pbuf);
/* 1�ֽ�ת10�����ַ��� */
void bcdtostr1(u8 val,u8 *pbuf);
/* 2�ֽ�ת10�����ַ��� */
void bcdtostr2(u16 val16,u8 *pbuf);
/* 4�ֽ�ת10�����ַ��� */
void bcdtostr4(u32 val32,u8 *pbuf);

#endif
