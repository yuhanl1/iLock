#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "FBT06.h"

// ��������д��ָ����С������
int16_t WriteATCmd(USART_TypeDef* USARTx, char *cmd, uint16_t size)
{
    uint16_t i = 0;
    if(cmd != NULL)
    {
        for(i=0; i<size; i++)
        {
            USART_SendData(USARTx, cmd[i]);
        }
        // ע�����Ҫд�� \r\n
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
    // ���ģ����Ӧ�ַ��������飬����� ��ĳ��ȿ��Ը���ATָ��ĵ�����Ӧ���ַ�������������
    char respon[10] = {0};        
    char AT_NAME_CMD[20] = "AT+NAME=";
    name_size = strlen(name);
    cmd_size = strlen(AT_NAME_CMD);
    // ��name�ĳ��ȹ���ֻȡ20-cmd_size���ַ�
    if((name_size+cmd_size) > 20)
        strncat(AT_NAME_CMD, name, 20-cmd_size);
    else
        strcat(AT_NAME_CMD, name);
    cmd_size = strlen(AT_NAME_CMD);
    // ͨ��STM32��USART1������д������AT_NAME_CMD����д��cmd_size��С
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
        // ÿ�δӴ��ڶ�ȡһ���ַ�ǰ��Ҫ������ʱ��
        //TIM2_Config_Init(1000);
        TIM_Cmd(TIM2, ENABLE);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
        {
            // ��ʾ�Ѷ�ȡ��ĩβ
            if(Status == 1)
            {
                size = strlen(Str);
                TIM_Cmd(TIM2, DISABLE);
                break;
            }
            // ��ʾ�Ѿ���ʱ����ʱ���ٵȴ��Ӵ����ж�ȡ����
            if(TimeoutFlag == 1)
            {
                break;
            }
        }
        if(TimeoutFlag != 1)   // δ��ʱ
        {
            if(Status != 1)           // ��ȡδ����
            {
                data = USART_ReceiveData(USARTx);  // �Ӵ����ж�ȡ����
                // ֻ�д�ŷ� \r�� \n���ַ�
                if((data != '\r') && (data != '\n'))    
                {
                    Str[pos++] = data;
                    if(pos > 1)
                    {
                        // �жϵ������ĺ͵������ַ��Ƿ���"OK"������������Ѷ�ȡ��ĩβ(ע:������һ�͵ڶ����ַ���\r\n���ѱ����˵�)
                        if((Str[pos-2] == 'O') && (Str[pos-1] == 'K'))      
                        {
                            Str[pos] = '\0';                // Ϊ�˱��ڼ��㣬�ڻ�ȡ���ַ����м����ַ�������־
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
