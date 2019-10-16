#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

/* ����ͷ�ļ� */
#include"stm32f10x.h"

#include "stdio.h"
#include "stdlib.h"

#define LCD_RAM    *(__IO uint16_t *) ((uint32_t)0x6C000002)    	//��ַ�Ĵ���
#define LCD_REG    *(__IO uint16_t *) ((uint32_t)0x6C000000)	 	//ָ��Ĵ���


#define MAX_HZ_POSX 224
#define MAX_HZ_POSY 304 

#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY  304

#define Horizontal     0x00
#define Vertical       0x01

/*Private define-------------------------*/
#define POINT_COLOR WHITE
#define BACK_COLOR	BLACK

/*--------16λ��ɫֵ---------------*/
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ

/** 
  * @brief  ili9325/ili9328 LCD Registers  
  */ 
#define LCD_REG_0             0x00
#define LCD_REG_1             0x01
#define LCD_REG_2             0x02
#define LCD_REG_3             0x03
#define LCD_REG_4             0x04
#define LCD_REG_5             0x05
#define LCD_REG_6             0x06
#define LCD_REG_7             0x07
#define LCD_REG_8             0x08
#define LCD_REG_9             0x09
#define LCD_REG_10            0x0A
#define LCD_REG_12            0x0C
#define LCD_REG_13            0x0D
#define LCD_REG_14            0x0E
#define LCD_REG_15            0x0F
#define LCD_REG_16            0x10
#define LCD_REG_17            0x11
#define LCD_REG_18            0x12
#define LCD_REG_19            0x13
#define LCD_REG_20            0x14
#define LCD_REG_21            0x15
#define LCD_REG_22            0x16
#define LCD_REG_23            0x17
#define LCD_REG_24            0x18
#define LCD_REG_25            0x19
#define LCD_REG_26            0x1A
#define LCD_REG_27            0x1B
#define LCD_REG_28            0x1C
#define LCD_REG_29            0x1D
#define LCD_REG_30            0x1E
#define LCD_REG_31            0x1F
#define LCD_REG_32            0x20
#define LCD_REG_33            0x21
#define LCD_REG_34            0x22
#define LCD_REG_36            0x24
#define LCD_REG_37            0x25
#define LCD_REG_40            0x28
#define LCD_REG_41            0x29
#define LCD_REG_43            0x2B
#define LCD_REG_45            0x2D
#define LCD_REG_48            0x30
#define LCD_REG_49            0x31
#define LCD_REG_50            0x32
#define LCD_REG_51            0x33
#define LCD_REG_52            0x34
#define LCD_REG_53            0x35
#define LCD_REG_54            0x36
#define LCD_REG_55            0x37
#define LCD_REG_56            0x38
#define LCD_REG_57            0x39
#define LCD_REG_58            0x3A
#define LCD_REG_59            0x3B
#define LCD_REG_60            0x3C
#define LCD_REG_61            0x3D
#define LCD_REG_62            0x3E
#define LCD_REG_63            0x3F
#define LCD_REG_64            0x40
#define LCD_REG_65            0x41
#define LCD_REG_66            0x42
#define LCD_REG_67            0x43
#define LCD_REG_68            0x44
#define LCD_REG_69            0x45
#define LCD_REG_70            0x46
#define LCD_REG_71            0x47
#define LCD_REG_72            0x48
#define LCD_REG_73            0x49
#define LCD_REG_74            0x4A
#define LCD_REG_75            0x4B
#define LCD_REG_76            0x4C
#define LCD_REG_77            0x4D
#define LCD_REG_78            0x4E
#define LCD_REG_79            0x4F
#define LCD_REG_80            0x50
#define LCD_REG_81            0x51
#define LCD_REG_82            0x52
#define LCD_REG_83            0x53
#define LCD_REG_96            0x60
#define LCD_REG_97            0x61
#define LCD_REG_106           0x6A
#define LCD_REG_118           0x76
#define LCD_REG_128           0x80
#define LCD_REG_129           0x81
#define LCD_REG_130           0x82
#define LCD_REG_131           0x83
#define LCD_REG_132           0x84
#define LCD_REG_133           0x85
#define LCD_REG_134           0x86
#define LCD_REG_135           0x87
#define LCD_REG_136           0x88
#define LCD_REG_137           0x89
#define LCD_REG_139           0x8B
#define LCD_REG_140           0x8C
#define LCD_REG_141           0x8D
#define LCD_REG_143           0x8F
#define LCD_REG_144           0x90
#define LCD_REG_145           0x91
#define LCD_REG_146           0x92
#define LCD_REG_147           0x93
#define LCD_REG_148           0x94
#define LCD_REG_149           0x95
#define LCD_REG_150           0x96
#define LCD_REG_151           0x97
#define LCD_REG_152           0x98
#define LCD_REG_153           0x99
#define LCD_REG_154           0x9A
#define LCD_REG_157           0x9D
#define LCD_REG_192           0xC0
#define LCD_REG_193           0xC1
#define LCD_REG_229           0xE5

extern uint16_t DeviceCode;
void TFT_Init(void);
void TFT_WriteData(uint16_t dat);
void TFT_Init(void);
void TFT_SetWindow(unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end);
void TFT_ClearScreen(uint16_t color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_WR_REG(uint16_t LCD_Reg);
void TFT_SetWindowAPI(unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end);
///* TFT��ַ�ṹ�� */
//typedef struct
//{
//	u16 TFT_CMD;
//	u16 TFT_DATA;
//} TFT_TypeDef;
///* ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A10��Ϊ�������������� */ 
///* ע������ʱSTM32�ڲ�������һλ����! 111110=0X3E */			    
//#define TFT_BASE        ((uint32_t)(0x6C000000 | 0x000007FE))
//#define TFT             ((TFT_TypeDef *) TFT_BASE)

///* �������Ĵ�С */
#define TFT_XMAX 239		//����TFT���Ĵ�С
#define TFT_YMAX 319

///* ������ɫ�ĺ� */

//#define WHITE         	 0xFFFF
//#define BLACK         	 0x0000	  
//#define BLUE         	 0x001F  
//#define BRED             0XF81F
//#define GRED 			 0XFFE0
//#define GBLUE			 0X07FF
//#define RED           	 0xF800
//#define MAGENTA       	 0xF81F
//#define GREEN         	 0x07E0
//#define CYAN          	 0x7FFF
//#define YELLOW        	 0xFFE0
//#define BROWN 			 0XBC40 //��ɫ
//#define BRRED 			 0XFC07 //�غ�ɫ
//#define GRAY  			 0X8430 //��ɫ


//#define DARKBLUE      	 0X01CF	//����ɫ
//#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
//#define GRAYBLUE       	 0X5458 //����ɫ
////������ɫΪPANEL����ɫ 
// 
//#define LIGHTGREEN     	 0X841F //ǳ��ɫ
////#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
//#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

//#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
//#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) 

///* ����ȫ�ֱ��� */
//void TFT_WriteCmd(uint16_t cmd);
//void TFT_WriteData(uint16_t dat);
//void TFT_Init(void);
//void TFT_SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
//void TFT_ClearScreen(uint16_t color);
//void LCD_SetCursor(u16 Xpos, u16 Ypos);
//u16 LCD_ReadPoint(u16 x,u16 y);
#endif

