/**
  * @file    24C02.c 
  * @author  WB R&D Team - openmcu666
  * @version V1.0
  * @date    2016.05.05
  * @brief   EEPROM Driver
  */
#include "24C02.h"

/**
  * @brief  ��ʱ����
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
  * @brief  ��ʱ1us (��ͨ���������ж�����׼ȷ��)
  * @param  time (us) ע��time<65535				
  * @retval None
  */
void Delay_Us(uint16_t time)  
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<9;j++);
}

/**
  * @brief  ��ʼ��SDA,SCL��IO
  * @param  None
  * @retval None
  */
void Init_IIC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					          //����һ��GPIO�ṹ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	  //ʹ�ܸ����˿�ʱ�ӣ���Ҫ������

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //����LED�˿ڹҽӵ�6��12��13�˿�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	  //ͨ�������©
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	  //���ö˿��ٶ�Ϊ50M
  GPIO_Init(GPIOB, &GPIO_InitStructure);				   	      //���˿�GPIOD���г�ʼ������
}

/**
  * @brief  ����IIC��ʼ�ź�
  * @param  None
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();                                                //sda�����
	IIC_SDA(1);	  	  
	IIC_SCL(1);
	Delay_Us(4);
 	IIC_SDA(0);
	Delay_Us(4);
	IIC_SCL(0);                                              //ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

/**
  * @brief  ����IICֹͣ�ź�
  * @param  None
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();                                                //sda�����
	IIC_SCL(0);
	IIC_SDA(0);
 	Delay_Us(4);
	IIC_SCL(1);
	IIC_SDA(1);
	Delay_Us(4);							   	
}
        
/**
  * @brief  �ȴ�Ӧ���źŵ���
  * @param  None
  * @retval 1:����Ӧ��ʧ��;0:����Ӧ��ɹ�
  */
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();                               //SDA����Ϊ����  
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
  * @brief  ����ACKӦ��
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
  * @brief  ������ACKӦ��		
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
  * @brief  IIC����һ���ֽ�
  * @param  Ҫ���͵�����
  * @retval None
  */
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
	SDA_OUT(); 	    
  IIC_SCL(0);//IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
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
  * @brief  ��1���ֽ�
  * @param  ack=1ʱ,����ACK;ack=0,����nACK 
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
  if (!ack) IIC_NAck(); //����nACK
    
  else IIC_Ack();       //����ACK 
         
  return receive;
} 


/**
  * @brief  ��AT24CXXָ����ַ����һ������
  * @param  ReadAddr:��ʼ�����ĵ�ַ  			
  * @retval ����������
  */
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	                         //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);                    //���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);                     //���͵͵�ַ
	IIC_Wait_Ack();	     
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);                              //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();                                       //����һ��ֹͣ����	    
	return temp;
}

/**
  * @brief  ��AT24CXXָ����ַд��һ������
  * @param  WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ ;DataToWrite:Ҫд�������	
  * @retval None
  */
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	                             //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);                       //���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));           //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);                          //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);                           //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();                                           //����һ��ֹͣ���� 
	Delay_Ms(10);	 
}

/**
  * @brief  ��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������(�ú�������д��16bit����32bit������)
  * @param  WriteAddr  :��ʼд��ĵ�ַ;DataToWrite:���������׵�ַ; Len:Ҫд�����ݵĳ���2,4
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
  * @brief  ��AT24CXX�����ָ����ַ��ʼ��������ΪLen������(�ú������ڶ�ȡ16bit����32bit������)
  * @param  ReadAddr   :��ʼ�����ĵ�ַ;DataToWrite:���������׵�ַ; Len:Ҫ��ȡ���ݵĳ���2,4
  * @retval ��ȡ������
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
  * @brief  ���AT24CXX�Ƿ�����
  * @param  None
  * @retval 1:���ʧ��;0:���ɹ�
  */
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);    //����ÿ�ο�����дAT24CXX			   
	if(temp==0x55)return 0;		   
	else                              //�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0x55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0x55)return 0;
	}
	return 1;											  
}

/**
  * @brief  ��AT24CXX�����ָ����ַ��ʼ����ָ������������
  * @param  ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255;pBuffer  :���������׵�ַ
  *         NumToRead:Ҫ�������ݵĸ���
  * @retval *pBuffer:ָ���ȡ���ݵĻ�����
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
  * @brief  ��AT24CXX�����ָ����ַ��ʼд��ָ������������
  * @param  WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255;pBuffer   :���������׵�ַ
  *         NumToWrite:Ҫд�����ݵĸ���
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
