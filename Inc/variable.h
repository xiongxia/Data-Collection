#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "stm32f4xx_hal.h"


//extern uint8_t dir; // 0 ：顺时针   1：逆时针 
//extern uint8_t ena; // 0 ：正常运行 1：停机


#define FALSE                                 0
#define TRUE                                  1
#define CW                                    0 // 顺时针
#define CCW                                   1 // 逆时针

#define SCM_RUN                                   1 //
#define SCM_STOP                                  0 // 系统状态

#define STOP                                  0 // 加减速曲线状态：停止
#define ACCEL                                 1 // 加减速曲线状态：加速阶段
#define DECEL                                 2 // 加减速曲线状态：减速阶段
#define RUN                                   3 // 加减速曲线状态：匀速阶段
#define T1_FREQ                               (SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // 频率ft值
#define FSPR                                  200         //步进电机单圈步数
#define MICRO_STEP                            32          // 步进电机驱动器细分数
#define SPR                                   (FSPR*MICRO_STEP)   // 旋转一圈需要的脉冲数

// 数学常数
#define ALPHA                                 ((float)(2*3.14159/SPR))       // α= 2*pi/spr
#define A_T_x10                               ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148                           ((float)((T1_FREQ*0.676)/10)) // 0.676为误差修正值
#define A_SQ                                  ((float)(2*100000*ALPHA)) 
#define A_x200                                ((float)(200*ALPHA))

typedef struct {
  __IO uint8_t  run_state ;  // 电机旋转状态
  __IO uint8_t  dir ;        // 电机旋转方向
  __IO int32_t  step_delay;  // 下个脉冲周期（时间间隔），启动时为加速度
  __IO uint32_t decel_start; // 启动减速位置
  __IO int32_t  decel_val;   // 减速阶段步数
  __IO int32_t  min_delay;   // 最小脉冲周期(最大速度，即匀速段速度)
  __IO int32_t  accel_count; // 加减速阶段计数值
  __IO int32_t  speed; // 匀速速度（rpm）
  __IO int32_t  step; //步数
  __IO int32_t  min_speed; //最小速度  （根据电机不同有所不同）
  __IO int32_t  max_speed; //最大速度
  __IO uint16_t Toggle_Pulse;//脉冲频率
}speedRampData;


//传感器数据结构(0:PH，1:电导率，2:温度，3:液位，4:压力)
typedef struct sensor_info{
    int num; //传感器数量
    int type;//类型
    float value;
    double min,max;//控制信息
    struct sensor_node *frist_node;//第一个传感器
}Sensor_data;   //传感器
typedef struct sensor_node{
    char devices[30];//设备ID
    char command[50];//命令
    float value;////累加值，求平均值
    int error;//错误标志产生错误的次数
    int amount;//计数
    char parsetype[10];
    int startadder;
    int datanum;
    int keep;
    char mode[10];
    struct sensor_node *next;
}Sensor;   //传感器节点

//上传数据结构
typedef struct data{
    int type;
    char devices[30];//设备ID
    float value;//返回-10000表示错误信息
}Data;  

//控制方式：1、从最小值加到最大值           2、从最大值加到最小值           3、当前时间定时加      4、系统时间加
//继电器数据结构（5：继电器）
typedef struct Delay_node{
    char devices[30];//设备ID
    char port;//端口(1：端口1)
    int state;//状态（0：关 1：开）
    int num;//控制指标数量
    int type[5];//监控类型(0表示PH)
    int control;//控制类型
    char start_time[10];//开始时间
    char sustain_time[10];//持续时间
    char interval_time[10];//间隔时间
    int counter;//计时器
    int save_counter;//保存计时器
}Delay;   //继电器节点
extern Delay delay[4];  //port与下标对应

//传感器配置
extern Data sensor_data[5];//上传数据，type与下标对应
extern Sensor_data sensor_array[5];
extern uint8_t RS485_Rx_buf[500];
extern uint8_t RS232_Rx_buf[500];//串口缓存
extern uint8_t Android_Rx_buf[1000];
extern uint8_t Rx_buf[1000];

extern uint16_t Android_Rx_Count;
extern uint16_t RS485_Rx_Count;
extern uint16_t RS485_Rx_Count_Old;
extern uint16_t RS232_Rx_Count;
extern uint8_t Sample_flag;//采集标志
extern uint8_t UpData_flag;//上传数据标志
extern uint8_t Debug_flag;//调试标志

//传感器命令
//温度
extern uint8_t BHZY_Temp[10];
//博海致远电导率
extern uint8_t BHZY_Conductivity[10]; 
//博海致远PH传感器
extern uint8_t BHZY_PH[10];
extern uint8_t BHZY_PH_4[10];
extern uint8_t BHZY_PH_9_18[10];
extern uint8_t BHZY_PH_6_86[10];
//GreenPrima电导率仪表
extern uint8_t GP_Conductivity[10];
//朴厚PH
extern uint8_t PH_PH[10];
extern uint8_t PH_PH_4[20];
extern uint8_t PH_PH_9_18[20];

//星仪压力传感器
extern uint8_t XY_Strees[10];
//液位
extern uint8_t Level[10];

extern speedRampData Stepper_Motor[4];//步进电机


//传感器类型********************************************************************************************************

extern uint8_t STREES_Type ; //压力           
extern uint8_t LEVEL_Type ; //液位
extern uint8_t WPH_Type  ; //PH
extern uint8_t WCOND_Type ; //电导率
extern uint8_t TEMP_Type   ; //温度


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



//******************其他变量********************
extern uint16_t	RevComplete;					//接收完成标志位

extern uint8_t Control_flag;//控制标志
extern uint8_t Sensor_Cfg_Mode;//串口接收传感器配置文件标志

extern uint8_t Delay_Cfg_Mode;//继电器配置文件标志

extern uint8_t time_Control_flag;//控制标志
extern uint8_t sys_time_Control_flag;//当前系统时间控制标志

extern int contral_time;//控制时间  单位秒
extern char alarm_Clock[50];


extern int dosage_sum;//总加药量
extern int dosing_frequency;//每小时加药量 每小时9L
extern int dosage;//默认1L


extern int SCM_state;


#endif
