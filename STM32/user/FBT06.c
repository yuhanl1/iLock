#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "FBT06.h"

// 往串口中写入指定大小的数据
int16_t WriteATCmd(USART_TypeDef* USARTx, char *cmd, uint16_t size)
{
    uint16_t i = 0;
    if(cmd != NULL)
    {
        for(i=0; i<size; i++)
        {
            USART_SendData(USARTx, cmd[i]);
        }
        // 注意最后要写入 \r\n
        USART_SendData(USARTx, 0x0d);
        USART_SendData(USARTx, 0x0a);
        return 0;
    }
    else
    {
        printf("cmd is NULL\n");
        return -1;
    }
}



int16_t HC_05_ATName(char *name)
{
    uint16_t cmd_size = 0;
    uint16_t name_size = 0;
    int16_t ret = 0;
    int16_t res_size = 0;
    // 存放模块响应字符串的数组，这个数 组的长度可以根据AT指令集文档中响应的字符串长度做调整
    char respon[10] = {0};        
    char AT_NAME_CMD[20] = "AT+NAME=";
    name_size = strlen(name);
    cmd_size = strlen(AT_NAME_CMD);
    // 当name的长度过大，只取20-cmd_size个字符
    if((name_size+cmd_size) > 20)
        strncat(AT_NAME_CMD, name, 20-cmd_size);
    else
        strcat(AT_NAME_CMD, name);
    cmd_size = strlen(AT_NAME_CMD);
    // 通过STM32的USART1往串口写入数据AT_NAME_CMD，共写入cmd_size大小
    ret = WriteATCmd(USART1, AT_NAME_CMD, cmd_size);
    if(ret == 0)
    {
        printf("Write AT_NAME_CMD OK\n");
    }
    else
    {
        printf("Write AT_NAME_CMD ERROR\n");
    }
    res_size = ReadResponse(USART2, respon);
    //ret = isResponseOK(res_size, respon);
    return ret; 
}



int16_t ReadResponse(USART_TypeDef* USARTx, char *Str)
{
    uint16_t data = 0;
    uint16_t Status = 0;    
    uint16_t pos = 0;
    int16_t size = 0;
    while(1)
    {
        // 每次从串口读取一个字符前都要启动定时器
        //TIM2_Config_Init(1000);
        TIM_Cmd(TIM2, ENABLE);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
        {
            // 表示已读取到末尾
            if(Status == 1)
            {
                size = strlen(Str);
                TIM_Cmd(TIM2, DISABLE);
                break;
            }
            // 表示已经超时，此时不再等待从串口中读取数据
            if(TimeoutFlag == 1)
            {
                break;
            }
        }
        if(TimeoutFlag != 1)   // 未超时
        {
            if(Status != 1)           // 读取未结束
            {
                data = USART_ReceiveData(USARTx);  // 从串口中读取数据
                // 只有存放非 \r和 \n的字符
                if((data != '\r') && (data != '\n'))    
                {
                    Str[pos++] = data;
                    if(pos > 1)
                    {
                        // 判断倒数第四和第三个字符是否是"OK"，若是则表明已读取到末尾(注:倒数第一和第二个字符是\r\n，已被过滤掉)
                        if((Str[pos-2] == 'O') && (Str[pos-1] == 'K'))      
                        {
                            Str[pos] = '\0';                // 为了便于计算，在获取的字符串中加入字符结束标志
                            Status = 1;
                        }
                    }
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            TimeoutFlag = 0;
            size = -1;
            printf("Read AT_Cmd response timeout\n");
            break;
        }       
    }
    return size;
}
