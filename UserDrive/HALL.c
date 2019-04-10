#include "HALL.h"
#include "MotorControl.h"
u16 motor_statue = 0; 
u16 My_PWM = 100;
extern u16 uiHall;
void Hall_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	/* 配置Hall接口IO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*霍尔信号线中断配置*/
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
 
	EXTI_InitStructure.EXTI_Line = EXTI_Line6|EXTI_Line7|EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void Hall_SW(void) //六步换向
{
	u8 ucHallTemp=0;
	if(ucMotorDrection==1)
	{
		ucHallTemp=(u8)(uiHall^0x07);
	}
	else
	{
		ucHallTemp=uiHall;
	}
	motor_statue=1;
	switch(ucHallTemp)
	{
		case 1:     //original 5
			/* Next step: Step 2 Configuration ---------------------------- */
			/* Channel3 configuration */

			TIM1->CCR2=0;             //AB  电机UV线通
			TIM1->CCR1 = My_PWM;
			TIM1->CCR3=0;
			//GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
			//GPIO_SetBits(GPIOB, GPIO_Pin_14);
			GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);

			break;
		case 3:  //original 1
			/* Next step: Step 3 Configuration ---------------------------- */
			/* Channel2 configuration */

			TIM1->CCR2=0;              //AC  电机UW线通
			TIM1->CCR1 = My_PWM;
			TIM1->CCR3=0;

			//		   GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
			//		   GPIO_SetBits(GPIOB, GPIO_Pin_15);
			GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);

			break;
		case 2:  //original 5
			/* Next step: Step 4 Configuration ---------------------------- */

			TIM1->CCR1=0;          //BC     电机VW线通
			TIM1->CCR2 = My_PWM;
			TIM1->CCR3=0;

			//		   GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
			//		   GPIO_SetBits(GPIOB, GPIO_Pin_15);
			GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);

			break;

		case 6:    //original 2
			/* Next step: Step 5 Configuration ---------------------------- */

			TIM1->CCR1=0;        //BA        电机VU线通
			TIM1->CCR2 = My_PWM;
			TIM1->CCR3=0;

			//		   GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
			//		   GPIO_SetBits(GPIOB, GPIO_Pin_13);
			GPIO_SetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);
			break;

		case 4:   //original 6
			/* Next step: Step 6 Configuration ---------------------------- */

			TIM1->CCR2=0;//CA                电机WU线通
			TIM1->CCR3 = My_PWM;
			TIM1->CCR1=0;

			//		   GPIO_ResetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
			//		   GPIO_SetBits(GPIOB, GPIO_Pin_13);
			GPIO_SetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);
			break;

		case 5:     //original 4
			/* Next step: Step 1 Configuration ---------------------------- */

			TIM1->CCR2=0; //CB                电机WV线通
			TIM1->CCR3 = My_PWM;
			TIM1->CCR1=0;

			//		   GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
			//		   GPIO_SetBits(GPIOB, GPIO_Pin_14);
			GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);

			break;


		default:
		/* Next step: Step 1 Configuration ---------------------------- */
		/* Channel1 configuration */
		break;
	}
}
