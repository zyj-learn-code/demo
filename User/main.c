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
/*定义变量*/
float Target, Actual, Out;			//目标值，实际值，输出值
float Kp=20;
float Ki=2;
float Kd=0.1;					//比例项，积分项，微分项的权重
float Error0, Error1, ErrorInt=500;		//本次误差，上次误差，误差积分

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
				OLED_ShowChinese(16,16,"恒压输出模式");
				OLED_ShowChinese(16,32,"恒流输出模式");
				OLED_Update();
				Button_Set_Mode0();
				break;
			case 1:
				OLED_Clear();
				OLED_ShowImage(1,Direction_Site(),16,16,triangle);
				OLED_ShowChinese(16,0,"设电压：");
	
				OLED_ShowFloatNum(80,0,Set_Voltage_Value,2,1,OLED_8X16);
				OLED_ShowChar(120,0,'V',OLED_8X16);
			
				OLED_ShowChinese(16,16,"返回");
				OLED_ShowFloatNum(48,16,ErrorInt,4,0,OLED_8X16);
				OLED_ShowNum(96,16,Out,4,OLED_8X16);
			
				OLED_ShowChinese(16,32,"测电压：");
				OLED_ShowChinese(16,48,"测电流：");

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
				OLED_ShowChinese(16,0,"设电流：");
		
				OLED_ShowFloatNum(80,0,Set_I_Value,1,2,OLED_8X16);
				OLED_ShowChar(120,0,'A',OLED_8X16);
			
				OLED_ShowChinese(16,16,"返回");
				OLED_ShowChinese(16,32,"测电压：");
				OLED_ShowChinese(16,48,"测电流：");				
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
	
	if (TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)		//判断是否是外部中断14号线触发的中断
	{
		Key_Tick();
		
		Count1 ++;				//计次自增
		if (Count1 >= 80)		//如果计次40次，则if成立，即if每隔40ms进一次
		{
			Count1 = 0;			//计次清零，便于下次计次
			
			/*获取实际速度值*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*此值正比于速度，所以可以表示速度，但它的单位并不是速度的标准单位*/
			/*此处每隔40ms获取一次计次值增量，电机旋转一周的计次值增量约为408*/
			/*因此如果想转换为标准单位，比如转/秒*/
			/*则可将此句代码改成Actual = Encoder_Get() / 408.0 / 0.04;*/
			
			
			Actual = V_Real_Value;
			Target = Set_Voltage_Value;
			/*获取本次误差和上次误差*/
			Error1 = Error0;			//获取上次误差
			Error0 = Actual-Target;	//获取本次误差，目标值减实际值，即为误差值
			
			/*误差积分（累加）*/
			/*如果Ki不为0，才进行误差积分，这样做的目的是便于调试*/
			/*因为在调试时，我们可能先把Ki设置为0，这时积分项无作用，误差消除不了，误差积分会积累到很大的值*/
			/*后续一旦Ki不为0，那么因为误差积分已经积累到很大的值了，这就导致积分项疯狂输出，不利于调试*/
			
			ErrorInt += Error0;		//进行误差积分
			
			if (ErrorInt > 500) {ErrorInt = 500;}		//限制误差积分最大为500
			if (ErrorInt < 225) {ErrorInt = 225;}		//限制误差积分最小为-500
			/*PID计算*/
			/*使用位置式PID公式，计算得到输出值*/
			Out = Kp * Error0 + Ki * ErrorInt + Kd * (Error0 - Error1);
			
			/*输出限幅*/
			if (Out > 1000) {Out = 1000;}		//限制输出值最大为100
			if (Out < 450) {Out = 450;}	//限制输出值最小为100
			
			/*执行控制*/
			/*输出值给到电机PWM*/
			/*因为此函数的输入范围是-100~100，所以上面输出限幅，需要给Out值限定在-100~100*/
			PWM_SetCompare1(Out);
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);		//清除外部中断14号线的中断标志位		
	}
}

