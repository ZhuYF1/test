/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "MotorControl.h"
#include "HALL.h"
u8 uc1msFlag=0;//1ms执行周期
u16 uiHall = 0;//记录霍尔传感器值
int counter1 = 0;
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}
//1ms中断，用于程序执行
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
		uc1msFlag = 1;
	}

}
//霍尔采集中断
void EXTI9_5_IRQHandler(void)
{
	uiHall=GPIO_ReadInputData(GPIOB); //读取引脚值
	uiHall=uiHall&0x01c0;//采集PB6,PB7,PB8
	uiHall=uiHall>>6;  
	if((uiHall&0x07)==ucForwardHall[HallTemp])
	{
		uiCurrentLocationHallCnt++;
	}
	else if((uiHall&0x07)==ucReverseHall[HallTemp])
	{
		//if(uiCurrentLocationHallCnt>0)
		{
			uiCurrentLocationHallCnt--;
		}
	}
	else
	{
	
	}

	HallTemp=(u8)(uiHall&0x07);

	uiHallSpeedCnt++;
	uiHallSpeedBackCnt++;
	Hall_SW();
	counter1++;
	if(EXTI_GetITStatus(EXTI_Line6)!= RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	if(EXTI_GetITStatus(EXTI_Line7)!= RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	if(EXTI_GetITStatus(EXTI_Line8)!= RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}


u16 uiCurrentAD_Last=0;
u16 uiHallA_Time=0;
u16 uiHallB_Time=0;
u16 uiHallC_Time=0;
void TIM1_UP_IRQHandler(void)
{
	static u8 ucHallLast=0;
	u8 ucHallErr=0;
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		GPIOC->ODR ^= GPIO_Pin_13;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		if((TIM1->RCR==0)&&((TIM1->CNT)>100))
		{
			TIM1->RCR=1;// 波谷产生更新事件
		}
		if(uiHallSpeedTimeCnt<10000)
		{
			uiHallSpeedTimeCnt++;
		}

		if(ucReadZeroFlag==0)
		{
			if(uiHallSpeedTimeCnt>10)
			{
				uiCurrentZero=ADC1->JDR1;
				ucReadZeroFlag=1;
				if((uiCurrentZero>664)||(uiCurrentZero<464))
				{
					uiCurrentZero=564;
				}
			}
		}
		uiCurrentAD=ADC1->JDR1;
		if(uiCurrentAD>uiCurrentZero)
		{
			uiCurrentAD=uiCurrentAD-uiCurrentZero;
		}
		else
		{
			uiCurrentAD=0;
		}

		uiCurrentFbk=uiCurrentAD_Last*3/4+uiCurrentAD/4;
		uiCurrentAD_Last=uiCurrentFbk;

		if(uiHallA_Time<65000)
		{
			uiHallA_Time++;
		}
		if(uiHallB_Time<65000)
		{
			uiHallB_Time++;
		}
		if(uiHallC_Time<65000)
		{
			uiHallC_Time++;
		}
		if(keytemp!=1)
		{
			uiHallA_Time=0;
			uiHallB_Time=0;
			uiHallC_Time=0;
		}
		if(ucHallLast!=uiHall)
		{
			ucHallErr=ucHallLast^uiHall;
			if(ucHallErr&0x01)
			{
				uiHallA_Time=0;
			}
			if(ucHallErr&0x02)
			{
				uiHallB_Time=0;
			}
			if(ucHallErr&0x04)
			{
				uiHallC_Time=0;
			}
			ucHallLast=uiHall;
			SpeedData.NewData = uiHallSpeedTimeCnt;
			SpeedSum(&SpeedData);
			uiHallSpeedTimeCnt=0;
		}

		if((uiHallA_Time>30000)||(uiHallB_Time>30000)||(uiHallC_Time>30000))//63492
		{
			if((uiCurrentFbk>230)&&(keytemp==1))
			{
				uiHallA_Time=0;
				uiHallB_Time=0;
				uiHallC_Time=0;
				if(ucMotorDrection==0)
				{
					ucForewardStallFlag=1;
				}
				else
				{
					ucReversedStallFlag=1;

					if(ucCalibrationSuccessFlag==0)//左转堵死，零点
					{
						ucCalibrationSuccessFlag=1;
						uiCurrentLocationHallCnt=0;
					}
				}
			}
		}
		uiCurrentLocation=uiCurrentLocationHallCnt;
		CalculateDistance();
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
