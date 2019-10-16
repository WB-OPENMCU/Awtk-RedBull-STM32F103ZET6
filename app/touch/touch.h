#ifndef __TOUCH_H
#define __TOUCH_H

/* ����ͷ�ļ� */
#include "sys.h"
#include "delay.h"

#define Key_Down   0x01  //����״̬
#define Key_Up     0x00

#define READ_TIMES 15   //��ȡ����
#define LOST_VAL    5	  //����ֵ
#define ERR_RANGE  50   //��Χ 

typedef struct 
{
	u16 X0;               //ԭʼ����
	u16 Y0;
	u16 X;                //����/�ݴ�����
	u16 Y;						   	    
	u8  Key_Sta;          //�ʵ�״̬			  
	
	float xfac;           //������У׼����
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;
extern Pen_Holder Pen_Point;


//������оƬ������������  
#define PEN	       GPIOG->IDR&0x0080                                            //PG7   INT
#define DOUT	     GPIOB->IDR&0x4000                                            //PB14  MISO

#define TDIN(x)   ((x) ? (GPIOB->BSRR = 0x00008000):(GPIOB->BSRR = 0x80000000)) //PB15  MOSI
#define TCLK(x)   ((x) ? (GPIOB->BSRR = 0x00002000):(GPIOB->BSRR = 0x20000000)) //PB13  SCLK
#define TCS(x)    ((x) ? (GPIOB->BSRR = 0x00001000):(GPIOB->BSRR = 0x10000000)) //PB12  CS 


//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ� 
#define CMD_RDY 0x90  //0B10010000���ò�ַ�ʽ��X����
#define CMD_RDX	0xD0  //0B11010000���ò�ַ�ʽ��Y����  
  
void TOUCH_Init(void);
void Touch_Adjust(void);
void Convert_Pos(void);
void Pen_Int_Set(uint8_t en);
void Touch_Configuration(void);
void ADS_Write_Byte(uint8_t num);
uint16_t ADS_Read_AD(uint8_t CMD);
uint16_t ADS_Read_XY(uint8_t xy);
uint8_t Read_TP_Once(void);
uint8_t Read_ADS2(uint16_t *x,uint16_t *y);
uint8_t Read_ADS(uint16_t *x,uint16_t *y);
void Save_Adjdata(void);                                    //����У׼����
uint8_t TOUCH_Scan(void);

#endif
