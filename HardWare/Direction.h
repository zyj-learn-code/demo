#ifndef __DIRECTION_H
#define __DIRECTION_H

#include <stdint.h>
#include <math.h>

static int16_t Y=16;
static uint16_t AD0_V_Count=0;
extern uint8_t Screen_Mode;
static int16_t Count1=0;
static int16_t Count2=0;
extern float V_Real_Value;
extern float Set_Voltage_Value;
extern float Set_I_Value;
extern uint16_t AD0_V;
extern float V_AD_Value;

int16_t Direction_Site(void);
void OLED_Show(void);
void Button_Set_Mode0(void);
void Button_Set_Voltage(void);
void V_ChangeValue(void);
void I_ChangeValue(void);
void Button_Set_I(void);
void AD_transform_V(void);
void Adjust_V(void);
void Speed_Paramenter_Adjust(void);
void AD_transform_I(void);

#endif

