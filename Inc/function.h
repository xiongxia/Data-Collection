#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "stm32f4xx_hal.h"
#include "variable.h"
int Get_Data(int type,char *data);
void Modbusprocess(Sensor *sensor,int type);
void Open_Delay(char port);
void Close_Delay(char port);
void delay_ms2(int32_t nms);
void Log();
void Command_Data(void);
void Clear_RS485Buf();
void Show_Data(uint8_t *bit,int len);
void Save_Data();
void UpData();
void Get_Average();
void Detection();
void Control();
void RTC_CalendarShow(void);
void Chang_Start_time(int i);
void Get_Device_Data(char* buf);
void Reboot();
void Open_Beep();
uint8_t CharToBit(char c);
float movedigit(const int n,const int num);
void Close_Beep();
int selectState(int port);
int BitToInt(uint8_t *bit);
void Open_Light();
void Close_Light();
void Open_YiYe_pupm();
void Close_YiYe_pupm();
void Open_Error(char port);
void Close_Error(char port);
void Clear_FLASH_Data();
void closeManualContorl();
void openManualContorl();
void itoa (int n,char s[]);
void Save_Device_Data();
float BitToFloat(uint8_t *bit);
void Clean_Data(int type);
void Alarm();
int htoi(char s[]);
void intToHex(unsigned short integer,uint8_t hex[2]);

#endif
