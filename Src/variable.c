#include "variable.h"
//const unsigned int Address_Offset = 0x08035000;//地址偏移


//全局标志位***************************************************************************************************
Delay delay[4];

long Invalid_Value = -10000; 

//传感器命令
//博海致远温度传感器
uint8_t BHZY_Temp[10] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02,0xC4,0x38};

//博海致远电导率
uint8_t BHZY_Conductivity[10] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x03, 0x05,0x9E}; 
//博海致远PH传感器
uint8_t BHZY_PH[10] = {0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};
uint8_t BHZY_PH_4[10] = {0x02, 0x03, 0x00, 0x04, 0x00, 0x01,0xC5,0xF8};
uint8_t BHZY_PH_9_18[10] = {0x02, 0x03, 0x00, 0x06, 0x00, 0x01,0x64,0x38};
uint8_t BHZY_PH_6_86[10] = {0x02, 0x03, 0x00, 0x05, 0x00, 0x01,0x94,0x38};
//GreenPrima电导率仪表
uint8_t GP_Conductivity[10] ={0x01,0x04,0x00,0x00,0x00,0x02,0xD5,0xCA};
//朴厚PH
uint8_t PH_PH[10] = {0x0A,0x03,0x00,0x20,0x00,0x0A,0x00,0x00};
uint8_t PH_PH_4[20]={0x01,0x10,0x00,0x20,0x00,0x02,0x04,0x00,0x00,0x40,0x80,0xC1,0xD7};
uint8_t PH_PH_9_18[20]={0x01,0x10,0x00,0x20,0x00,0x02,0x04,0xE1,0x48,0x41,0x12,0xF7,0xC0};

//星仪压力传感器
uint8_t XY_Strees[10] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};
//液位
uint8_t Level[10] = {0x01,0x00,0x03,0x01,0x00,0x02,0x95,0xCB};


//传感器类型********************************************************************************************************
//unsigned int Sensor_Number;  //传感器数量
uint8_t STREES_Type = 4; //压力           
uint8_t LEVEL_Type = 3; //液位
uint8_t WPH_Type = 0; //PH
uint8_t WCOND_Type = 1; //电导率
uint8_t TEMP_Type = 2; //温度




//传感器量程****************************************************************************************************
float LEVEL_Low = 0; //液位   cm
float LEVEL_High = 100;
float STREES_Low = 0; //压力  
float STREES_High = 200; //压力(kpa)
float WPH_Low = 1; //PH
float WPH_High = 14; //PH
float WCOND_Low = 0.0; //电导率
float WCOND_High = 20000.0; //电导率(m)
float TEMP_Low = -20; //温度
float TEMP_High = 100; //温度
int LEVEL = 0;//0 感应到液体  1没有感应

/*******************************************************
const unsigned int Sensor_Enable_Address = Address_Offset+0;
const unsigned int Data_Send_Interval_Address =Address_Offset+0x100;
const unsigned int Power_Batt_Address =Address_Offset+0x104; //供电方式地址
const unsigned int LED_OPEN_CLOSE_TIME_Address=Address_Offset+0x108;    //LED打开时间范围地址
const unsigned int LED_STATE_Address=Address_Offset+0x120;    //LED状态地址

const unsigned int System_Init_Address =Address_Offset+0x124;
const unsigned int MODE_Address =Address_Offset+0x128;
const unsigned int GPRSMODE_Address =Address_Offset+0x12C;



const unsigned int Sample_Hz_Address = Address_Offset+0x130; //采样频率地址
const unsigned int Sample_Noise_Time_Address = Address_Offset+0x134; //噪声采样间隔
const unsigned int GPRS_Port_Address = Address_Offset+0x138; //GPRS端口
const unsigned int GPRS_IP_Address = Address_Offset+0x13C;
const unsigned int SERVER_Address = Address_Offset+0x140;
const unsigned int SN_Address = Address_Offset+0x240;
const unsigned int SERVER_LEN_Address = Address_Offset+0x340;
const unsigned int SN_LEN_Address = Address_Offset+0x344;

const unsigned int Correcting_Flag_Address =Address_Offset+0x348; //参数修正开关地址
const unsigned int Correct_K_WLVL_Address = Address_Offset+0x34C;
unsigned int SENSOR_NUM_Address=Address_Offset+0x350;
unsigned int SENSOR_Address = Address_Offset+0x354;
unsigned int HEART_Address = Address_Offset+0x1010;
unsigned int HEART_LEN_Address = Address_Offset+0x1110;

//传感器信息地址
unsigned int Sensor_info_Address = Address_Offset+0x2000;
//传感器数据地址
unsigned int Sensor_data_Address = Address_Offset+0x3000;
/*******************************************************/


//******************其他变量********************
uint16_t	RevComplete = 0;					//接收完成标志位


uint8_t Sensor_Cfg_Mode = 0;//串口接收传感器配置文件标志
uint8_t Delay_Cfg_Mode = 0;//继电器配置文件标志
 

//传感器配置
Data sensor_data[5];//上传数据
Sensor_data sensor_array[5];
uint8_t RS485_Rx_buf[500];
uint8_t RS232_Rx_buf[500];//串口缓存
uint8_t Android_Rx_buf[1000];

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

int dosage_sum = 0;//总加药量
int dosing_frequency = 9;//每小时加药量 每小时9L
int dosage = 1;//默认1L

uint8_t dir = 0; // 0 ：顺时针   1：逆时针 
uint8_t ena = 1; // 0 ：正常运行 1：停机

float Speed_Motor = 92.16;//电机速度


//液位（12个）

