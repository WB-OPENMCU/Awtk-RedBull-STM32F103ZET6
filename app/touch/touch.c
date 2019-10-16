/**
  * @file    Touch.c 
  * @author  WB R&D Team - openmcu666
  * @version V1.0
  * @date    2016.05.05
  * @brief   Touch Driver
  */
	#include "24C02.h"
#include "Touch.h"
#include <gui.h>
#include "lcd_driver.h"
#include <string.h>
#include <stdlib.h>
Pen_Holder Pen_Point;	                                     //�����ʵ�� 

void Delay(uint32_t Time)
{
  uint32_t i;
    
  i = 0;
  while (Time--) 
	{
    for (i = 0; i < 1000; i++);
  }
}
/**
  * @brief  SPIд1byte����
  * @param  д�������
  * @retval None
  */
void ADS_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN(1);  
		else TDIN(0);   
		num<<=1;    
		TCLK(0);                                                 //��������Ч	   	 
		TCLK(1);      
	} 			    
} 		 
  
/**
  * @brief  ��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	
  * @param  ����
  * @retval ��ȡ������
  */
uint16_t ADS_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t i;
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK(0);                                                   //������ʱ�� 	 
	TCS(0);                                                    //ѡ��xpt2046 
	ADS_Write_Byte(CMD);                                      //����������
	for(i=100;i>0;i--);
	//delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK(1);                                                   //��1��ʱ�ӣ����BUSY   	    
	TCLK(0); 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK(0);                                                 //�½�����Ч  	    	   
		TCLK(1);
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;                                                  //ֻ�и�12λ��Ч.
	TCS(1);                                                    //�ͷ�XPT2046 
	return(Num);   
}

/**
  * @brief  ������ȡREAD_TIMES������,����Щ������������,
            Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
  * @param  ����������
  * @retval ����ֵ
  */
uint16_t ADS_Read_XY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)                                  //����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])                                         //��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

/**
  * @brief  ���˲��������ȡ  ��Сֵ��������100.    
  * @param  xy�׵�ַ
  * @retval return 1�����ɹ�
  */
uint8_t Read_ADS(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;                              //����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;                                                      //�����ɹ�
}

/**
  * @brief  2�ζ�ȡADS7846,������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���
            50,��������,����Ϊ������ȷ,�����������. 
  * @param  xy�׵�ַ
  * @retval return 1�����ɹ�
  */
uint8_t Read_ADS2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
		
/**
  * @brief  ��ȡһ������ֵ,ֱ��PEN�ɿ��ŷ���!	
  * @param  None
  * @retval return 1�����ɹ�
  */
uint8_t Read_TP_Once(void)
{
	uint8_t t=0;	    
	Pen_Int_Set(0);                            //�ر��ж�
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(!(PEN&0x80)&&t<=250)
	{
		t++;
		Delay(10);
	}
	Pen_Int_Set(1);                             //�����ж�		 
	if(t>=250)return 0;                         //����2.5s ��Ϊ��Ч
	else return 1;	
}


/**
  * @brief  ���ݴ�������У׼����������ת����Ľ��,������X0,Y0��
  * @param  None
  * @retval None
  */
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	}
}

/**
  * @brief  ��ȡУ׼ֵ
  * @param  None
  * @retval return 1 �����ɹ�
  */
u8 Get_Adjdata(void)
{
	s32 temp_data;
	u16  temp[8],i;
	temp_data = AT24CXX_Read(0x20,temp,8);
   if(temp[0] == 0xff)
   {
		for(i=0;i<8;i++)
		{
			temp[i] = 0;
		}
	   AT24CXX_Read(0x00,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.xfac = (float)temp_data /100000000;
	
	    AT24CXX_Read(0x08,temp,8);
	   temp_data = (s32)((temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0]);
	   Pen_Point.yfac = (float)temp_data /100000000;
	
	   AT24CXX_Read(0x10,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.xoff = temp_data;
	
	    AT24CXX_Read(0x18,temp,8);
	   temp_data = (s32)((temp[1]<<8)|temp[0]);
	   Pen_Point.yoff = temp_data;
	   return 1;
   }

   return 0;

}

/**
  * @brief  �õ��ĸ�У׼����
  * @param  None
  * @retval None
  */
void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];                        //���껺��ֵ
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				
	TFT_ClearScreen(WHITE);                               //����   
	Drow_Touch_Point(20,20,RED);                    //����1 
	Pen_Point.Key_Sta=Key_Up;                       //���������ź� 
	Pen_Point.xfac=0;                                //xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)                //����������
		{
			if(Read_TP_Once())                           //�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					TFT_ClearScreen(WHITE);                        //���� 
					Drow_Touch_Point(220,20,RED);            //����2
					break;
				case 2:
					TFT_ClearScreen(WHITE);                    //���� 
					Drow_Touch_Point(20,300,RED);            //����3
					break;
				case 3:
					TFT_ClearScreen(WHITE);                    //���� 
					Drow_Touch_Point(220,300,RED);           //����4
					break;
				case 4:	                               //ȫ���ĸ����Ѿ��õ�
	    		    	                                            //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);          //x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);          //y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);          //x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);          //y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)              //���ϸ�
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                               //���� 
						Drow_Touch_Point(20,20,RED);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);          //x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);          //y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);          //x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);          //y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)                            //���ϸ�
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                               //���� 
						Drow_Touch_Point(20,20,RED);
						continue;
					}                                                 //��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);          //x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);          //y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);          //x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);          //y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)                            //���ϸ�
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                                //���� 
						Drow_Touch_Point(20,20,RED);
						continue;
					}                                                  //��ȷ��
					                                                   //������
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);            //�õ�xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);            //�õ�yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					
					TFT_ClearScreen(WHITE);                                                     //����
					//LCD_DisplayStr(35,110,(unsigned char*)"Touch Screen Adjust OK!",RED,WHITE);           //��ʾУ�����
					Delay(1000);
					TFT_ClearScreen(WHITE);                                                     //����   
					return;                                                               //У�����				 
			}
		}
	} 
}

/**
  * @brief  ����У׼����
  * @param  None
  * @retval None
  */
void Save_Adjdata(void)
{
	s32 temp_data;
	u16  temp[8];
	temp_data=Pen_Point.xfac*100000000;    //����xУ������
	temp[0]=(u8)(temp_data & 0xff);        //����xУ������
	temp[1]=(u8)((temp_data >> 8) & 0xff); //����xУ������	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//����xУ������	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//����xУ������			      
  AT24CXX_Write(0x00,temp,8); 
	 
	temp_data=Pen_Point.yfac*100000000;    //����yУ������
	temp[0]=(u8)(temp_data & 0xff);        //����xУ������
	temp[1]=(u8)((temp_data >> 8) & 0xff); //����xУ������	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//����xУ������	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//����xУ������			
  AT24CXX_Write(0x08,temp,8);
  

	temp_data=Pen_Point.xoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
   AT24CXX_Write(0x10,temp,8);

	temp_data=Pen_Point.yoff;
	temp[0]=(u8)(temp_data &0xff);
	temp[1]=(u8)((temp_data >>8)&0xff);
  AT24CXX_Write(0x18,temp,8);

	temp[0]=0xff;
  AT24CXX_Write(0x20,temp,8);
}

/**
  * @brief  SPI��������(ģ��SPI)
  * @param  None
  * @retval None
  */
void Touch_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG , ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;           //SCLK MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                   //ͨ���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;                          //MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                       //��������
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Configure PB0 pin: TP_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                           //CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	                   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Configure PC5 pin: TP_INT pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                           //TOUCH_INT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	                    //��������
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	         //�������
  GPIO_Init(GPIOG,&GPIO_InitStructure); 
  GPIO_SetBits(GPIOG, GPIO_Pin_11);                          //ʧ��spi�����ϵ������豸(spi flash)

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			        //�жϷ���2

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	        //�����ж���7
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		      //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  ������ʼ��
  * @param  None
  * @retval None
  */
void TOUCH_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	Touch_Configuration();
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);                     //��һ�ζ�ȡ��ʼ��			 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	    //ʹ��IO���ù��ܣ�ʹ���жϹ�����Ҫ������
  /* Connect PEN EXTI Line to Key Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource7);

  /* Configure PEN EXTI Line to generate an interrupt on falling edge */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Generate software interrupt: simulate a falling edge applied on PEN EXTI line */
  EXTI_GenerateSWInterrupt(EXTI_Line7);
	 // AT24CXX_Write(0x20,0,8);
	 TFT_ClearScreen(WHITE);                     //����
	 if(Get_Adjdata())
		 return;              //�Ѿ�У׼
	 else			                             //δУ׼
	 { 										    
	   Touch_Adjust();                     //��ĻУ׼ 
		 Save_Adjdata();	                   //����У׼ֵ
	}			
	
  Get_Adjdata();                         //��ȡУ׼ֵ	
}

/**
  * @brief  �жϿ���
  * @param  1���ж� 0���ж�
  * @retval None
  */
void Pen_Int_Set(uint8_t en)
{
	if(en)EXTI->IMR|=1<<7;                       //����line7�ϵ��ж�	  	
	else EXTI->IMR&=~(1<<7);                     //�ر�line7�ϵ��ж�	   
}

/****************************************************************************
* Function Name  : TOUCH_Scan
* Description    : ɨ���Ƿ��д�������
* Input          : None
* Output         : TouchData����ȡ������������ֵ�Ͷ�Ӧ�Ĳ�������ֵ
* Return         : 0����ȡ�ɹ���0xFF��û�д���
****************************************************************************/

uint8_t TOUCH_Scan(void)
{
    
 //   if(TOUCH_PEN == 0)   //�鿴�Ƿ��д���
  //  {
        if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y)==0) //û�д���
        {
            return 0xFF;    
        }
        /* ������������ֵ���������������ֵ */
   
		   Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		   Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  

        /* �鿴��������ֵ�Ƿ񳬹�������С */
        if(Pen_Point.X0 > TFT_XMAX)
        {
            Pen_Point.X0 = TFT_XMAX;
        }
        if(Pen_Point.Y0 > TFT_YMAX)
        {
           Pen_Point.Y0 = TFT_YMAX;
        }
        return 0; 
  //  }
  //  return 0xFF;       
}
