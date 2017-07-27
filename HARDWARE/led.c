#include "led.h"
#include "sys.h"
void LED_Init(void)
{
GPIO_InitTypeDef GPIO_InitStruct;
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
GPIO_InitStruct.GPIO_Speed=GPIO_Fast_Speed;		
GPIO_Init(GPIOF,&GPIO_InitStruct);

led0=1;
led1=1;

}
