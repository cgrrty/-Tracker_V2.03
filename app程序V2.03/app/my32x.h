#ifndef my32x_h
#define my32x_h

void delay_ms(u16 value);

/* 1字节转16进制字符串 */
void hextostr1(u8 val8,u8 *pbuf);
/* 4字节转16进制字符串 */
void hextostr4(u32 val32,u8 *pbuf);
/* 2位16进制字符串转1字节 */
u8 strtohex8(u8 *pbuf);
/* 1字节转10进制字符串 */
void bcdtostr1(u8 val,u8 *pbuf);
/* 2字节转10进制字符串 */
void bcdtostr2(u16 val16,u8 *pbuf);
/* 4字节转10进制字符串 */
void bcdtostr4(u32 val32,u8 *pbuf);

#endif
