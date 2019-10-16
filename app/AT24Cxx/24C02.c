/**
  * @file    24C02.c 
  * @author  WB R&D Team - openmcu666
  * @version V1.0
  * @date    2016.05.05
  * @brief   EEPROM Driver
  */
#include "24C02.h"

/**
  * @brief  延时函数
  * @param  None
  * @retval None
  */
void Delay_Ms(uint16_t time)  
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<10260;j++);
}

/**
  * @brief  延时1us (可通过仿真来判断他的准确度)
  * @param  time (us) 注意time<65535				
  * @retval None
  */
void Delay_Us(uint16_t time)  
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<9;j++);
}

/**
  * @brief  初始化SDA,SCL的IO
  * @param  None
  * @retval None
  */
void Init_IIC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					          //定义一个GPIO结构体变量
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //使能各个端口时钟，重要！！！

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //配置LED端口挂接到6、12、13端口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	  //通用输出开漏
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	  //配置端口速度为50M
  GPIO_Init(GPIOB, &GPIO_InitStructure);				   	      //将端口GPIOD进行初始化配置
}

/**
  * @brief  产生IIC起始信号
  * @param  None
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();                                                //sda线输出
	IIC_SDA(1);	  	  
	IIC_SCL(1);
	Delay_Us(4);
 	IIC_SDA(0);
	Delay_Us(4);
	IIC_SCL(0);                                              //钳住I2C总线，准备发送或接收数据 
}	  

/**
  * @brief  产生IIC停止信号
  * @param  None
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();                                                //sda线输出
	IIC_SCL(0);
	IIC_SDA(0);
 	Delay_Us(4);
	IIC_SCL(1);
	IIC_SDA(1);
	Delay_Us(4);							   	
}
        
/**
  * @brief  等待应答信号到来
  * @param  None
  * @retval 1:接收应答失败;0:接收应答成功
  */
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();                               //SDA设置为输入  
	IIC_SDA(1);
	Delay_Us(1);	   
	IIC_SCL(1);
	Delay_Us(1);	 
	while(IIC_SDA_STATE)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0); 
	return 0;  
} 

/**
  * @brief  发出ACK应答
  * @param  None
  * @retval None
  */
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	Delay_Us(2);
	IIC_SCL(1);
	Delay_Us(2);
	IIC_SCL(0);
}

/**
  * @brief  不产生ACK应答		
  * @param  None
  * @retval None
  */
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	Delay_Us(2);
	IIC_SCL(1);
	Delay_Us(2);
	IIC_SCL(0);
}					 				     
		 
/**
  * @brief  IIC发送一个字节
  * @param  要发送的数据
  * @retval None
  */
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
	SDA_OUT(); 	    
  IIC_SCL(0);//IIC_SCL=0;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    IIC_SDA((txd&0x80)>>7);
    txd<<=1; 	  
		Delay_Us(2);   
		IIC_SCL(1);
		Delay_Us(2); 
		IIC_SCL(0);
		Delay_Us(2);
  }	 

} 	    

/**
  * @brief  读1个字节
  * @param  ack=1时,发送ACK;ack=0,发送nACK 
  * @retval None
  */
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();
  for(i=0;i<8;i++ )
	{
    IIC_SCL(0);
    Delay_Us(2);
		IIC_SCL(1);
    receive<<=1;
    if(IIC_SDA_STATE) receive++;  
		Delay_Us(1); 
  }					 
  if (!ack) IIC_NAck(); //发送nACK
    
  else IIC_Ack();       //发送ACK 
         
  return receive;
} 


/**
  * @brief  在AT24CXX指定地址读出一个数据
  * @param  ReadAddr:开始读数的地址  			
  * @retval 读到的数据
  */
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	                         //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);                    //发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);                     //发送低地址
	IIC_Wait_Ack();	     
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);                              //进入接收模式			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();                                       //产生一个停止条件	    
	return temp;
}

/**
  * @brief  在AT24CXX指定地址写入一个数据
  * @param  WriteAddr  :写入数据的目的地址 ;DataToWrite:要写入的数据	
  * @retval None
  */
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	                             //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);                       //发送高地址
		IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));           //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);                          //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);                           //发送字节							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();                                           //产生一个停止条件 
	Delay_Ms(10);	 
}

/**
  * @brief  在AT24CXX里面的指定地址开始写入长度为Len的数据(该函数用于写入16bit或者32bit的数据)
  * @param  WriteAddr  :开始写入的地址;DataToWrite:数据数组首地址; Len:要写入数据的长度2,4
  * @retval None
  */
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{  	
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**
  * @brief  在AT24CXX里面的指定地址开始读出长度为Len的数据(该函数用于读取16bit或者32bit的数据)
  * @param  ReadAddr   :开始读出的地址;DataToWrite:数据数组首地址; Len:要读取数据的长度2,4
  * @retval 读取的数据
  */
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/**
  * @brief  检查AT24CXX是否正常
  * @param  None
  * @retval 1:检测失败;0:检测成功
  */
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);    //避免每次开机都写AT24CXX			   
	if(temp==0x55)return 0;		   
	else                              //排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0x55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0x55)return 0;
	}
	return 1;											  
}

/**
  * @brief  在AT24CXX里面的指定地址开始读出指定个数的数据
  * @param  ReadAddr :开始读出的地址 对24c02为0~255;pBuffer  :数据数组首地址
  *         NumToRead:要读出数据的个数
  * @retval *pBuffer:指向读取数据的缓冲区
  */
u16 AT24CXX_Read(uint16_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
	return *pBuffer;
}  

/**
  * @brief  在AT24CXX里面的指定地址开始写入指定个数的数据
  * @param  WriteAddr :开始写入的地址 对24c02为0~255;pBuffer   :数据数组首地址
  *         NumToWrite:要写入数据的个数
  * @retval None
  */
void AT24CXX_Write(uint16_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
