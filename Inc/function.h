#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "stm32f4xx_hal.h"
#include "variable.h"
void Backups_Data();
int Modbusprocess(Sensor *sensor,int type);
void Open_Delay(char port);
void Close_Delay(char port);
void delay_ms2(int32_t nms);
int BitToInt(uint8_t *bit);
uint8_t CharToBit(char c);
void Command_Data(void);
void Clear_RS485Buf();
float BitToFloat(uint8_t *bit);
void Show_Data(uint8_t *bit,int len);
void Save_Data();
void Clean_Data(int type);
void UpData();
void Get_Average();
int Get_Data(int type,char * data);
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
void Get_Device_Data(char* buf);
void itoa (int n,char s[]);
void Reboot();
void Open_Beep();
void Close_Beep();
void Open_Light();
void Close_Light();
void Open_YiYe_pupm();
void Close_YiYe_pupm();
void Alarm();
void Open_Error(char port);
void Close_Error(char port);
void Clear_FLASH_Data();

#endif
