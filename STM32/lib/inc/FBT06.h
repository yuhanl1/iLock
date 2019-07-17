#ifndef FBT06_H
#define FBT06_H

#include "stm32f10x.h"

int TimeoutFlag;
int16_t WriteATCmd(USART_TypeDef* USARTx, char *cmd, uint16_t size);
int16_t HC_05_ATName(char *name);
int16_t ReadResponse(USART_TypeDef* USARTx, char *Str);

#endif
