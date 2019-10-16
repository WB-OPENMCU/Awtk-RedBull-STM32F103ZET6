#include "lcd_driver.h"


uint16_t DeviceCode;                                             //���ڴ�ȡ�豸ID
#define  Font_Size 16                 		                       // Ĭ������Ϊ16*8 
#define  XY        1											                       //����֧�ֺ������л�,����:1;����:0

                                               
/*********************************************************
** Function name:       void Delay_10ms (u32 nCount)
** Descriptions:        LCD��ʱ���� 10MS
** input parameters:    nCount
** output parameters:   ��
** Returned value:      ��
*********************************************************/
void Delay_10ms (u32 nCount)
{
	volatile int i;	 	
	for (i=0;i<nCount*800;i++);
}

/*********************************************************
** Function name:       void LCD_WR_REG(uint16_t LCD_Reg)
** Descriptions:        д�Ĵ���
** input parameters:    LCD_Reg
** output parameters:   ��
** Returned value:      ��
*********************************************************/
void LCD_WR_REG(uint16_t LCD_Reg)
{
	LCD_REG= LCD_Reg;	 		//��ָ��Ĵ���дָ��
}

/*********************************************************
** Function name:       void LCD_WR_DATA(uint16_t LCD_Data)	
** Descriptions:        д����
** input parameters:    LCD_Data
** output parameters:   ��
** Returned value:      ��
*********************************************************/
void TFT_WriteData(uint16_t dat)	
{
	LCD_RAM= dat; 			//�����ݼĴ���д����
} 

/*********************************************************************************
** Function name:     void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
** Descriptions:      ���üĴ���
** input parameters:  �Ĵ�����ַ:LCD_Reg   ����:RegValue
** output parameters: ��
** Returned value:    ��
**********************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);                    //д�Ĵ���
  TFT_WriteData(LCD_RegValue);	            //д����
			    
}

/*********************************************************
** Function name:       void LCD_WriteRAM_Prepare(void)
** Descriptions:        ��ʼдGRAM
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************/
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(0x22);
}


/*********************************************************
** Function name:       void LCD_WriteRAM(u16 RGB_Code)
** Descriptions:        LCDдGRAM
** input parameters:    ��ɫֵ
** output parameters:   ��
** Returned value:      ��
*********************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{							    
	TFT_WriteData(RGB_Code);               //дʮ��λGRAM
}

/*********************************************************
** Function name:       uint16_t Read_LCDReg(uint16_t LCD_Reg)
** Descriptions:        ���Ĵ���
** input parameters:    LCD_Reg:�Ĵ���
** output parameters:   ��
** Returned value:      ���ؼĴ���ֵ
*********************************************************/
uint16_t Read_LCDReg(uint16_t LCD_Reg)
{
	uint16_t temp;

	LCD_REG= LCD_Reg;
 
	temp= LCD_RAM; 
  
  return temp;		
}

/**********************************************************************************
** Function name:       void LCD_Configuration(void)
** Descriptions:        LCD�����������
** input parameters:  	��
** output parameters:   ��
** Returned value:      ��
************************************************************************************/
void LCD_IOConfig(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);
  /* Set PF.00(A0 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**********************************************************************************
** Function name:       void FSMC_LCDInit(void)
** Descriptions:        FSMCʱ������
** input parameters:  	��
** output parameters:   ��
** Returned value:      ��
************************************************************************************/
void FSMC_LCDInit(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	   	//����һ��NOR��ʼ���ṹ��
	FSMC_NORSRAMTimingInitTypeDef  w,r;						          //����һ��NORʱ���ʼ���ṹ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);						//ʹ��FSMC����ʱ��

  w.FSMC_AddressSetupTime = 0x00;							                  //��ַ����ʱ�䣺0x1
  w.FSMC_AddressHoldTime = 0x00;							                  //��ַ����ʱ�䣺0x00
  w.FSMC_DataSetupTime = 0x01;							                    //���ݽ���ʱ�䣺0x2
  w.FSMC_BusTurnAroundDuration = 0x00;
  w.FSMC_CLKDivision = 0x00;
  w.FSMC_DataLatency = 0x00;
  w.FSMC_AccessMode = FSMC_AccessMode_B;

	r.FSMC_AddressSetupTime = 0x00;							                   //��ַ����ʱ�䣺0x1
  r.FSMC_AddressHoldTime = 0x00;							                   //��ַ����ʱ�䣺0x00
  r.FSMC_DataSetupTime = 0x07;							                     //���ݽ���ʱ�䣺0x2
  r.FSMC_BusTurnAroundDuration = 0x00;
  r.FSMC_CLKDivision = 0x00;
  r.FSMC_DataLatency = 0x0f;
  r.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;	   	//��չNOR BANK �ĵ�1����BANK
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//��ʹ�����߸���
																	                                            
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//��չ����ΪNOR FLASH
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//��չ���߿��Ϊ16λ
																	                                          
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
																	
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
																	
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &r;	  	
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &w;	  
 
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 							//����ָ��������ʼ���ṹ��
  										
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); 							//ʹ��FSMC_Bank1_NORSRAM4 �ڴ�

}
/**********************************************************************************
** Function name:       void LCD_Init(void)
** Descriptions:        LCD��ʼ��
** input parameters:  	��
** output parameters:   ��
** Returned value:      ��
************************************************************************************/
void TFT_Init(void)
{
	LCD_IOConfig();
  FSMC_LCDInit();
	Delay_10ms(5);                     // delay 50 ms 
	Delay_10ms(5);                     // delay 50 ms 
	DeviceCode = Read_LCDReg(0x0000);       // ������IC
	Delay_10ms(5);                     // delay 50 ms
	//printf("lcd id is 0x%x",DeviceCode);
	//DeviceCode=0x8999;
	if(DeviceCode==0x8999)	           // ��Ӧ������ICΪSSD1298
	{
		/*-----   Start Initial Sequence ------*/
		LCD_WriteReg(0x00, 0x0001); /*Start internal OSC */
		LCD_WriteReg(0x01, 0x3B3F); /*Driver output control */
		LCD_WriteReg(0x02, 0x0600); /* set 1 line inversion	*/
		/*-------- Power control setup --------*/
		LCD_WriteReg(0x0C, 0x0007); /* Adjust VCIX2 output voltage */
		LCD_WriteReg(0x0D, 0x0006); /* Set amplitude magnification of VLCD63 */
		LCD_WriteReg(0x0E, 0x3200); /* Set alternating amplitude of VCOM */
		LCD_WriteReg(0x1E, 0x00BB); /* Set VcomH voltage */
		LCD_WriteReg(0x03, 0x6A64); /* Step-up factor/cycle setting  */
		/*-------- RAM position control --------*/
		LCD_WriteReg(0x0F, 0x0000); /* Gate scan position start at G0 */
		LCD_WriteReg(0x44, 0xEF00); /* Horizontal RAM address position */
		LCD_WriteReg(0x45, 0x0000); /* Vertical RAM address start position*/
		LCD_WriteReg(0x46, 0x013F); /* Vertical RAM address end position */
		/* ------ Adjust the Gamma Curve -------*/
		LCD_WriteReg(0x30, 0x0000);
		LCD_WriteReg(0x31, 0x0706);
		LCD_WriteReg(0x32, 0x0206);
		LCD_WriteReg(0x33, 0x0300);
		LCD_WriteReg(0x34, 0x0002);
		LCD_WriteReg(0x35, 0x0000);
		LCD_WriteReg(0x36, 0x0707);
		LCD_WriteReg(0x37, 0x0200);
		LCD_WriteReg(0x3A, 0x0908);
		LCD_WriteReg(0x3B, 0x0F0D);
		/*--------- Special command -----------*/
		LCD_WriteReg(0x28, 0x0006); /* Enable test command	*/
		LCD_WriteReg(0x2F, 0x12EB); /* RAM speed tuning	 */
		LCD_WriteReg(0x26, 0x7000); /* Internal Bandgap strength */
		LCD_WriteReg(0x20, 0xB0E3); /* Internal Vcom strength */
		LCD_WriteReg(0x27, 0x0044); /* Internal Vcomh/VcomL timing */
		LCD_WriteReg(0x2E, 0x7E45); /* VCOM charge sharing time */ 
		/*--------- Turn On display ------------*/
		LCD_WriteReg(0x10, 0x0000); /* Sleep mode off */
		Delay_10ms(3);              /* Wait 30mS  */
		LCD_WriteReg(0x11, 0x6870); /* Entry mode setup. 262K type B, take care on the data bus with 16it only */
		LCD_WriteReg(0x07, 0x0033); /* Display ON	*/
	}		

  if(DeviceCode==0x9325||DeviceCode==0x9328)//ILI9325
	{				
		LCD_WriteReg(LCD_REG_0, 0x0001); /* Start internal OSC. */
    LCD_WriteReg(LCD_REG_1, 0x0100); /* Set SS and SM bit */
    LCD_WriteReg(LCD_REG_2, 0x0700); /* Set 1 line inversion */
    LCD_WriteReg(LCD_REG_3, 0x1018); /* Set GRAM write direction and BGR=1. */
    LCD_WriteReg(LCD_REG_4, 0x0000); /* Resize register */
    LCD_WriteReg(LCD_REG_8, 0x0202); /* Set the back porch and front porch */
    LCD_WriteReg(LCD_REG_9, 0x0000); /* Set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(LCD_REG_10, 0x0000); /* FMARK function */
    LCD_WriteReg(LCD_REG_12, 0x0000); /* RGB interface setting */
    LCD_WriteReg(LCD_REG_13, 0x0000); /* Frame marker Position */
    LCD_WriteReg(LCD_REG_15, 0x0000); /* RGB interface polarity */

    /* Power On sequence -----------------------------------------------------*/
    LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    Delay_10ms(130);                     /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    Delay_10ms(50);                      /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
    Delay_10ms(50);                     /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
    Delay_10ms(50);                       /* Delay 50 ms */
    LCD_WriteReg(LCD_REG_32, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(LCD_REG_33, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve (ILI9325)---------------------------------------*/
    LCD_WriteReg(LCD_REG_48, 0x0007);
    LCD_WriteReg(LCD_REG_49, 0x0302);
    LCD_WriteReg(LCD_REG_50, 0x0105);
    LCD_WriteReg(LCD_REG_53, 0x0206);
    LCD_WriteReg(LCD_REG_54, 0x0808);
    LCD_WriteReg(LCD_REG_55, 0x0206);
    LCD_WriteReg(LCD_REG_56, 0x0504);
    LCD_WriteReg(LCD_REG_57, 0x0007);
    LCD_WriteReg(LCD_REG_60, 0x0105);
    LCD_WriteReg(LCD_REG_61, 0x0808);

    /* Set GRAM area ---------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_80, 0x0000); /* Horizontal GRAM Start Address */
    LCD_WriteReg(LCD_REG_81, 0x00EF); /* Horizontal GRAM End Address */
    LCD_WriteReg(LCD_REG_82, 0x0000); /* Vertical GRAM Start Address */
    LCD_WriteReg(LCD_REG_83, 0x013F); /* Vertical GRAM End Address */
	
    LCD_WriteReg(LCD_REG_96,  0xA700); /* Gate Scan Line(GS=1, scan direction is G320~G1) */
    LCD_WriteReg(LCD_REG_97,  0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(LCD_REG_106, 0x0000); /* set scrolling line */

    /* Partial Display Control -----------------------------------------------*/
    LCD_WriteReg(LCD_REG_128, 0x0000);
    LCD_WriteReg(LCD_REG_129, 0x0000);
    LCD_WriteReg(LCD_REG_130, 0x0000);
    LCD_WriteReg(LCD_REG_131, 0x0000);
    LCD_WriteReg(LCD_REG_132, 0x0000);
    LCD_WriteReg(LCD_REG_133, 0x0000);

    /* Panel Control ---------------------------------------------------------*/
    LCD_WriteReg(LCD_REG_144, 0x0010);
    LCD_WriteReg(LCD_REG_146, 0x0000);
    LCD_WriteReg(LCD_REG_147, 0x0003);
    LCD_WriteReg(LCD_REG_149, 0x0110);
    LCD_WriteReg(LCD_REG_151, 0x0000);
    LCD_WriteReg(LCD_REG_152, 0x0000);

    /* set GRAM write direction and BGR = 1 */
    /* I/D=00 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
  //  LCD_WriteReg(LCD_REG_3, 0x1018);  //����
    LCD_WriteReg(LCD_REG_3, 0x1030); 
    LCD_WriteReg(LCD_REG_7, 0x0133); /* 262K color and display ON */ 			
	}
	Delay_10ms(5);	                       //��ʱ50ms
	TFT_ClearScreen(BLACK);
}

/************************************************************************
** Function name:       void LCD_SetCursor(u16 Xpos, u16 Ypos)
** Descriptions:        ���ù��λ��
** input parameters:    x�����꣺uint8_t Xpos ;  y�����꣺uint16_t Ypos
** output parameters:   ��
** Returned value:      ��
**************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	
	if(DeviceCode == 0x8999)
 	{
 		LCD_WriteReg(0x4E, Xpos);
 		LCD_WriteReg(0x4F, Ypos);
  }
	else 
 	{
		LCD_WriteReg(0x20, Xpos);
		LCD_WriteReg(0x21, Ypos);
	}
}
/***********************************************************
** Function name:       void LCD_Clear(uint16_t Color)
** Descriptions:        ˢ������
** input parameters:    ��ɫֵ
** output parameters:   ��
** Returned value:      ��
************************************************************/
void TFT_ClearScreen(uint16_t Color)
{
	uint32_t index=0;      
	LCD_SetCursor(0x00,0x0000);          //���ù��λ�� 
	LCD_WriteRAM_Prepare();              //��ʼд��GRAM	  
	for(index=0;index<76800;index++)
	{
		TFT_WriteData(Color);	
	}
}

//u16 tft_id;
///****************************************************************************
//*��������TFT_GPIO_Config
//*��  �룺��
//*��  ������
//*��  �ܣ���ʼ��TFT��IO�ڡ�
//****************************************************************************/	  

//void TFT_GPIO_Config(void)
//{
////	GPIO_InitTypeDef GPIO_InitStructure;
////	
////	/* ��ʱ��ʹ�� */
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE  
////	                      | RCC_APB2Periph_GPIOG, ENABLE);

////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
////	/* FSMC_A10(G12) ��RS��G0��*/
////	GPIO_Init(GPIOG, &GPIO_InitStructure);

////	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 
////	                              | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_8 
////								  | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11
////								  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
////								  | GPIO_Pin_15 );

////	GPIO_Init(GPIOD, &GPIO_InitStructure);

////	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9
////	                               | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12
////								   | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

////	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	/* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
//	                       RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
//	                       RCC_APB2Periph_AFIO, ENABLE);
//	
//	/* Set  PD.00(D2), PD.01(D3), PD.04(NOE)--LCD_RD ������, PD.05(NWE)--LCD_WR��д��,
//	      PD.08(D13),PD.09(D14),PD.10(D15),PD.14(D0),PD.15(D1) */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | 
//								GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15  ;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                //�����������
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
//	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
//	 PE.14(D11), PE.15(D12) as alternate function push pull */
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
//	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
//	                            GPIO_Pin_15;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	
//	/* Set PF.00(A0 (RS)) as alternate function push pull */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	
//	/* Set PG.12 PE2 (LCD_CS) as alternate function push pull - CE3(LCD /CS) */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//	GPIO_Init(GPIOG, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	                 //�����������
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
//	
//}

///****************************************************************************
//* Function Name  : TFT_FSMC_Config
//* Description    : ��ʼ��FSMC
//* Input          : None
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_FSMC_Config(void)
//{
////	/* ��ʼ������ */
////	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
////	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTiming;

////	/* ���ö�дʱ�򣬸�FSMC_NORSRAMInitStructure���� */
////	/* ��ַ����ʱ�䣬3��HCLK���� */
////	FSMC_NORSRAMTiming.FSMC_AddressSetupTime = 0x02;

////	/* ��ַ����ʱ�䣬1��HCLK���� */
////	FSMC_NORSRAMTiming.FSMC_AddressHoldTime = 0x00;

////	/* ���ݽ���ʱ�䣬6��HCLK���� */
////	FSMC_NORSRAMTiming.FSMC_DataSetupTime = 0x05;

////	/* ���ݱ���ʱ�䣬1��HCLK���� */
////	FSMC_NORSRAMTiming.FSMC_DataLatency = 0x00;

////	/* ���߻ָ�ʱ������ */
////	FSMC_NORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
////	
////	/* ʱ�ӷ�Ƶ���� */
////	FSMC_NORSRAMTiming.FSMC_CLKDivision = 0x01;

////	/* ����ģʽ������ڵ�ַ/���ݲ�����ʱ��ABCDģʽ�����𲻴� */
////	FSMC_NORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_B;

////	/*����FSMC_NORSRAMInitStructure������*/
////	/* FSMC���ĸ��洢�飨bank��������ʹ�õ�һ����bank1�� */
////	/* ͬʱ����ʹ�õ���bank����ĵ� 4��RAM�� */
////	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;

////	/* ��������ʹ��SRAMģʽ */
////	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;

////	/* ʹ�õ����ݿ��Ϊ16λ */
////	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;

////	/* ����дʹ�ܴ� */
////	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;

////	/* ѡ����չģʽʹ�ܣ������ö���д�ò�ͬ��ʱ�� */
////	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
////	
////	/* ���õ�ַ�����ݸ���ʹ�ܲ��� */
////	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
////	
////	/* ���ö�дʱ�� */
////	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTiming;
////	
////	/* ����дʱ�� */
////	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTiming;

////	/* ��FSMC��ʱ�� */
////	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
////	 
////	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

////	/*!< Enable FSMC Bank1_SRAM Bank */
////	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);	


//	FSMC_NORSRAMInitTypeDef  Init;
//	FSMC_NORSRAMTimingInitTypeDef  Time;

//	/*-- FSMC Configuration ------------------------------------------------------*/
//	/*----------------------- SRAM Bank 4 ----------------------------------------*/
//	/* FSMC_Bank1_NORSRAM4 configuration */
//	Time.FSMC_AddressSetupTime = 4;
//	Time.FSMC_AddressHoldTime = 0;
//	Time.FSMC_DataSetupTime = 4;
//	Time.FSMC_BusTurnAroundDuration = 0;
//	Time.FSMC_CLKDivision = 0;
//	Time.FSMC_DataLatency = 0;
//	Time.FSMC_AccessMode = FSMC_AccessMode_A;

//	/*
//	 LCD configured as follow:
//	    - Data/Address MUX = Disable
//	    - Memory Type = SRAM
//	    - Data Width = 16bit
//	    - Write Operation = Enable
//	    - Extended Mode = Enable
//	    - Asynchronous Wait = Disable
//	*/
//	Init.FSMC_Bank = FSMC_Bank1_NORSRAM4;
//	Init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//	Init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//	Init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//	Init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//	Init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	
//	Init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//	Init.FSMC_WrapMode = FSMC_WrapMode_Disable;
//	Init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//	Init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//	Init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//	Init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//	Init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

//	Init.FSMC_ReadWriteTimingStruct = &Time;
//	Init.FSMC_WriteTimingStruct = &Time;

//	FSMC_NORSRAMInit(&Init);

//	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
//}

///****************************************************************************
//* Function Name  : TFT_WriteCmd
//* Description    : LCDд������
//* Input          : cmd��д���16λ����
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_WriteCmd(uint16_t cmd)
//{
//	TFT->TFT_CMD = cmd;
//}

///****************************************************************************
//* Function Name  : TFT_WriteData
//* Description    : LCDд������
//* Input          : dat��д���16λ����
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_WriteData(u16 dat)
//{
//	TFT->TFT_DATA = dat;
//}
///*********************************************************************************
//** Function name:     void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
//** Descriptions:      ���üĴ���
//** input parameters:  �Ĵ�����ַ:LCD_Reg   ����:RegValue
//** output parameters: ��
//** Returned value:    ��
//**********************************************************************************/
//void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
//{
//   TFT->TFT_CMD = LCD_Reg;                 //д�Ĵ���
//	TFT->TFT_DATA = LCD_RegValue;
//			    
//}
///****************************************************************************
//* Function Name  : TFT_Init
//* Description    : ��ʼ��LCD��
//* Input          : None
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_Init(void)
//{
//	uint16_t i;

//	TFT_GPIO_Config();
//	TFT_FSMC_Config();

//	TFT_WriteCmd(0xE9); 
//	TFT_WriteData(0x20); 
//	
//	TFT_WriteCmd(0x11); //Exit Sleep 
//	for(i=500; i>0; i--);
//	
//	TFT_WriteCmd(0x3A);
//	TFT_WriteData(0x55);  //16Bit colors
//	
//	TFT_WriteCmd(0xD1); 
//	TFT_WriteData(0x00); 
//	TFT_WriteData(0x65); //���Դ�ֵ����ˮ��
//	TFT_WriteData(0x1F); 
//	
//	TFT_WriteCmd(0xD0); 
//	TFT_WriteData(0x07); 
//	TFT_WriteData(0x07); 
//	TFT_WriteData(0x80); 
//	
//	TFT_WriteCmd(0x36); 	 //Set_address_mode
//	TFT_WriteData(0x48);   	//48
//	
//	TFT_WriteCmd(0xC1); 
//	TFT_WriteData(0x10); 
//	TFT_WriteData(0x10); 
//	TFT_WriteData(0x02); 
//	TFT_WriteData(0x02); 
//	
//	TFT_WriteCmd(0xC0); //Set Default Gamma 
//	TFT_WriteData(0x00);  
//	TFT_WriteData(0x35); 
//	TFT_WriteData(0x00); 
//	TFT_WriteData(0x00); 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x02); 

//	TFT_WriteCmd(0xC4);			
//	TFT_WriteData(0x03);

//	TFT_WriteCmd(0xC5); //Set frame rate 
//	TFT_WriteData(0x01); 
//	
//	TFT_WriteCmd(0xD2); //power setting 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x22); 

//	TFT_WriteCmd(0xE7);			
//	TFT_WriteData(0x38);
//	
//	TFT_WriteCmd(0xF3);			
//    TFT_WriteData(0x08); 
//	TFT_WriteData(0x12);
//	TFT_WriteData(0x12);
//	TFT_WriteData(0x08);

//	TFT_WriteCmd(0xC8); //Set Gamma 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x52); 
//	TFT_WriteData(0x37); 
//	TFT_WriteData(0x10); 
//	TFT_WriteData(0x0d); 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x04); 
//	TFT_WriteData(0x51); 
//	TFT_WriteData(0x77); 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x01); 
//	TFT_WriteData(0x0d); 
//	TFT_WriteData(0x08); 
//	TFT_WriteData(0x80); 
//	TFT_WriteData(0x00);  

//	TFT_WriteCmd(0x29); //display on



//}

///****************************************************************************
//* Function Name  : TFT_SetWindow
//* Description    : ����Ҫ�����Ĵ��ڷ�Χ
//* Input          : xStart��������ʼX����
//*                * yStart��������ʼY����
//*                * xEnd�����ڽ���X����
//*                * yEnd�����ڽ���Y����
//* Output         : None
//* Return         : None
//****************************************************************************/

void TFT_SetWindow(unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end)
{
 	  	if(DeviceCode==0x8999)
		{
	    LCD_WriteReg(0x44,x|((x_end-1)<<8));
	    LCD_WriteReg(0x45,y);
	    LCD_WriteReg(0x46,y_end-1);
		}
		else
		{
		  LCD_WriteReg(0x50, x);	   	       //ˮƽ����GRAM��ʼ��ַ
  	  LCD_WriteReg(0x51, x+x_end); 	     //ˮƽ����GRAM������ַ 
  	  LCD_WriteReg(0x52, y);		  	     //��ֱ����GRAM��ʼ��ַ
  	  LCD_WriteReg(0x53, y+y_end);       //��ֱ����GRAM������ַ
  	
		}
		 LCD_SetCursor(x, y);			       //���ù��λ��
	    LCD_WR_REG(0x22);
}


///****************************************************************************
//* Function Name  : TFT_SetWindow
//* Description    : ����Ҫ�����Ĵ��ڷ�Χ
//* Input          : xStart��������ʼX����
//*                * yStart��������ʼY����
//*                * xEnd�����ڽ���X����
//*                * yEnd�����ڽ���Y����
//* Output         : None
//* Return         : None
//****************************************************************************/

void TFT_SetWindowAPI(unsigned int x,unsigned int y,unsigned int x_end,unsigned int y_end)
{
 	  	if(DeviceCode==0x8999)
		{
	    LCD_WriteReg(0x44,x|((x_end-1)<<8));
	    LCD_WriteReg(0x45,y);
	    LCD_WriteReg(0x46,y_end-1);
		}
		else
		{
		  LCD_WriteReg(0x50, x);	   	       //ˮƽ����GRAM��ʼ��ַ
  	  LCD_WriteReg(0x51, x_end); 	     //ˮƽ����GRAM������ַ 
  	  LCD_WriteReg(0x52, y);		  	     //��ֱ����GRAM��ʼ��ַ
  	  LCD_WriteReg(0x53, y_end);       //��ֱ����GRAM������ַ
  	
		}
		 LCD_SetCursor(x, y);			       //���ù��λ��
	    LCD_WR_REG(0x22);
}
///****************************************************************************
//* Function Name  : TFT_ClearScreen
//* Description    : ��LCD��������Ӧ����ɫ
//* Input          : color��������ɫ
//* Output         : None
//* Return         : None
//****************************************************************************/
//	  
//void TFT_ClearScreen(uint16_t color)
//{
// 	uint16_t i, j ;

//	TFT_SetWindow(0, 0, TFT_XMAX, TFT_YMAX);	 //��������
//  	for(i=0; i<TFT_XMAX+1; i++)
//	{
//		for (j=0; j<TFT_YMAX+1; j++)
//		{
//			TFT_WriteData(color);
//		}
//	}
//}


