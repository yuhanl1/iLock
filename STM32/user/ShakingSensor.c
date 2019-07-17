#include "stm32f10x.h"

void GPIO_ShakingSensor_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure PB8 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure PA2 as input floating (EXTI Line2)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA2 use as shaking sensor input. PB8 use as LED indicator light.
	
	/* Connect EXTI Line3 to PA.2 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
	/* Configure EXTI Line3 to generate an interrupt on falling edge */
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void GPIO_TemperatureHumiditySensor_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure PB9 as Output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure PA1 as input floating (EXTI Line1)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA1 use as temperature and humidity sensor input. PB9 use as LED indicator light when tmp or humidity beyond a value.
	
	/* Connect EXTI Line3 to PA.1 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	/* Configure EXTI Line4 to generate an interrupt on falling edge */
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void GPIO_Enable_GPIOClock(void)
{
	/* Enable GPIOA and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
}
void GPIO_Disable_GPIOClock(void)
{
	/* Enable GPIOA and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, DISABLE);
}

void EXTI3_IRQHandler(void)
{
	/* Clear the EXTI line 3 pending bit */
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	/* Clear the EXTI line 4 pending bit */
	EXTI_ClearITPendingBit(EXTI_Line4);
}
