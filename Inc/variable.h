#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include "stm32f4xx_hal.h"

#define FALSE                                 0
#define TRUE                                  1

#define SCM_RUN                               1 
#define SCM_STOP                              0 // 系统状态

#define  LOGIC_SWITCH  0


extern __IO uint16_t warn_timer_count;
//传感器数据结构(0:PH，1:电导率，2:温度，3:压力，4:液位)
typedef struct sensor_info{
    uint8_t num; //传感器数量
    uint8_t type;//类型
    float value;
    double min,max;//控制指标信息
    double up,down;//控制告警的正常范围
    uint8_t warn;//发出告警传感器的数量
    float old_value;//之前的平均值
    int save_num;
    uint8_t control_delay;
    uint8_t warningFlag;//告警标志（为1 的时候表示 每个传感器异常都告警）
    uint8_t errorNum;//异常次数，超过三次才上传
    struct sensor_node *frist_node;//第一个传感器
}Sensor_data;   //传感器
typedef struct sensor_node{
    char devices[32];//设备ID
    char command[32];//命令
    float value;////累加值，求平均值
    char parsetype[8];
    uint8_t startadder;
    uint8_t datanum;
    uint8_t keep;
    uint8_t error;//错误标志产生错误的次数
    uint8_t amount;//计数
    uint8_t errorNum;//异常次数，超过三次才上传
    char mode[8];
    double elec_4ma;
    double elec_20ma;
    struct sensor_node *next;
}Sensor;   //传感器节点

//上传数据结构
typedef struct data{
    int type;
    char devices[32];//设备ID
    float value;//返回-10000表示错误信息
}Data;  

//控制方式：1、从最小值加到最大值           2、从最大值加到最小值           3、当前时间定时加      4、系统时间加   5:安卓控制  6：锆指标辅助控制
//继电器数据结构（5：继电器）
typedef struct Delay_node{
    char devices[32];//设备ID
    char port;//端口(1：端口1)（1-5 对应）
    int state;//状态（0：关 1：开）
    int num;//控制指标数量
    int type[6];//监控类型(0表示PH)
    int control;//控制类型(1、从最小值加到最大值   2、从最大值加到最小值   3、当前时间定时加     4、系统时间加)
    char start_save_time[12][10];//开始时间
    char sustain_save_time[12][10];//持续时间
    char start_time[12];//开始时间
    char sustain_time[12];//持续时间
    char interval_time[12];//间隔时间
    int nowControlTime;//记录当前控制时间
    int counter;//计时器
    int save_counter;//保存计时器
    int error;//控制出现严重异常，强制关闭，不运行控制逻辑，等待处理
    double volume;//对应药泵的体积
    double dosage;//加药泵每小时加药量
    uint8_t WarningFlag;
}Delay;   //继电器节点
extern Delay delay[5];  //port与下标对应


extern __IO uint8_t ManualContorlFlag;
//传感器配置
extern Data sensor_data[7];//上传数据，type与下标对应
extern Sensor_data sensor_array[7];
extern uint8_t RS485_Rx_buf[64];
extern char Android_Rx_buf[1024];

extern __IO uint16_t Android_Rx_Count;
extern __IO uint16_t RS485_Rx_Count;
extern uint16_t RS485_Rx_Count_Old;
extern uint8_t Sample_flag;//采集标志
extern uint8_t UpData_flag;//上传数据标志
extern __IO uint8_t Reboot_flag;
extern __IO uint8_t Save_flag;
extern __IO uint8_t YiYe_pump_flag;//移液泵状态,0表示关，1表示开
extern __IO uint8_t YiYe_pump_control_flag;//移液泵控制状态
extern __IO uint8_t Error_flag;
extern __IO uint8_t RTC_Config_flag;
extern uint8_t index_time_control_flag;
extern uint8_t index_time_control_flag_assist;
extern __IO uint32_t time_num;
extern uint8_t saveRS485Adder;
extern int overWeightNum;
extern float overWeightNumSave;
extern uint8_t ErrorNum;
extern uint8_t delayContorlFlag;


//传感器量程****************************************************************************************************
extern float LEVEL_Low; //液位      
extern float LEVEL_High; //液位    
extern float STREES_Low ; //压力  
extern float STREES_High; //压力
extern float WPH_Low; //PH
extern float WPH_High ; //PH
extern float WCOND_Low; //电导率
extern float WCOND_High; //电导率
extern float TEMP_Low; //温度
extern float TEMP_High; //温度
extern int LEVEL;
extern float WEIGHT_Low; //重量 kg
extern float WEIGHT_High;

extern __IO uint16_t	RevComplete;					//接收完成标志位

extern uint8_t Control_flag;//控制标志
extern uint8_t Sensor_Cfg_Mode;//串口接收传感器配置文件标志

extern uint8_t Delay_Cfg_Mode;//继电器配置文件标志

extern uint8_t time_Control_flag;//控制标志
extern uint8_t sys_time_Control_flag;//当前系统时间控制标志

extern int contral_time;//控制时间  单位秒
extern char alarm_Clock[50];

extern __IO int SCM_state;
extern char ErrorInfo[100];

extern uint8_t D0_Open[8];
extern uint8_t D0_Close[8];
extern uint8_t D1_Open[8];
extern uint8_t D1_Close[8];

extern uint8_t D2_Open[8];
extern uint8_t D2_Close[8];
extern uint8_t D3_Open[8];
extern uint8_t D3_Close[8];

extern uint8_t D4_Open[8];
extern uint8_t D4_Close[8];
extern uint8_t D5_Open[8];
extern uint8_t D5_Close[8];

extern uint8_t D6_Open[8];
extern uint8_t D6_Close[8];
extern uint8_t D7_Open[8];
extern uint8_t D7_Close[8];
extern uint8_t selectStateCommand[8];

extern __IO uint8_t FLAG;
extern uint8_t FLAG_REC;
extern __IO uint8_t clear_flag;

#endif
