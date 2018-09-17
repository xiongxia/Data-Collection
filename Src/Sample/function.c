#include "function.h"
#include "variable.h"
#include "variable.h" 
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "sample.h"
#include "crc16.h"
#include "usart/bsp_debug_usart.h"
#include "RS485/bsp_usartx_RS485.h"
#include "gpio/bsp_gpio.h"
#include "rtc/bsp_rtc.h"
#include "rtc/bsp_calendar.h"
#include <conio.h>
#include "spiflash/bsp_spiflash.h"

/**
  * 函数功能: 串口接收命令
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Command_Data(){
  
  printf("开始执行命令\n");
  char *port = NULL;
  char *cmd = NULL;
  
  if(Debug_flag){
    port = strtok(RS232_Rx_buf, ",");
    Debug_flag = 0;
  }
  else{
    port = strtok(Android_Rx_buf, ",");
  }

  cmd = strtok( NULL,",");
  if(cmd[0] == '0'){
    Close_Delay(port[0]);
  }
  else if(cmd[0] == '1'){
    Open_Delay(port[0]);
  }
  else{
    printf("命令错误!!!\n");
  }
}
/**
  * 函数功能: 串口接收数据保存（传感器设备信息）
  * 输入参数: 串口数据
  * 返 回 值: 无
  * 说    明：无
  */
void Save_Data(){
  
  printf("开始保存配置信息\n");
  char temp[6][50] = {0};//暂存信息
  char *p = NULL;
  uint8_t i = 0;
  //切分数据
  if(Debug_flag){
    p = strtok(RS232_Rx_buf, ";");
    Debug_flag = 0;
  }
  else{
    p = strtok(Android_Rx_buf, ";");
  }
  while (p != NULL) {
  //  printf("ptr=%s\n", p);
    strcpy(temp[i],p);
    i++;
    p = strtok(NULL, ";");
  }
  //分析信息
  for(uint8_t j=0;j<i;j++){
    char a = temp[j][0];

    switch(a)
    {
        case '0':
            //PH
            Clean_Data(0);
            Get_Data(0,temp[j]);
           // Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
            break;
        case '1':
            //电导率
            Clean_Data(1);
            Get_Data(1,temp[j]);
           // Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
            break;
        case '2':
            //温度
            Clean_Data(2);
            Get_Data(2,temp[j]);
           // Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
            break;
        case '3':
            //压力
            Clean_Data(3);
            Get_Data(3,temp[j]);
           // Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
            break;
        case '4':
            //液位
            Clean_Data(4);
            Get_Data(4,temp[j]);
           // Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
            break;
        case '5':
            //继电器
            Clean_Data(5);
            Get_Data(5,temp[j]);
            Delay_Cfg_Mode = 1;//继电器配置文件标志
            break;
        default:
        //错误
            printf("配置信息错误!!!\n");
            break;
    }
    //只要有配置信息就设置标志位
    Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
  }
 
}
/**
  * 函数功能: 清除传感器数据
  * 输入参数: 对应类型
  * 返 回 值: 无
  * 说    明：无
  */

void Clean_Data(int type){
  if(type == 5){
    //继电器
    for(int i=0;i<5;i++){
        delay[i].devices[0] = '\0';
        delay[i].port = 0;
        delay[i].state = 0;
        delay[i].type[0] = -1;
        delay[i].num = 0;
        delay[i].control = 0;
        delay[i].counter = 0;
        delay[i].interval_time[0] = '\0';
        delay[i].save_counter = 0;
        delay[i].start_time[0] = '\0';
        delay[i].sustain_time[0] = '\0' ;
    }
    printf("继电器信息清除完毕\n");
    return ;
  }
    Sensor *p,*q;
    p = sensor_array[type].frist_node;
    q = p;
    for(int j=1;j<sensor_array[type].num;j++){
        q = p->next;
        free(p);
        p = q;
    }
    sensor_array[type].max = 0;
    sensor_array[type].min = 0;
    sensor_array[type].num = 0;
    sensor_array[type].frist_node = NULL;
    printf("传感器信息清除完毕\n");
}
/**
  * 函数功能: 上传传感器数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void UpData(){
  
  printf("开始上传数据\n");
  char devicesID[20];
  float value;
  int type,i,j;
  char data[100];
  Sensor *p = NULL;
  //打包数据 
  for(i=0;i<5;i++){
    //上传存在的传感器
   if(sensor_array[i].num >0){
    p = sensor_array[i].frist_node;
    type = sensor_data[i].type;
    value = sensor_data[i].value;
    sprintf(data,"%.2f,%d\n",value,type);
    printf("上传数据：%.2f,%d\n",value,type);
    HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
    for(j=0;j<sensor_array[i].num;j++){
        value = p->value;
        strcpy(devicesID,p->devices);
        sprintf(data,"%.2f,%d,%s\n",value,type,devicesID);
        printf("上传数据：%.2f,%d,%s\n",value,type,devicesID);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
         //清除数据
        p->error = 0;
        p->amount = 0;
        p->value = 0;
        p = p->next;
    }
   } 
  }
  printf("上传数据完成\n");
}
/**
  * 函数功能: 计算传感器数据（平均值），判断异常,保证每次上传显示数据稳定
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Get_Average(){
  
  printf("开始计算数据均值\n");
  Sensor *p = NULL,*q = NULL;//q记录error次数最大的传感器
  float temp_value = 0;//暂存数据
  int e = 0,error = 0;//e记录最大错误次数,error为累加错误次数
  
  for(int i=0;i<5;i++){
    p = sensor_array[i].frist_node;
    if(!p){
        continue;
    }
    while(p){
      //考虑全错的情况,全错的设备不进行平均值计算或者错误次数过多
        if(p->amount == 0 || p->error >30){
            p->value = 0;
        }
        else{
            p->value = (float)p->value / p->amount;
        }
        temp_value = temp_value + p->value;
        if(p->error > e){
          e = p->error;
          q = p;
        }
        error = error + p->error;
        p = p->next;
    }//while
    sensor_data[i].type = i;
    //保存对的数据
    if(error > 30)
    {
      //采集数据有问题
        sensor_data[i].value = -10000;
        strcpy(sensor_data[i].devices,q->devices);
    }
    else
    {
        temp_value = temp_value / sensor_array[i].num;
        sensor_array[i].value = temp_value;        
        //打包数据
        sensor_data[i].value = temp_value;      
    }
  }//for
 
}
/**
  * 函数功能: 提取关键数据
  * 输入参数: 传感器类型,传感器信息字符串
  * 返 回 值: 无
  * 说    明：无
  */
void Get_Data(int type,char * data){
  
    char *result = NULL;
    Sensor *p = NULL;
    char temp_data[20][50];
    int i = 0;//计数
    double min,max;
    
    result = strtok(data,",");
    //result = strtok( NULL,",");
    //提取数据
    while( result != NULL ) {
       result = strtok( NULL,",");
       strcpy(temp_data[i],result);
       i++;
    }
  
    if(type == 5){
        //继电器
        int n = atoi(temp_data[0]);
        int m;
        for(i=0;i<n;i++){
            strcpy(delay[i].devices,temp_data[4*i+1]);
            delay[i].port = temp_data[4*i+2][0];
            delay[i].control = atoi(temp_data[4*i+3]);
            if(delay[i].control == 4){   
                    temp_data[4*i+4][5] = '\0';
                    strcpy(delay[i].start_time,temp_data[4*i+4]); 
                    strcpy(delay[i].sustain_time,&temp_data[4*i+4][6]); 
            }
            else if(delay[i].control == 3){
                temp_data[4*i+4][5] = '\0';
                strcpy(delay[i].interval_time,temp_data[4*i+4]); 
                strcpy(delay[i].sustain_time,&temp_data[4*i+4][6]); 
            
            }
            else{
                m = strlen(temp_data[4*i+4]);
                delay[i].num = m;
                for(int j=0;j<m;j++){
                    delay[j].type[j] = temp_data[4*i+4][j] - 48;
                }
            }
          
            delay[i].state = 0;
       }
       return ;
    }
    
    sensor_array[type].type = type;
    sensor_array[type].num = atoi(temp_data[0]);
    min = strtod(temp_data[1],NULL);
    max = strtod(temp_data[2],NULL);
    sensor_array[type].min = min;
    sensor_array[type].max = max;
    sensor_array[type].frist_node = NULL;
    //建立传感器链表
    for(i=0;i<sensor_array[type].num;i++){
       // printf("%s %c %c\n",temp_data[3*i+3],temp_data[3*i+4][0],temp_data[3*i+5][0]);
        p = (Sensor *)malloc(sizeof(Sensor));
        strcpy(p->devices,temp_data[7*i+3]);
        strcpy(p->command,temp_data[7*i+4]);
        strcpy(p->parsetype,temp_data[7*i+5]);
        p->startadder =  atoi(temp_data[7*i+6]);
        p->datanum = atoi(temp_data[7*i+7]);
        p->keep = atoi(temp_data[7*i+8]);
        strcpy(p->mode,temp_data[7*i+9]);
        p->error = 0;
        p->value = 0;
        p->amount = 0;
        p->next = NULL;
        //插入链表
        if(sensor_array[type].frist_node == NULL){
            sensor_array[type].frist_node = p;
           // p->next = NULL;
        }
        else{
            p->next = sensor_array[type].frist_node;
            sensor_array[type].frist_node = p;
        }
     }//for
   // free(temp_data);//释放数据
 }
    
/**
  * 函数功能: 检测传感器异常
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Detection(){
  Sensor * p;
  int i;
  char data[20];
  
  for(i=0;i<5;i++){
    p = sensor_array[i].frist_node;
    while(p){
      if(p->error > 30){
        sprintf(data,"!%s",p->devices);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      }
      p = p->next;
    }
  
  }
  

}
/**
  * 函数功能: 拉格朗日插值法
  * 输入参数: x数组，y数组，求值x
  * 返 回 值: y值
  * 说    明：无
  */
float lagrange(float *x,float *y,float xx,int n)     /*拉格朗日插值算法*/

{

    int i,j;

    float *a,yy=0.0;    /*a作为临时变量，记录拉格朗日插值多项式*/

    a=(float *)malloc(n*sizeof(float));

    for(i=0; i<=n-1; i++)

    {

        a[i]=y[i];

        for(j=0; j<=n-1; j++)

            if(j!=i) a[i]*=(
			-x[j])/(x[i]-x[j]);

        yy+=a[i];

    }

    free(a);

    return yy;

}

 /**
  * 函数功能: 打开继电器
  * 输入参数: 端口号
  * 返 回 值: 无
  * 说    明：无
  */
void Open_Delay(char port){  
  
  printf("打开继电器:%c\n",port);
  RTC_CalendarShow();
  switch(port){
    case '1':
      OUTPUT1_GPIO_ON;
      delay[0].state = 1;
      sprintf(data,"%s,1,%d",delay[0].devices,delay[0].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '2':
      OUTPUT2_GPIO_ON;
      delay[1].state = 1;
      sprintf(data,"%s,2,%d",delay[1].devices,delay[1].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '3':
      OUTPUT3_GPIO_ON;
      delay[2].state = 1;
      sprintf(data,"%s,3,%d",delay[2].devices,delay[2].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '4':
      OUTPUT4_GPIO_ON;
      delay[3].state = 1;
      sprintf(data,"%s,4,%d",delay[3].devices,delay[3].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    default:
      break;
  }
}
/**
  * 函数功能: 修改开启时间
  * 输入参数: int i对应传感器 
  * 返 回 值: 无
  * 说    明：无
  */
void Chang_Start_time(int i){
  
    char *result = NULL;
    char start_temp_data[2][20];
    char interval_temp_data[2][20];
    int n = 0;
    int hours,minute;
    char temp[20];
    RTC_TimeTypeDef stimestructureget;
    /* 获取当前时间 */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  
    /* 显示时间 */
    sprintf(temp,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);
    strcpy(delay[i].start_time,temp);
    
    strcpy(temp,delay[i].interval_time);
    result = strtok(temp,":");
       //提取数据
    while( result != NULL ) {
       strcpy(interval_temp_data[n],result);
       result = strtok(NULL,":");
       n++;
    }
    n = 0;
    strcpy(temp,delay[i].start_time);
    result = strtok(temp,":");
       //提取数据
    while( result != NULL ) {
       strcpy(start_temp_data[n],result);
       result = strtok(NULL,":");
       n++;
    }
    
    minute =  atoi(start_temp_data[1]) + atoi(interval_temp_data[1]);
    if(minute >= 60){
        hours = minute / 60;
        minute = minute % 60;
    }
    hours += (atoi(start_temp_data[0]) + atoi(interval_temp_data[0])); 
    hours = hours % 24;
    sprintf(delay[i].start_time,"%02d:%02d",hours,minute);

}
/**
  * 函数功能: 关闭继电器
  * 输入参数: 端口号
  * 返 回 值: 无
  * 说    明：无
  */
void Close_Delay(char port){
  char data[50];
  
  printf("关闭继电器:%c\n",port);
  RTC_CalendarShow();
    switch(port){
    case '1':
      OUTPUT1_GPIO_OFF; 
      delay[0].state = 0;
      sprintf(data,"%s,1,%d",delay[0].devices,delay[0].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '2':
      OUTPUT2_GPIO_OFF; 
      delay[1].state = 0;
      sprintf(data,"%s,2,%d",delay[1].devices,delay[1].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '3':
      OUTPUT3_GPIO_OFF;
      delay[2].state = 0;
      sprintf(data,"%s,3,%d",delay[2].devices,delay[2].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '4':
      OUTPUT4_GPIO_OFF; 
      delay[4].state = 0;
      sprintf(data,"%s,4,%d",delay[3].devices,delay[3].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    default:
      break;
  }
                                    
}

/**
  * 函数功能: 显示当前时间和日期
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void RTC_CalendarShow(void)
{
  uint8_t str[30]; // 字符串暂存  

  
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  /* 获取当前时间 */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* 获取当前日期 */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

  /* 显示日期*/
  printf("当前时间为: %02d年(%s年)%02d月%02d日(星期%s)  ", 2000+sdatestructureget.Year,zodiac_sign[(2000+sdatestructureget.Year-3)%12],
  sdatestructureget.Month, sdatestructureget.Date,WEEK_STR[sdatestructureget.WeekDay]);
  /* 显示时间 */
  printf("%02d:%02d:%02d\n", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  
}

/**
  * 函数功能: 控制继电器方式一：根据指标从小到大
  * 输入参数: 对应端口的继电器
  * 返 回 值: 无
  * 说    明：无
  */
void Control_min_from_max(int i){
  int j,n,type;
 
  n = delay[i].num;
  //查询指标
  for(j=0;j<n;j++){
    type = delay[i].type[j];
    if(sensor_array[type].value < sensor_array[type].min)
     //小于最小值
     {
        //控制对应端口继电器
        Open_Delay(delay[i].port);
     }
    if(sensor_array[type].value > sensor_array[type].max)
          //大于最大值
    {
            //控制对应端口继电器
        Close_Delay(delay[i].port);
    }
          
  }//for 
    
 
}
/**
  * 函数功能: 控制继电器方式二：根据指标从大到小
  * 输入参数: 对应端口的继电器
  * 返 回 值: 无
  * 说    明：无
  */
void Control_max_from_min(int i){
  int j,n,type;
 
  n = delay[i].num;
  //查询指标
  for(j=0;j<n;j++){
    type = delay[i].type[j];
    if(sensor_array[type].value < sensor_array[type].min)
     //小于最小值
     {
         //控制对应端口继电器
        Close_Delay(delay[i].port);
     }
    if(sensor_array[type].value > sensor_array[type].max)
          //大于最大值
    {
           
        //控制对应端口继电器
        Open_Delay(delay[i].port);
    }
          
  }//for 



}
/**
  * 函数功能: 控制继电器方式三：当前定时加
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Control_temer(int i){
    
    char start_time[50],sustain_time[50];
    char temp_data[5][10];
    int n = 0;//计数
    char *result = NULL;
    //char hours[5],minutes[5],seconds[5];
   
    RTC_TimeTypeDef stimestructureget;
    /* 获取当前时间 */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  
    /* 显示时间 */
    sprintf(start_time,"%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes);
     
    strcpy(delay[i].start_time,start_time);
    strcpy(sustain_time,delay[i].sustain_time);
   
    result = strtok(sustain_time,":");
       //提取数据
    while( result != NULL ) {
       strcpy(temp_data[n],result);
      // printf("时间：%s\n",result);
       result = strtok(NULL,":");
       n++;
    }
     
    if(n == 0){
      printf("输入异常!\n");
      return;
    }
    delay[i].counter = ((atoi(temp_data[0])) * 60 + atoi(temp_data[1]))*60000;
    delay[i].save_counter = delay[i].counter;

    time_Control_flag = 1;

}
/**
  * 函数功能: 控制继电器方式四：系统时间定时
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Control_sys_temer(int i){
    char start_time[50],sustain_time[50];
    char temp_data[5][10];
    int n = 0;//计数
    char *result = NULL;
        
    strcpy(start_time,delay[i].start_time);
    strcpy(sustain_time,delay[i].sustain_time);
   
    result = strtok(sustain_time,":");
       //提取数据
    while( result != NULL ) {
       strcpy(temp_data[n],result);
      // printf("时间：%s\n",result);
       result = strtok(NULL,":");
       n++;
    }
     
    if(n == 0){
      printf("输入异常!\n");
      return;
    }
    
    delay[i].counter = ((atoi(temp_data[0])) * 60 + atoi(temp_data[1]))*60000;
    delay[i].save_counter = delay[i].counter;
      
    sys_time_Control_flag = 1;

}
/**
  * 函数功能: 控制继电器
  * 输入参数: 对应端口的继电器
  * 返 回 值: 无
  * 说    明：检测配置好了的继电器，判断devices是否为0，0表示没有配置信息
  */
void Control(){
  int i,j,n,type;
  for(i=0;i<5;i++){
    if(delay[i].devices[0] != '0'){
      //有配置
        n = delay[i].control;
        switch(n){
            case 1:
              Control_min_from_max(i);
              break;
            case 2:
              Control_max_from_min(i);
              break;
            case 3:
              if(delay[i].counter == 0){
                Control_temer(i);
              }
              break;
            case 4:
              if(delay[i].counter == 0){
                Control_sys_temer(i);
              }
              
              break;
            default:
              break;
        }
   
    }//if
  
  }//for
}

/**
  * 函数功能: 字符转十六进制
  * 输入参数: 字符
  * 返 回 值: 十六制
  * 说    明：无
  */
uint8_t CharToBit(char c){

  uint8_t b = c -  (uint8_t)48;
 // printf("%x",b);
  return b;

}
void Clear_RS485Buf(){
  for(int i=0;i<RS485_Rx_Count_Old;i++)
    RS485_Rx_buf[i] = 0x00;
}


/**
  * 函数功能: int移位变成浮点 
  * 输入参数: n 待移位的整数，num 移动的位数 
  * 返 回 值: 转换的浮点型 
  * 说    明：无
  */
float movedigit(const int n,const int num)
{
	float f = (float)n;
    printf("......%.2f\n",f);
    switch(num)	
	{
		case 0:
			//f = f * 0.01;
			break;
		case 1:
			f = f * 0.1;
			break;
		case 2:
			f = f * 0.01;
			break;
		case 3:
			f = f * 0.001;
			break;
		case 4:
			f = f * 0.0001;
			break;
		case 5:
			f = f * 0.00001;
			break;
		case 6:
			f = f * 0.000001;
			break;
		case 7:
			f = f * 0.0000001;
			break;
		case 8:
			f = f * 0.00000001;
			break;
		case 9:
			f = f * 0.000000001;
			break;
		default:
			printf("num erro!\n");
			break;

	}
   
	return f;

}
/**
  * 函数功能: 485接收数据处理
  * 输入参数: cmd数据，Sensor 传感器结构
  * 返 回 值: 无
  * 说    明：接收完成对接收的数据进行CRC验证，验证失败error次数累加
  */

void Modbusprocess(uint8_t * data,Sensor *sensor,int type)
{
  
   uint8_t temp[20]={0x00,0x00,0x00,0x00};

   float value;
   float vaule_float,vaule;
   int vaule_int;
   char buf[10];
   
   if(CRC16_MODBUS(RS485_Rx_buf,RS485_Rx_Count_Old) == 0){
        printf("传感器接收数据正确\n");
        //验证通过
   }
   else{
        printf("传感器接收数据错误\n");
        sensor->error++;
        Clear_RS485Buf();
        return;
   }
   
   if(!strcmp(sensor->parsetype,"Float")){
        if(!strcmp(sensor->mode,"big")){
            if(sensor->datanum == 4){
                temp[2] = data[sensor->startadder-1];
                temp[3] = data[sensor->startadder];
                temp[0] = data[sensor->startadder + 1];
                temp[1] = data[sensor->startadder + 2];
            }
			else{
                temp[2] = data[sensor->startadder];
                temp[3] = data[sensor->startadder-1];
                temp[0] = 0x00;
                temp[1] = 0x00;
            }

        }
		else{
            if(sensor->datanum == 4){
                temp[0] = data[sensor->startadder-1];
                temp[1] = data[sensor->startadder];
                temp[2] = data[sensor->startadder + 1];
                temp[3] = data[sensor->startadder + 2];
            }
            else{
                temp[2] = data[sensor->startadder-1];
                temp[3] = data[sensor->startadder];
                temp[0] = 0x00;
                temp[1] = 0x00;
			}
		}
		vaule_float = BitToFloat(temp);	
        sensor->value += vaule_float;
        //sprintf(buf, "%.2f", vaule_float);

		//save_data
         printf("\nvaule-float:%.2f\n",vaule_float);
   }
   else if(!strcmp(sensor->parsetype,"Integer")){
        if(sensor->datanum == 2){
            temp[0] = 0x00;
            temp[1] = 0x00;
            temp[2] = data[sensor->startadder - 1];
            temp[3] = data[sensor->startadder];
		}
		else{
            temp[0] = data[sensor->startadder-1];
            temp[1] = data[sensor->startadder];
            temp[2] = data[sensor->startadder + 1];
            temp[3] = data[sensor->startadder + 2];
		}
		vaule_int = BitToInt(temp);
		vaule = movedigit(vaule_int,sensor->keep);
					
		printf("\nvaule-int:%d\n",vaule_int);
		printf("\nvaule-int:%.2f\n",vaule);
		//sprintf(buf, "%.2f", vaule);
					
	}
	else{
        printf("\nNOt compatibility\n");
        //continue;
        return;
	}
   switch(type){
        case 0:
            //PH
              //判断是否在正常范围
              if(WPH_Low <= value && WPH_High >= value)
              {
                sensor->value = sensor->value + value;
                sensor->amount++;
              }
              else
              {
                sensor->error++;
              }           
            break;
        case 1:
            //电导率
          //判断是否在正常范围
              if(WCOND_Low <= value && WCOND_High >= value)
              {
                sensor->value = sensor->value + value;
                sensor->amount++;
              }
              else
              {
                sensor->error++;
              }
            break;
        case 2:
            //温度   
              //判断是否在正常范围
              if(TEMP_Low <= value && TEMP_High >= value)
              {
                sensor->value = sensor->value + value;
                sensor->amount++;
              }
              else
              {
                sensor->error++;
              }
            break;
        
        case 3:
            
              //判断是否在正常范围
              if(STREES_Low <= value && STREES_High >= value)
              {
                sensor->value = sensor->value + value;
                sensor->amount++;
              }
              else
              {
                sensor->error++;
              }
          
            break;
        case 4:
            //液位
                    
              //判断是否在正常范围
              if(LEVEL_Low <= value && LEVEL_High >= value)
              {
                sensor->value = sensor->value + value;
                sensor->amount++;
              }
              else
              {
                sensor->error++;
              }
          
            break;
        default:
        //错误
            break;
    }//switch
   Clear_RS485Buf();

}

//16进制数复制
void Hex_Copy(uint8_t * a,uint8_t * b,char num)
{
	uint8_t i=0;
	for(i=0;i<num;i++)
	{
		a[i]=b[i];
	
	}

}

//给字符串加前缀
//data：要加的前缀
//string：要加前缀的字符串
void String_Add(char * data,char * string)
{
	uint8_t data_len=0;   //前缀长度
	uint8_t string_len=0;   //字符串长度
	short i=0;
	data_len=strlen(data);			//获取前缀长度
	string_len=strlen(string);		//获取字符串长度
	for(i=string_len-1;i>=0;i--)
	{
		string[i+data_len]=string[i];
	
	}
	for(i=0;i<data_len;i++)
	{
		string[i]=data[i];
	
	}
}
/**
  * 函数功能: 十六进制字节组转换为int
  * 输入参数: 字节组
  * 返 回 值: int
  * 说    明：无
  */
int BitToInt(uint8_t *bit)
{
    int a = bit[0]<<24;
  	int b = bit[1]<<16;
  	int c = bit[2]<<8;
  	a = a + b + c + bit[3]; 
  	
  return a;
  
}

/**
  * 函数功能: 十六进制字节组转换为float
  * 输入参数: 字节组
  * 返 回 值: float
  * 说    明：无
  */
float BitToFloat(uint8_t *bit)
{
    uint8_t m0 = bit[0];
    uint8_t m1 = bit[1];
    uint8_t m2 = bit[2];
    uint8_t m3 = bit[3];
    
    // 求符号位
    float sig = 1.;
    if (m0 >=128.)
        sig = -1.;
   // printf("sig %f\n",sig);

    //求阶码
    float jie = 0.;
        if (m0 >=128.)
        {
            jie = m0-128.;
        }
        else
        {
            jie = m0;
        }
    jie = jie * 2.;
    if (m1 >=128.)
        jie += 1.;
    //printf("jie %f\n",jie);
    jie -= 127.;
    //printf("jie %f\n",jie);
    //求尾码
    float tail = 0.;
    if (m1 >=128.)
        m1 -= 128.;
    tail =  m3 + (m2 + m1 * 256.) * 256.;
    //printf("tail %f\n",tail);
    tail  = (tail)/8388608;   //   8388608 = 2^23
    //printf("tail %f\n",tail);
    float f = sig * pow(2., jie) * (1+tail);
    return f;
}
/**
  * 函数功能: 显示字节数据
  * 输入参数: 字节组
  * 返 回 值: 无
  * 说    明：无
  */
void Show_Data(uint8_t *bit,int len){
 
  for(int i=0;i<len;i++){
    printf("%02x ",bit[i]);
    
  }
    printf("\n");
}

/**
  * 函数功能: 保存配置信息
  * 输入参数: 字节组
  * 返 回 值: 无
  * 说    明：无
  */
void Save_Device_Data(){
  
  SPI_FLASH_BufferWrite(Flash_Buffer, FLASH_WriteAddress, 1000);
  printf("保存配置：%s\n"，Flash_Buffer);
 
}
/**
  * 函数功能: 获取配置信息
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Get_Device_Data(char* buf){
  
  SPI_FLASH_BufferRead(buf, FLASH_WriteAddress, 1000);
  printf("获取配置：%s\n"，buf);
 
}


