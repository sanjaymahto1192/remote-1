/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: usart.c
* Type: C code file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/
/* include headers */
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "usart.h"
#include "xprintf.h"
/*USARTx send byte */
void usart_sendb(USART_TypeDef * USART, char byte) {
    while ((USART->SR & USART_SR_TXE) == 0);
    USART->DR = byte;
}
/*USARTx get byte */
char usart_getb(USART_TypeDef * USART) {
    while ((USART->SR & USART_SR_RXNE) == 0);
    return USART->DR;
}
/*USART1 send byte */
void ser1_sendb(unsigned char byte) {
    usart_sendb(USART1, byte);
}
/*USART1 get byte */
unsigned char ser1_getb(void) {
    return usart_getb(USART1);
}
/*USART1 setup */
void usart1_setup(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    /* Enable USART1 and GPIOB clock              */
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    /* Configure USART1 Tx (PB6) and Rx (PB7) as af            */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Setup AF fuctions for Tx, Rx pins */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    /* USART1 configured as follow:
	- BaudRate = 115200 baud
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	- USART Clock disabled
	- USART CPOL: Clock is active low
	- USART CPHA: Data is captured on the middle
	- USART LastBit: The clock pulse of the last data bit is not output to
	the SCLK pin
    */
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    /* enable usart interrupts */
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* preinit xprintf lib
     * route xprintf output function to ser1_sendb function
     */
    xdev_out(ser1_sendb);
}