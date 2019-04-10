#include "Timer.h"
void TIM1_Configuration1(void)
{			
	TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
	TIM_OCInitTypeDef TIM1_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* TIM1 Registers reset */
	/* Enable TIM1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//使能APB2时钟
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
	/* Time Base configuration */
	TIM1_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;//上下计数模式
	TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD;
	TIM1_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;

	// Initial condition is REP=0 to set the UPDATE only on the underflow
	TIM1_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
	TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);

	TIM1->CR2=0X0020;//更新事件作为触发源

	TIM_OCStructInit(&TIM1_OCInitStructure);
	/* Channel 1, 2,3 in PWM mode */
	TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                 
	TIM1_OCInitStructure.TIM_Pulse = 0x505; //dummy value
	TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       

	TIM_OC1Init(TIM1, &TIM1_OCInitStructure); 
	TIM_OC2Init(TIM1, &TIM1_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM1_OCInitStructure);           
	TIM_OC4Init(TIM1, &TIM1_OCInitStructure);


	/* Enables the TIM1 Preload on CC1 Register */
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* Enables the TIM1 Preload on CC2 Register */
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* Enables the TIM1 Preload on CC3 Register */
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* Enables the TIM1 Preload on CC4 Register */
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* TIM1 counter enable */

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//TIM2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIM3在ARR上的预装载寄存器
	TIM_CtrlPWMOutputs(TIM1, DISABLE); 
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_Cmd(TIM1, ENABLE);
}



void RCC_Configuration(void)
{
    //ErrorStatus HSEStartUpStatus;
    u32 tmpreg = 0;
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();
    /* Enable HSE */
    RCC_HSICmd(ENABLE);
    /* Wait till HSE is ready */
    //HSEStartUpStatus = RCC_WaitForHSEStartUp();
    //if(HSEStartUpStatus == SUCCESS)
    {
    /* HCLK = SYSCLK */
    // RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC->CFGR &=0xFFFFFF0F;//CFGR_HPRE_Reset_Mask;//
    /* PCLK2 = HCLK */
    // RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC->CFGR &=0xFFFFC7FF;//CFGR_PPRE2_Reset_Mask;//
    /* PCLK1 = HCLK/2 */
    // RCC_PCLK1Config(RCC_HCLK_Div2);
    tmpreg = RCC->CFGR;//
    tmpreg &= 0xFFFFF8FF;//CFGR_PPRE1_Reset_Mask;
    tmpreg |= 0x00000400;//RCC_HCLK_Div2;
    RCC->CFGR = tmpreg;//APB1 2分频输出
    /* Flash 2 wait state */
    // FLASH_SetLatency(FLASH_Latency_2);
    FLASH->ACR &=0x00000038;//ACR_LATENCY_Mask;//
    FLASH->ACR |= 0x00000002;//FLASH_Latency_2;//
    /* Enable Prefetch Buffer */
    //
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH->ACR &= 0xFFFFFFEF;//ACR_PRFTBE_Mask;//
    FLASH->ACR |=0x00000010;//FLASH_PrefetchBuffer_Enable;//
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    /* PLLCLK = 8MHz/2 * 16 = 64 MHz */
    // RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
    tmpreg = RCC->CFGR;//
    tmpreg &= 0xFFC0FFFF;//CFGR_PLL_Mask;
    tmpreg |= 0x00388000;//RCC_PLLMul_16
    RCC->CFGR = tmpreg;
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    //asm("nop");//151126-1
    }
    /* Select PLL as system clock source */
    // RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    tmpreg = RCC->CFGR;//
    tmpreg &= 0xFFFFFFFC;//CFGR_SW_Mask;
    tmpreg |= 0x00000002;//RCC_SYSCLKSource_PLLCLK;
    RCC->CFGR = tmpreg;
    while(((u8)(RCC->CFGR & 0x0000000C)) != 0x08)//
    {
    //asm("nop");//151126-1
    }
    }
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}

