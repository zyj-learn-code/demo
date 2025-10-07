#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "PWM.h"
#include "Delay.h"
#include "Encoder.h"
#include "Key.h"
#include "Direction.h"
#include "CountKey.h"
#include "Relay.h"
#include "AD.h"

uint8_t Screen_Mode = 0;
float V_Real_Value=0;
float Set_Voltage_Value=0;
float Set_I_Value=0;
uint16_t AD0_V=0;
float V_AD_Value=0;
/*�������*/
float Target, Actual, Out;			//Ŀ��ֵ��ʵ��ֵ�����ֵ
float Kp=20;
float Ki=2;
float Kd=0.1;					//����������΢�����Ȩ��
float Error0, Error1, ErrorInt=500;		//�������ϴ���������

int main(void)
{	
	OLED_Init();
	PWM_Init();
	SD_select(1);
	Encoder_Init();
	Key_Init();
	CountKey_Init();
	Relay_Init();
	AD_Init();
	
	Relay_Protect_Off();
	
	
	while(1)
	{	
		
//		OLED_Show();
		switch(Screen_Mode)
		{
			case 0:
				OLED_Clear();
				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
				OLED_ShowChinese(16,16,"��ѹ���ģʽ");
				OLED_ShowChinese(16,32,"�������ģʽ");
				OLED_Update();
				Button_Set_Mode0();
				break;
			case 1:
				OLED_Clear();
				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
				OLED_ShowChinese(16,0,"���ѹ��");
	
				OLED_ShowFloatNum(80,0,Set_Voltage_Value,2,1,OLED_8X16);
				OLED_ShowChar(120,0,'V',OLED_8X16);
			
				OLED_ShowChinese(16,16,"����");
				OLED_ShowFloatNum(48,16,ErrorInt,4,0,OLED_8X16);
				OLED_ShowNum(96,16,Out,4,OLED_8X16);
			
				OLED_ShowChinese(16,32,"���ѹ��");
				OLED_ShowChinese(16,48,"�������");

				OLED_ShowFloatNum(80,32,V_Real_Value,2,1,OLED_8X16);
				OLED_ShowChar(120,32,'V',OLED_8X16);
				OLED_Update();
				
				Button_Set_Voltage();
				AD_transform_V();
				if(V_Real_Value > 30) Relay_Protect_On();
				break;
			case 2:
				OLED_Clear();
				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
				OLED_ShowChinese(16,0,"�������");
		
				OLED_ShowFloatNum(80,0,Set_I_Value,1,2,OLED_8X16);
				OLED_ShowChar(120,0,'A',OLED_8X16);
			
				OLED_ShowChinese(16,16,"����");
				OLED_ShowChinese(16,32,"���ѹ��");
				OLED_ShowChinese(16,48,"�������");				
				OLED_Update();
			
				Button_Set_I();
				break;
			default:
				break;
		}
		
		
	}

}

void TIM3_IRQHandler(void)
{
	static uint8_t Count1=0;
	
	if (TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)		//�ж��Ƿ����ⲿ�ж�14���ߴ������ж�
	{
		Key_Tick();
		
		Count1 ++;				//�ƴ�����
		if (Count1 >= 80)		//����ƴ�40�Σ���if��������ifÿ��40ms��һ��
		{
			Count1 = 0;			//�ƴ����㣬�����´μƴ�
			
			/*��ȡʵ���ٶ�ֵ*/
			/*Encoder_Get���������Ի�ȡ���ζ�ȡ�������ļƴ�ֵ����*/
			/*��ֵ�������ٶȣ����Կ��Ա�ʾ�ٶȣ������ĵ�λ�������ٶȵı�׼��λ*/
			/*�˴�ÿ��40ms��ȡһ�μƴ�ֵ�����������תһ�ܵļƴ�ֵ����ԼΪ408*/
			/*��������ת��Ϊ��׼��λ������ת/��*/
			/*��ɽ��˾����ĳ�Actual = Encoder_Get() / 408.0 / 0.04;*/
			
			
			Actual = V_Real_Value;
			Target = Set_Voltage_Value;
			/*��ȡ���������ϴ����*/
			Error1 = Error0;			//��ȡ�ϴ����
			Error0 = Actual-Target;	//��ȡ������Ŀ��ֵ��ʵ��ֵ����Ϊ���ֵ
			
			/*�����֣��ۼӣ�*/
			/*���Ki��Ϊ0���Ž��������֣���������Ŀ���Ǳ��ڵ���*/
			/*��Ϊ�ڵ���ʱ�����ǿ����Ȱ�Ki����Ϊ0����ʱ�����������ã�����������ˣ������ֻ���۵��ܴ��ֵ*/
			/*����һ��Ki��Ϊ0����ô��Ϊ�������Ѿ����۵��ܴ��ֵ�ˣ���͵��»�����������������ڵ���*/
			
			ErrorInt += Error0;		//����������
			
			if (ErrorInt > 500) {ErrorInt = 500;}		//�������������Ϊ500
			if (ErrorInt < 225) {ErrorInt = 225;}		//������������СΪ-500
			/*PID����*/
			/*ʹ��λ��ʽPID��ʽ������õ����ֵ*/
			Out = Kp * Error0 + Ki * ErrorInt + Kd * (Error0 - Error1);
			
			/*����޷�*/
			if (Out > 1000) {Out = 1000;}		//�������ֵ���Ϊ100
			if (Out < 450) {Out = 450;}	//�������ֵ��СΪ100
			
			/*ִ�п���*/
			/*���ֵ�������PWM*/
			/*��Ϊ�˺��������뷶Χ��-100~100��������������޷�����Ҫ��Outֵ�޶���-100~100*/
			PWM_SetCompare1(Out);
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);		//����ⲿ�ж�14���ߵ��жϱ�־λ		
	}
}

