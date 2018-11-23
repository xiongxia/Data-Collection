#include "variable.h"

//全局标志位***************************************************************************************************
Delay delay[4];

long Invalid_Value = -10000; 


//传感器量程****************************************************************************************************
float LEVEL_Low = 0; //液位   mm
float LEVEL_High = 2000;
float STREES_Low = 0; //压力  
float STREES_High = 400; //压力(kpa)
float WPH_Low = 0; //PH
float WPH_High = 14; //PH
float WCOND_Low = 0.0; //电导率
float WCOND_High = 20000.0; //电导率(m)
float TEMP_Low = -20; //温度
float TEMP_High = 100; //温度
int LEVEL = 0;//0 感应到液体  1没有感应


//******************其他变量********************
uint16_t RevComplete = 0;  //接收完成标志位
uint8_t Sensor_Cfg_Mode = 0;//串口接收传感器配置文件标志
uint8_t Delay_Cfg_Mode = 0;//继电器配置文件标志
 
//传感器配置
Data sensor_data[5];//上传数据
Sensor_data sensor_array[5];
uint8_t RS485_Rx_buf[50];
uint8_t RS232_Rx_buf[50];//串口缓存
uint8_t Android_Rx_buf[1000];
uint8_t Rx_buf[1000];//掉电保持最新数据

uint16_t Android_Rx_Count = 0;
uint16_t RS485_Rx_Count = 0;
uint16_t RS485_Rx_Count_Old = -1;
uint16_t RS232_Rx_Count = 0;

uint8_t Sample_flag = 0;//采集标志

uint8_t UpData_flag = 0;//上传数据标志
uint8_t Control_flag = 0;//控制标志

uint8_t Debug_flag = 0;//调试标志

uint8_t time_Control_flag = 0;//当前时间控制标志
uint8_t sys_time_Control_flag = 0;//当前系统时间控制标志
char alarm_Clock[50] = "0"; 

int contral_time = 0;//控制时间  单位分

int SCM_state = SCM_RUN;//1:run 0:stop   系统状态
uint8_t Save_flag = 0;//备份状态
uint8_t YiYe_pump_flag = 0;//移液泵状态
uint8_t YiYe_pump_control_flag = 0;//移液泵控制状态
uint8_t Reboot_flag = 0;

/*

台控1012模块采集命令
控制数字量输出
D0
开：01 05 00 10 FF 00 8D FF
关：01 05 00 10 00 00 CC 0F

D1
开：01 05 00 11 FF 00 DC 3F
关：01 05 00 11 00 00 9D CF
*/

uint8_t D0_Open[8] = {0x01,0x05,0x00,0x10,0xFF,0x00,0x8D,0xFF};
uint8_t D0_Close[8] = {0x01,0x05,0x00,0x10,0x00,0x00,0xCC,0x0F};
uint8_t D1_Open[8] = {0x01,0x05,0x00,0x11,0xFF,0x00,0xDC,0x3F};
uint8_t D1_Close[8] = {0x01,0x05,0x00,0x11,0x00,0x00,0x9D,0xCF};



