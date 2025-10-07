#include "stm32f10x.h"                  // Device header
#include "Relay.h"

void Relay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);			//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//��PA6��PA7���ų�ʼ��Ϊ��������
	
}

void Relay_Protect_On(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
}

void Relay_Protect_Off(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}

