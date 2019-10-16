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
Pen_Holder Pen_Point;	                                     //定义笔实体 

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
  * @brief  SPI写1byte数据
  * @param  写入的数据
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
		TCLK(0);                                                 //上升沿有效	   	 
		TCLK(1);      
	} 			    
} 		 
  
/**
  * @brief  从7846/7843/XPT2046/UH7843/UH7846读取adc值	
  * @param  命令
  * @retval 读取的数据
  */
uint16_t ADS_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t i;
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK(0);                                                   //先拉低时钟 	 
	TCS(0);                                                    //选中xpt2046 
	ADS_Write_Byte(CMD);                                      //发送命令字
	for(i=100;i>0;i--);
	//delay_us(6);//ADS7846的转换时间最长为6us
	TCLK(1);                                                   //给1个时钟，清除BUSY   	    
	TCLK(0); 	 
	for(count=0;count<16;count++)  
	{ 				  
		Num<<=1; 	 
		TCLK(0);                                                 //下降沿有效  	    	   
		TCLK(1);
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;                                                  //只有高12位有效.
	TCS(1);                                                    //释放XPT2046 
	return(Num);   
}

/**
  * @brief  连续读取READ_TIMES次数据,对这些数据升序排列,
            然后去掉最低和最高LOST_VAL个数,取平均值 
  * @param  读坐标命令
  * @retval 坐标值
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
	for(i=0;i<READ_TIMES-1; i++)                                  //排序
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])                                         //升序排列
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
  * @brief  带滤波的坐标读取  最小值不能少于100.    
  * @param  xy首地址
  * @retval return 1读数成功
  */
uint8_t Read_ADS(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;                              //读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;                                                      //读数成功
}

/**
  * @brief  2次读取ADS7846,连续读取2次有效的AD值,且这两次的偏差不能超过
            50,满足条件,则认为读数正确,否则读数错误. 
  * @param  xy首地址
  * @retval return 1读数成功
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
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
		
/**
  * @brief  读取一次坐标值,直到PEN松开才返回!	
  * @param  None
  * @retval return 1读数成功
  */
uint8_t Read_TP_Once(void)
{
	uint8_t t=0;	    
	Pen_Int_Set(0);                            //关闭中断
	Pen_Point.Key_Sta=Key_Up;
	Read_ADS2(&Pen_Point.X,&Pen_Point.Y);
	while(!(PEN&0x80)&&t<=250)
	{
		t++;
		Delay(10);
	}
	Pen_Int_Set(1);                             //开启中断		 
	if(t>=250)return 0;                         //按下2.5s 认为无效
	else return 1;	
}


/**
  * @brief  根据触摸屏的校准参数来决定转换后的结果,保存在X0,Y0中
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
  * @brief  获取校准值
  * @param  None
  * @retval return 1 读数成功
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
  * @brief  得到四个校准参数
  * @param  None
  * @retval None
  */
void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];                        //坐标缓存值
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				
	TFT_ClearScreen(WHITE);                               //清屏   
	Drow_Touch_Point(20,20,RED);                    //画点1 
	Pen_Point.Key_Sta=Key_Up;                       //消除触发信号 
	Pen_Point.xfac=0;                                //xfac用来标记是否校准过,所以校准之前必须清掉!以免错误	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)                //按键按下了
		{
			if(Read_TP_Once())                           //得到单次按键值
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					TFT_ClearScreen(WHITE);                        //清屏 
					Drow_Touch_Point(220,20,RED);            //画点2
					break;
				case 2:
					TFT_ClearScreen(WHITE);                    //清屏 
					Drow_Touch_Point(20,300,RED);            //画点3
					break;
				case 3:
					TFT_ClearScreen(WHITE);                    //清屏 
					Drow_Touch_Point(220,300,RED);           //画点4
					break;
				case 4:	                               //全部四个点已经得到
	    		    	                                            //对边相等
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);          //x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);          //y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //得到1,2的距离
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);          //x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);          //y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //得到3,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)              //不合格
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                               //清屏 
						Drow_Touch_Point(20,20,RED);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);          //x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);          //y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //得到1,3的距离
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);          //x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);          //y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //得到2,4的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)                            //不合格
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                               //清屏 
						Drow_Touch_Point(20,20,RED);
						continue;
					}                                                 //正确了
								   
					//对角线相等
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);          //x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);          //y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);                               //得到1,4的距离
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);          //x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);          //y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);                               //得到2,3的距离
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)                            //不合格
					{
						cnt=0;
						TFT_ClearScreen(WHITE);                                //清屏 
						Drow_Touch_Point(20,20,RED);
						continue;
					}                                                  //正确了
					                                                   //计算结果
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);            //得到xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//得到xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);            //得到yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//得到yoff  
					
					TFT_ClearScreen(WHITE);                                                     //清屏
					//LCD_DisplayStr(35,110,(unsigned char*)"Touch Screen Adjust OK!",RED,WHITE);           //显示校正完成
					Delay(1000);
					TFT_ClearScreen(WHITE);                                                     //清屏   
					return;                                                               //校正完成				 
			}
		}
	} 
}

/**
  * @brief  保存校准参数
  * @param  None
  * @retval None
  */
void Save_Adjdata(void)
{
	s32 temp_data;
	u16  temp[8];
	temp_data=Pen_Point.xfac*100000000;    //保存x校正因素
	temp[0]=(u8)(temp_data & 0xff);        //保存x校正因素
	temp[1]=(u8)((temp_data >> 8) & 0xff); //保存x校正因素	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//保存x校正因素	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//保存x校正因素			      
  AT24CXX_Write(0x00,temp,8); 
	 
	temp_data=Pen_Point.yfac*100000000;    //保存y校正因素
	temp[0]=(u8)(temp_data & 0xff);        //保存x校正因素
	temp[1]=(u8)((temp_data >> 8) & 0xff); //保存x校正因素	
	temp[2]=(u8)((temp_data >> 16) & 0xff);//保存x校正因素	
	temp[3]=(u8)((temp_data >> 24) & 0xff);//保存x校正因素			
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
  * @brief  SPI引脚配置(模拟SPI)
  * @param  None
  * @retval None
  */
void Touch_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG , ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;           //SCLK MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                   //通用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;                          //MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                       //上拉输入
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Configure PB0 pin: TP_CS pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                           //CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	                   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//Configure PC5 pin: TP_INT pin 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                           //TOUCH_INT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	                    //上拉输入
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	         //推挽输出
  GPIO_Init(GPIOG,&GPIO_InitStructure); 
  GPIO_SetBits(GPIOG, GPIO_Pin_11);                          //失能spi总线上的其他设备(spi flash)

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			        //中断分组2

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	        //配置中断线7
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //先占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		      //次占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  触摸初始化
  * @param  None
  * @retval None
  */
void TOUCH_Init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	
	Touch_Configuration();
 	Read_ADS(&Pen_Point.X,&Pen_Point.Y);                     //第一次读取初始化			 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	    //使能IO复用功能，使用中断功能重要！！！
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
	 TFT_ClearScreen(WHITE);                     //清屏
	 if(Get_Adjdata())
		 return;              //已经校准
	 else			                             //未校准
	 { 										    
	   Touch_Adjust();                     //屏幕校准 
		 Save_Adjdata();	                   //保存校准值
	}			
	
  Get_Adjdata();                         //获取校准值	
}

/**
  * @brief  中断开关
  * @param  1开中断 0关中断
  * @retval None
  */
void Pen_Int_Set(uint8_t en)
{
	if(en)EXTI->IMR|=1<<7;                       //开启line7上的中断	  	
	else EXTI->IMR&=~(1<<7);                     //关闭line7上的中断	   
}

/****************************************************************************
* Function Name  : TOUCH_Scan
* Description    : 扫描是否有触摸按下
* Input          : None
* Output         : TouchData：读取到的物理坐标值和对应的彩屏坐标值
* Return         : 0：读取成功；0xFF：没有触摸
****************************************************************************/

uint8_t TOUCH_Scan(void)
{
    
 //   if(TOUCH_PEN == 0)   //查看是否有触摸
  //  {
        if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y)==0) //没有触摸
        {
            return 0xFF;    
        }
        /* 根据物理坐标值，计算出彩屏坐标值 */
   
		   Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		   Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  

        /* 查看彩屏坐标值是否超过彩屏大小 */
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
