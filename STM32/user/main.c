#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
//#include "FBT06.h"

#define StartAddr  ((u32)0x08002000)
#define EndAddr    ((u32)0x08002500)
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08008000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800C000)

void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void EXTI_Configuration(void);
void TIM_Configuration(void);
void USART1_Putts(char *str);
void DMA_Configuration(void);
void OpenTheLock(void);

char receive_data[256];
int receive_flag = 0;
int jiou = 0;
uint16_t Uart1_Buffer[128];
uint16_t Uart1_Rx = 0, Uart1_Tx = 0, Uart1_Len = 0;
uint8_t Uart1_Sta = 0;
volatile FLASH_Status FLASHStatus;
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
volatile TestStatus MemoryProgramStatus;
char string1[17] = "#w1zHeL10l4iw3ei";
			char string2[17] = "s97h3i5n9i2d300a";
			char string3[17] = "yenid40ayes9huiw";
			char string4[17] = "angonc9u0obgwoch";
			char string5[17] = "4liwe265iguoy34a";
			char string6[17] = "ocong8shui2l1iwe";
			char string7[17] = "7i9liw8eish5u8ig";
			char string8[17] = "uoya3oijiaowochu";
int main(void)
{
	int state = 0;
	int flag = 1;
	int i,length = 0;
	uint32_t Address;
	uint16_t halfData;
	uint32_t Data;
	uint16_t imei[15];
	uint32_t NbrOfPage = 0x00;
	uint32_t EraseCounter = 0x00;
	
	RCC_Configuration();
	DMA_Configuration();
	USART_Configuration();
	GPIO_Configuration();
	NVIC_Configuration();
	EXTI_Configuration();
	
	
	while(1){	
		//if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
		//{
		//	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
		//}
		while(receive_flag == 0)
		{
		}
    receive_flag = 0;
		
	  //Head is I means IMEI state = 1
	  //Head is O means OPEN state = 2
	  //Head is @ means KEY state = 3
	  //Tail is D means END
		length = strlen(receive_data);
		/*
		if(length == 17 && receive_data[0] == 'I')
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);//Lighten LED
			
		}
		else if(length == 17 && receive_data[0] == 'O')
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_9);//Lighten LED
			OpenTheLock();
		}
		*/
		
		switch(receive_data[0])
		{
			case 'I':state = 1;break;//imei init
			case 'O':state = 2;break;//open
			case 'A':state = 4;break;//add
			default: state = 3;break;//key
		}
	
		if(state == 1 && length == 17)                      //I000000000000000D
		{
			//GPIO_ResetBits(GPIOB,GPIO_Pin_8);//Lighten LED
			
			FLASHStatus = FLASH_COMPLETE;
			
			FLASH_UnlockBank1();
			NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;
			FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
			for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
			{
				FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
			}
			
			
			Address = BANK1_WRITE_START_ADDR;
			Data = *(vu32*)Address;
			
			if(Data != 0x494D4549)//init
			{
				FLASHStatus = FLASH_ProgramWord(Address, 0x494d4549);
				Data = *(vu32*)Address;
				halfData = 0x0000;
				Address += 4;
				
				for(i = 1;i < length - 1;i++,Address = Address + 2)
				{
					halfData = receive_data[i];
					while(FLASHStatus != FLASH_COMPLETE)
					{
					}
					FLASHStatus = FLASH_ProgramHalfWord(Address, halfData);
				}
				FLASHStatus = FLASH_ProgramHalfWord(Address, 0x44); // 'D'
			}
			FLASH_LockBank1();
		}                                   
		else if(state == 2 && length == 17)                 //O000000000000000D
		{
			FLASHStatus = FLASH_COMPLETE;
			
			FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
			
			Address = BANK1_WRITE_START_ADDR;
			
			while(*(vu32*)Address == 0x494D4549)//Maybe more than 1 IMEI.
			{
				//GPIO_ResetBits(GPIOB,GPIO_Pin_9);
				Address += 4;
				for(i = 0; i < 15 ; i++)
				{
					imei[i] = *(vu16*)Address;
					Address += 2;
				}
				Address += 2; // 'D'
				for(i = 0; i < 15 ;i++)
				{
					if(imei[i] != receive_data[i + 1])
					{
						flag = 0;
						break;
					}
				}
				if(flag == 1)
					OpenTheLock();
			}
			flag = 1;
		}
		else if(state == 3)                 //KEY later version
		{
			flag = 1;
			OpenTheLock();
			for(i=0;i<17;i++)
			{
				if(receive_data[i+1] != string1[i])
				{
					flag = 0;
					break;
				}
			}
			
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+17] != string2[i])
					{
						flag = 0;
						break;
					}
				}
			}
			/*
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+33] != string3[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+49] != string4[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+65] != string5[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+81] != string6[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+97] != string7[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+113] != string8[i])
					{
						flag = 0;
						break;
					}
				}
			}
			if(flag == 1)
			{
				for(i=0;i<17;i++)
				{
					if(receive_data[i+17] != string2[i])
					{
						flag = 0;
						break;
					}
				}
			}
			*/
			if(flag == 1)
			{
				OpenTheLock();
			}
		}
		else if(state == 4 && length == 33)                 //ADD A000000000000000,000000000000001D
		{
			FLASHStatus = FLASH_COMPLETE;
			
			FLASH_UnlockBank1();
			FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
			
			Address = BANK1_WRITE_START_ADDR;
			Data = *(vu32*)Address;
			
			if(Data == 0x494D4549)//Maybe more than 1 IMEI.
			{
				Address += 4;
				for(i = 0; i < 15 ; i++)
				{
					imei[i] = *(vu16*)Address;
					Address += 2;
				}
				Address += 2;//'D'
				for(i = 0; i < 15 ;i++)
				{
					if(imei[i] != receive_data[i + 1])
					{
						flag = 0;
						break;
					}
				}
				if(flag == 1)
				{
					Data = *(vu32*)Address;
					while(Data == 0x494D4549)
					{
						Address += 4;
						Address += 32;
					}
					FLASHStatus = FLASH_ProgramWord(Address, 0x494d4549);
					
					halfData = 0x0000;
					Address += 4;
				
					for(i = 17;i < length - 1;i++,Address = Address + 2)
					{
						halfData = receive_data[i];
						while(FLASHStatus != FLASH_COMPLETE)
						{
						}
						FLASHStatus = FLASH_ProgramHalfWord(Address, halfData);
					}
					FLASHStatus = FLASH_ProgramHalfWord(Address, 0x44);
				}
			}
			flag = 1;
			FLASH_LockBank1();
		}
		state = 0;
	}
}


void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
	
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
   
  /* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2 , ENABLE);
}



void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
/*
#ifdef  VECT_TAB_RAM  
  // Set the Vector Table base location at 0x20000000 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  // VECT_TAB_FLASH
  // Set the Vector Table base location at 0x08000000 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

	*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
}



void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

  //PC10 Relay Out_PP
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_Init(GPIOC, &GPIO_InitStructure);
  //GPIO_ResetBits(GPIOC,GPIO_Pin_10);
  
	//PC0.PC1 Button FLOATING_IN
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//PB8.PB9 LED OUT_PP
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	
	//PA9 USART2 TX AF_PP
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	//PA10 USART2 RX IN_FLOATING
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}



void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}



void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
	
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  

  USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	USART_Cmd(USART2, ENABLE);
}
void DMA_Configuration(void)
{
	DMA_InitTypeDef    DMA_Initstructure;
 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  
	DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);;
  DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)receive_data;
  DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_Initstructure.DMA_BufferSize = 256;
  DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
  DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
  DMA_Initstructure.DMA_Priority = DMA_Priority_High;
  DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel6,&DMA_Initstructure);
	
  DMA_Cmd(DMA1_Channel6,ENABLE);
	//DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);
}

void OpenTheLock(void)
{
	int i = 0;
	int j = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	for(i=0;i<=1000000;i++)
	for(j=0;j<=10;j++);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	//µç´ÅËø
	
	/*
	//int i = 0;
	//int j = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(jiou == 0)
	{
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	jiou = 1;
	}
	//for(i=0;i<=1000000;i++)
	//for(j=0;j<=10;j++);
	else if(jiou == 1)
	{
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	jiou = 0;
	}
	*/
}



