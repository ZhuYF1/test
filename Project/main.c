#include "stm32f10x.h"
#include "Drv8301.h"
#include "Delay.h"
#include "GPIO.h"
#include "Timer.h"
#include "IWDG.h"
#include "ADC.h"
#include "HALL.h"
short ADC_ConvertedValue[5]={0,0,0,0,0};
int main(void)
{	
	RCC_Configuration();
	MY_GPIO_Init();
	delay_init(64);
	GateDriverConf();
	TIM2_Int_Init(9,6399);//1ms
	IWDG_Init(4,500);//800ms
	TIM1_Configuration1();
	ADC_Configuration1();
	DMA_Configuration1();
	Hall_Init();
}

