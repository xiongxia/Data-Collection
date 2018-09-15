#ifndef _FUNCTION_H_
#define _FUNCTION_H_


#include "stm32f4xx_hal.h"
#include "variable.h"
#include "rtc/bsp_calendar.h"

void Modbusprocess(uint8_t * cmd,Sensor *sensor,int type);
void Open_Delay(char port);
void Close_Delay(char port);
int BitToInt(uint8_t *bit);
uint8_t CharToBit(char c);
void Command_Data(void);
void Clear_RS485Buf();
void Hex_Copy(uint8_t * a,uint8_t * b,char num);//16进制数复制
void String_Add(char * data,char * string);
float BitToFloat(uint8_t *bit);
void Show_Data(uint8_t *bit,int len);
void Save_Data();
void Clean_Data(int type);
void UpData();
void Get_Average();
void Get_Data(int type,char * data);
void Detection();
void Control();
int Compare_Float(float a,float b);
void RTC_CalendarShow(void);
void Control_min_from_max(int i);
void Control_max_from_min(int i);
void Control_temer(int i);
void Control_sys_temer(int i);
void Chang_Start_time(int i);
void Save_Device_Data();
void Get_Device_Data(char buf)

#endif
