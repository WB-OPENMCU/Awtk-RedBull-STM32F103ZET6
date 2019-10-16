
#include "delay.h"
#include "gui.h"
#include "stdlib.h"
#include "sys.h"
#include "tim.h"
#include "touch.h"
#include "usart.h"
#include "lcd_driver.h"
#include "24C02.h"
extern void systick_init(void);
extern int gui_app_start(int lcd_w, int lcd_h);

int main()
{	
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   systick_init();
	 Init_IIC();
  TFT_Init();
  TFT_ClearScreen(BLACK);
 TOUCH_Init();
  TIM3_Init(50, 7199);

	return gui_app_start(240, 320);
}
