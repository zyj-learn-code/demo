#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
#include "Direction.h"
#include "Key.h"
#include "OLED.h"
#include "Encoder.h"
#include "Delay.h"
#include "CountKey.h"
#include "AD.h"
#include "Relay.h"
#include "PWM.h"


//float Set_I_Value=0;

//uint16_t AD0_V=0;
uint16_t Duty_Cycle=3950;

uint16_t AD1_I;
uint16_t count;		//采样计数
uint8_t Count_Speed_Adjust=0;
uint8_t Count_Speed_Para=0;

float V_Set_Median;

int16_t Direction_Site(void)
{
	uint8_t direction=0;
	
	Count1=Count2;
	Count2=Encoder_Get()/2;
	
	if(Count1 > Count2)direction=2;
	else if(Count1 < Count2) direction=1;
	
	if(direction==1)
	{
		Y=Y+16;
		direction=0;
	}
	else if(direction==2)
	{
		Y=Y-16;
		direction=0;
	}
	
	switch(Screen_Mode)
	{
		case 0:
			if(Y>32)Y=16;
			else if(Y<16)Y=32;
			break;
		case 1:
			if(Y>48)Y=0;
			else if(Y<0)Y=48;
			break;
		case 2:
			if(Y>48)Y=0;
			else if(Y<0)Y=48;
			break;
		default:
			break;
	}
	
	return Y;
}

void OLED_Show(void)
{
//	switch(Screen_Mode)
//		{
//			case 0:
//				OLED_Clear();
//				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
//				OLED_ShowChinese(16,16,"恒压输出模式");
//				OLED_ShowChinese(16,32,"恒流输出模式");
//				OLED_Update();
//				Button_Set_Mode0();
//				break;
//			case 1:
//				OLED_Clear();
//				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
//				OLED_ShowChinese(16,0,"设电压：");
//	
//				OLED_ShowFloatNum(80,0,Set_Voltage_Value,2,1,OLED_8X16);
//				OLED_ShowChar(120,0,'V',OLED_8X16);
//			
//				OLED_ShowChinese(16,16,"返回");
//				OLED_ShowNum(56,16,AD0_V,4,OLED_8X16);
////				OLED_ShowNum(96,16,Duty_Cycle,4,OLED_8X16);
//			
//				OLED_ShowChinese(16,32,"测电压：");
//				OLED_ShowChinese(16,48,"测电流：");				

//				OLED_ShowFloatNum(80,32,V_Real_Value,2,1,OLED_8X16);
//				OLED_ShowChar(120,32,'V',OLED_8X16);
//				OLED_Update();
//				
//				Button_Set_Voltage();
//				AD_transform_V();
//				
//				break;
//			case 2:
//				OLED_Clear();
//				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
//				OLED_ShowChinese(16,0,"设电流：");
//		
//				OLED_ShowFloatNum(80,0,Set_I_Value,1,2,OLED_8X16);
//				OLED_ShowChar(120,0,'A',OLED_8X16);
//			
//				OLED_ShowChinese(16,16,"返回");
//				OLED_ShowChinese(16,32,"测电压：");
//				OLED_ShowChinese(16,48,"测电流：");				
//				OLED_Update();
//			
//				Button_Set_I();
//				break;
//			default:
//				break;
//		}
}

void Button_Set_Mode0(void)
{
	if(Key_GetNum()==1 && Screen_Mode==0)
	{
		if(Direction_Site()==16) Screen_Mode=1;//恒压界面
		else Screen_Mode=2; //恒流界面
	}
}

void Button_Set_Voltage(void)
{
	if(Key_GetNum()==1 && Screen_Mode==1)
	{
		if(Direction_Site()==0)
		{	
			do{
				V_ChangeValue();
				OLED_ShowFloatNum(80,0,V_Set_Median,2,1,OLED_8X16);
				OLED_Update();
				
			}while(Key_GetNum()==0);
			Set_Voltage_Value = V_Set_Median;
		}
		else if(Direction_Site()==16)
		{
			Screen_Mode=0;
		}
		
	}
}

void V_ChangeValue(void)
{
	uint8_t direction=0;
	
	Count1=Count2;
	Count2=Encoder_Get()/2;
	
	if(Count1 > Count2)direction=2;
	else if(Count1 < Count2) direction=1;
	
	if(direction==1)
	{
		direction=0;
		V_Set_Median=V_Set_Median+1.1;
	}
	else if(direction==2)
	{
		direction=0;
		V_Set_Median=V_Set_Median-0.9;
		if(V_Set_Median<0) V_Set_Median=0;
	}
	
}

void I_ChangeValue(void)
{
	uint8_t direction=0;
	
	Count1=Count2;
	Count2=Encoder_Get()/2;
	
	if(Count1 > Count2)direction=2;
	else if(Count1 < Count2) direction=1;
	
	if(direction==1)
	{
		direction=0;
		Set_I_Value=Set_I_Value+0.11;
	}
	else if(direction==2)
	{
		direction=0;
		Set_I_Value=Set_I_Value-0.09;
		if(Set_I_Value<0) Set_I_Value=0;
	}
	
}

void Button_Set_I(void)
{
	if(Key_GetNum()==1 && Screen_Mode==2)
	{
		if(Direction_Site()==0)
		{	
			do{
				I_ChangeValue();
				OLED_ShowFloatNum(80,0,Set_I_Value,1,2,OLED_8X16);
				OLED_Update();
				
			}while(Key_GetNum()==0);	
		}
		else if(Direction_Site()==16)
		{
			Screen_Mode=0;
		}
	}
}

void AD_transform_V(void)
{
	
	AD0_V_Count=AD0_V_Count+AD_GetValue(ADC_Channel_0);
	count++;
	
	if(count>=10)
	{
		AD0_V=AD0_V_Count/count;
		AD0_V_Count=0;
		count=0;
		V_AD_Value=AD0_V*3.3/4095.0;
	}
	
//	V_Real_Value=(float)(AD0_V*3.3*V_Gain/4095.0);	
	V_Real_Value=V_AD_Value*20;
}


void AD_transform_I(void)
{
	AD1_I = AD_GetValue(ADC_Channel_1);
}
