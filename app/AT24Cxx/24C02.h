#ifndef __24C02_H
#define __24C02_H
#include "stm32f10x.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767  
//红牛开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02

//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x30000000;}

#define IIC_SCL(x)   ((x) ? (GPIOB->BSRR = 0x00000040):(GPIOB->BSRR = 0x00400000)) //GPIO_PIN_6
#define IIC_SDA(x)   ((x) ? (GPIOB->BSRR = 0x00000080):(GPIOB->BSRR = 0x00800000)) //GPIO_PIN_7

#define IIC_SDA_STATE	   GPIOB->IDR&0x0080                             //0000 0000 1000 0000


void Init_IIC(void);                     		 
void IIC_Start(void);				          
void IIC_Stop(void);	  			       
void IIC_Send_Byte(uint8_t txd);			   
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void); 				     
void IIC_Ack(void);					            
void IIC_NAck(void);			

uint8_t AT24CXX_Check(void);  
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);							
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);		
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len);					
void AT24CXX_Write(uint16_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);	
u16  AT24CXX_Read(uint16_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   	



			

#endif

