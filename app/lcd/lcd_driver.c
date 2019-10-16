#include "lcd_driver.h"


uint16_t DeviceCode;                                             //用于存取设备ID
#define  Font_Size 16                 		                       // 默认字体为16*8 
#define  XY        1											                       //文字支持横竖屏切换,竖屏:1;横屏:0

                                               
/*********************************************************
** Function name:       void Delay_10ms (u32 nCount)
** Descriptions:        LCD延时函数 10MS
** input parameters:    nCount
** output parameters:   无
** Returned value:      无
*********************************************************/
void Delay_10ms (u32 nCount)
{
	volatile int i;	 	
	for (i=0;i<nCount*800;i++);
}

/*********************************************************
** Function name:       void LCD_WR_REG(uint16_t LCD_Reg)
** Descriptions:        写寄存器
** input parameters:    LCD_Reg
** output parameters:   无
** Returned value:      无
*********************************************************/
void LCD_WR_REG(uint16_t LCD_Reg)
{
	LCD_REG= LCD_Reg;	 		//往指令寄存器写指令
}

/*********************************************************
** Function name:       void LCD_WR_DATA(uint16_t LCD_Data)	
** Descriptions:        写数据
** input parameters:    LCD_Data
** output parameters:   无
** Returned value:      无
*********************************************************/
void TFT_WriteData(uint16_t dat)	
{
	LCD_RAM= dat; 			//往数据寄存器写数据
} 

/*********************************************************************************
** Function name:     void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
** Descriptions:      配置寄存器
** input parameters:  寄存器地址:LCD_Reg   数据:RegValue
** output parameters: 无
** Returned value:    无
**********************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);                    //写寄存器
  TFT_WriteData(LCD_RegValue);	            //写数据
			    
}

/*********************************************************
** Function name:       void LCD_WriteRAM_Prepare(void)
** Descriptions:        开始写GRAM
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************/
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(0x22);
}


/*********************************************************
** Function name:       void LCD_WriteRAM(u16 RGB_Code)
** Descriptions:        LCD写GRAM
** input parameters:    颜色值
** output parameters:   无
** Returned value:      无
*********************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{							    
	TFT_WriteData(RGB_Code);               //写十六位GRAM
}

/*********************************************************
** Function name:       uint16_t Read_LCDReg(uint16_t LCD_Reg)
** Descriptions:        读寄存器
** input parameters:    LCD_Reg:寄存器
** output parameters:   无
** Returned value:      返回寄存器值
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
** Descriptions:        LCD相关引脚配置
** input parameters:  	无
** output parameters:   无
** Returned value:      无
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
** Descriptions:        FSMC时序配置
** input parameters:  	无
** output parameters:   无
** Returned value:      无
************************************************************************************/
void FSMC_LCDInit(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	   	//定义一个NOR初始化结构体
	FSMC_NORSRAMTimingInitTypeDef  w,r;						          //定义一个NOR时序初始化结构体

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);						//使能FSMC总线时钟

  w.FSMC_AddressSetupTime = 0x00;							                  //地址建立时间：0x1
  w.FSMC_AddressHoldTime = 0x00;							                  //地址保持时间：0x00
  w.FSMC_DataSetupTime = 0x01;							                    //数据建立时间：0x2
  w.FSMC_BusTurnAroundDuration = 0x00;
  w.FSMC_CLKDivision = 0x00;
  w.FSMC_DataLatency = 0x00;
  w.FSMC_AccessMode = FSMC_AccessMode_B;

	r.FSMC_AddressSetupTime = 0x00;							                   //地址建立时间：0x1
  r.FSMC_AddressHoldTime = 0x00;							                   //地址保持时间：0x00
  r.FSMC_DataSetupTime = 0x07;							                     //数据建立时间：0x2
  r.FSMC_BusTurnAroundDuration = 0x00;
  r.FSMC_CLKDivision = 0x00;
  r.FSMC_DataLatency = 0x0f;
  r.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;	   	//扩展NOR BANK 的第1个子BANK
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//不使用总线复用
																	                                            
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//扩展类型为NOR FLASH
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//扩展总线宽度为16位
																	                                          
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
 
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 							//根据指定参数初始化结构体
  										
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); 							//使能FSMC_Bank1_NORSRAM4 内存

}
/**********************************************************************************
** Function name:       void LCD_Init(void)
** Descriptions:        LCD初始化
** input parameters:  	无
** output parameters:   无
** Returned value:      无
************************************************************************************/
void TFT_Init(void)
{
	LCD_IOConfig();
  FSMC_LCDInit();
	Delay_10ms(5);                     // delay 50 ms 
	Delay_10ms(5);                     // delay 50 ms 
	DeviceCode = Read_LCDReg(0x0000);       // 读驱动IC
	Delay_10ms(5);                     // delay 50 ms
	//printf("lcd id is 0x%x",DeviceCode);
	//DeviceCode=0x8999;
	if(DeviceCode==0x8999)	           // 对应的驱动IC为SSD1298
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
  //  LCD_WriteReg(LCD_REG_3, 0x1018);  //横屏
    LCD_WriteReg(LCD_REG_3, 0x1030); 
    LCD_WriteReg(LCD_REG_7, 0x0133); /* 262K color and display ON */ 			
	}
	Delay_10ms(5);	                       //延时50ms
	TFT_ClearScreen(BLACK);
}

/************************************************************************
** Function name:       void LCD_SetCursor(u16 Xpos, u16 Ypos)
** Descriptions:        设置光标位置
** input parameters:    x轴坐标：uint8_t Xpos ;  y轴坐标：uint16_t Ypos
** output parameters:   无
** Returned value:      无
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
** Descriptions:        刷屏函数
** input parameters:    颜色值
** output parameters:   无
** Returned value:      无
************************************************************/
void TFT_ClearScreen(uint16_t Color)
{
	uint32_t index=0;      
	LCD_SetCursor(0x00,0x0000);          //设置光标位置 
	LCD_WriteRAM_Prepare();              //开始写入GRAM	  
	for(index=0;index<76800;index++)
	{
		TFT_WriteData(Color);	
	}
}

//u16 tft_id;
///****************************************************************************
//*函数名：TFT_GPIO_Config
//*输  入：无
//*输  出：无
//*功  能：初始化TFT的IO口。
//****************************************************************************/	  

//void TFT_GPIO_Config(void)
//{
////	GPIO_InitTypeDef GPIO_InitStructure;
////	
////	/* 打开时钟使能 */
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE  
////	                      | RCC_APB2Periph_GPIOG, ENABLE);

////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_12;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
////	/* FSMC_A10(G12) 和RS（G0）*/
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
//	/* Set  PD.00(D2), PD.01(D3), PD.04(NOE)--LCD_RD （读）, PD.05(NWE)--LCD_WR（写）,
//	      PD.08(D13),PD.09(D14),PD.10(D15),PD.14(D0),PD.15(D1) */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | 
//								GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15  ;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                //复用推挽输出
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
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	                 //复用推挽输出
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
//	
//}

///****************************************************************************
//* Function Name  : TFT_FSMC_Config
//* Description    : 初始化FSMC
//* Input          : None
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_FSMC_Config(void)
//{
////	/* 初始化函数 */
////	FSMC_NORSRAMInitTypeDef        FSMC_NORSRAMInitStructure;
////	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTiming;

////	/* 设置读写时序，给FSMC_NORSRAMInitStructure调用 */
////	/* 地址建立时间，3个HCLK周期 */
////	FSMC_NORSRAMTiming.FSMC_AddressSetupTime = 0x02;

////	/* 地址保持时间，1个HCLK周期 */
////	FSMC_NORSRAMTiming.FSMC_AddressHoldTime = 0x00;

////	/* 数据建立时间，6个HCLK周期 */
////	FSMC_NORSRAMTiming.FSMC_DataSetupTime = 0x05;

////	/* 数据保持时间，1个HCLK周期 */
////	FSMC_NORSRAMTiming.FSMC_DataLatency = 0x00;

////	/* 总线恢复时间设置 */
////	FSMC_NORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
////	
////	/* 时钟分频设置 */
////	FSMC_NORSRAMTiming.FSMC_CLKDivision = 0x01;

////	/* 设置模式，如果在地址/数据不复用时，ABCD模式都区别不大 */
////	FSMC_NORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_B;

////	/*设置FSMC_NORSRAMInitStructure的数据*/
////	/* FSMC有四个存储块（bank），我们使用第一个（bank1） */
////	/* 同时我们使用的是bank里面的第 4个RAM区 */
////	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;

////	/* 这里我们使用SRAM模式 */
////	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;

////	/* 使用的数据宽度为16位 */
////	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;

////	/* 设置写使能打开 */
////	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;

////	/* 选择拓展模式使能，即设置读和写用不同的时序 */
////	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
////	
////	/* 设置地址和数据复用使能不打开 */
////	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
////	
////	/* 设置读写时序 */
////	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTiming;
////	
////	/* 设置写时序 */
////	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTiming;

////	/* 打开FSMC的时钟 */
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
//* Description    : LCD写入命令
//* Input          : cmd：写入的16位命令
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_WriteCmd(uint16_t cmd)
//{
//	TFT->TFT_CMD = cmd;
//}

///****************************************************************************
//* Function Name  : TFT_WriteData
//* Description    : LCD写入数据
//* Input          : dat：写入的16位数据
//* Output         : None
//* Return         : None
//****************************************************************************/

//void TFT_WriteData(u16 dat)
//{
//	TFT->TFT_DATA = dat;
//}
///*********************************************************************************
//** Function name:     void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
//** Descriptions:      配置寄存器
//** input parameters:  寄存器地址:LCD_Reg   数据:RegValue
//** output parameters: 无
//** Returned value:    无
//**********************************************************************************/
//void LCD_WriteReg(uint16_t LCD_Reg ,uint16_t LCD_RegValue)
//{
//   TFT->TFT_CMD = LCD_Reg;                 //写寄存器
//	TFT->TFT_DATA = LCD_RegValue;
//			    
//}
///****************************************************************************
//* Function Name  : TFT_Init
//* Description    : 初始化LCD屏
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
//	TFT_WriteData(0x65); //调试此值改善水纹
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
//* Description    : 设置要操作的窗口范围
//* Input          : xStart：窗口起始X坐标
//*                * yStart：窗口起始Y坐标
//*                * xEnd：窗口结束X坐标
//*                * yEnd：窗口结束Y坐标
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
		  LCD_WriteReg(0x50, x);	   	       //水平方向GRAM起始地址
  	  LCD_WriteReg(0x51, x+x_end); 	     //水平方向GRAM结束地址 
  	  LCD_WriteReg(0x52, y);		  	     //垂直方向GRAM起始地址
  	  LCD_WriteReg(0x53, y+y_end);       //垂直方向GRAM结束地址
  	
		}
		 LCD_SetCursor(x, y);			       //设置光标位置
	    LCD_WR_REG(0x22);
}


///****************************************************************************
//* Function Name  : TFT_SetWindow
//* Description    : 设置要操作的窗口范围
//* Input          : xStart：窗口起始X坐标
//*                * yStart：窗口起始Y坐标
//*                * xEnd：窗口结束X坐标
//*                * yEnd：窗口结束Y坐标
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
		  LCD_WriteReg(0x50, x);	   	       //水平方向GRAM起始地址
  	  LCD_WriteReg(0x51, x_end); 	     //水平方向GRAM结束地址 
  	  LCD_WriteReg(0x52, y);		  	     //垂直方向GRAM起始地址
  	  LCD_WriteReg(0x53, y_end);       //垂直方向GRAM结束地址
  	
		}
		 LCD_SetCursor(x, y);			       //设置光标位置
	    LCD_WR_REG(0x22);
}
///****************************************************************************
//* Function Name  : TFT_ClearScreen
//* Description    : 将LCD清屏成相应的颜色
//* Input          : color：清屏颜色
//* Output         : None
//* Return         : None
//****************************************************************************/
//	  
//void TFT_ClearScreen(uint16_t color)
//{
// 	uint16_t i, j ;

//	TFT_SetWindow(0, 0, TFT_XMAX, TFT_YMAX);	 //作用区域
//  	for(i=0; i<TFT_XMAX+1; i++)
//	{
//		for (j=0; j<TFT_YMAX+1; j++)
//		{
//			TFT_WriteData(color);
//		}
//	}
//}


