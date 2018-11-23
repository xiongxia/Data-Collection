#include "function.h"
#include "variable.h"
#include "string.h"
# include "stdio.h"
# include "stdlib.h"
#include "math.h"
#include "sample.h"
#include "crc16.h"
#include "usart/bsp_debug_usart.h"
#include "RS485/bsp_usartx_RS485.h"
#include "gpio/bsp_gpio.h"
#include "rtc/bsp_rtc.h"
#include "spiflash/bsp_spiflash.h"

/**
  * 函数功能: 回退传感器信息
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Backups_Data(){
  
  printf("Backups_Data：回退配置信息\n");
  strcpy(RS232_Rx_buf,Rx_buf);
  strcpy(Android_Rx_buf,Rx_buf);
  Save_Data();
}


/**
  * 函数功能: 串口接收命令
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Command_Data(){
  
  printf("Command_Data：开始执行命令\n");
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
    printf("Command_Data：命令错误!!!\n");
  }
}
/**
  * 函数功能: 串口接收数据保存（传感器设备信息）
  * 输入参数: 串口数据
  * 返 回 值: 无
  * 说    明：无
  */
void Save_Data(){
  
  printf("Save_Data：开始保存配置信息\n");
  char temp[500] = {0};//暂存信息
  char *p = NULL;
  int error=0,len = 0,data_len = 0;
  uint8_t buf[1000] = {0},n = 0;
  char c;
  int flag = 1;

  //切分数据
  if(Debug_flag){
    strcpy(buf,RS232_Rx_buf);
    //p = strtok(RS232_Rx_buf, ";");
    Debug_flag = 0;
  }
  else{
    strcpy(buf,Android_Rx_buf);
    //p = strtok(Android_Rx_buf, ";");
  }
  
  data_len = strlen(buf);
  Clean_Data(0);
  Clean_Data(1);
  Clean_Data(2);
  Clean_Data(3);
  Clean_Data(4);
  Clean_Data(5);  
  p = strtok(buf, ";");
  while (p != NULL) {
    n++;
    //printf("ptr=%s\n",p);
    strcpy(temp,p);
    c = temp[0]; 
    len += strlen(p);
    //printf("%d\n",len+n);
    len = len + 1;
    p = &buf[len];
    p = strtok(p, ";");
    if(len >= data_len)
      p = NULL;

    switch(c)
    {
        case '0':
            //PH
            error = Get_Data(0,temp);
            break;
        case '1':
            //电导率
            error = Get_Data(1,temp);
            break;
        case '2':
            //温度
            error = Get_Data(2,temp);
            break;
        case '3':
            //压力
            error = Get_Data(3,temp);
            break;
        case '4':
            //液位
            error = Get_Data(4,temp);
            break;
        case '5':
            //继电器
            error = Get_Data(5,temp);
            Delay_Cfg_Mode = 1;//继电器配置文件标志
            break;
        default:
        //错误
            flag = 0;
            printf("Save_Data：%d 配置信息错误!!!\n",c);
            break;
    }

    if(error == -1){
      Backups_Data();
      return;
    }
  }
  if(flag){
    printf("Save_Data：配置信息完成!!!");
  }

  if(n > 0 && flag)
   //只要有配置信息就设置标志位,并且配置成功
    Sensor_Cfg_Mode = 1;//串口接收传感器配置文件标志
}
/**
  * 函数功能: 清除传感器数据
  * 输入参数: 对应类型
  * 返 回 值: 无
  * 说    明：无
  */

void Clean_Data(int type){
  
  Sensor *p,*q;
      
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
    printf("Clean_Data：继电器信息清除完毕\n");
    return ;
  }

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
  sensor_array[type].warn= 0;
  sensor_array[type].up = 0;
  sensor_array[type].down = 0;
  sensor_array[type].frist_node = NULL;
  printf("Clean_Data：%d 传感器信息清除完毕\n",type);
  return;
}
/**
  * 函数功能: 上传传感器数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void UpData(){
  
  printf("UpData：开始上传数据\n");
  char devicesID[20] = {0};
  float value = 0.0;
  int type = 0,i = 0,j = 0;
  char data[50] = {0};
  Sensor *p = NULL;
  //打包数据 
  for(i=0;i<5;i++){
    //上传存在的传感器
   if(sensor_array[i].num >0){
    p = sensor_array[i].frist_node;
    type = sensor_data[i].type;
    value = sensor_data[i].value;
    sprintf(data,"%.2f,%d;",value,type);
    printf("UpData：上传数据：%.2f,%d\n",value,type);
    HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
    for(j=0;j<sensor_array[i].num;j++){
        value = p->value;
        strcpy(devicesID,p->devices);
        sprintf(data,"%.2f,%d,%s;",value,type,devicesID);
        printf("UpData：上传数据：%.2f,%d,%s\n",value,type,devicesID);
        HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
        p = p->next;
    }
   } 
  }
  HAL_UART_Transmit(&husart_debug,"#",1,1000);
  printf("UpData：上传数据完成\n");
}
/**
  * 函数功能: 计算传感器数据（平均值），判断异常,保证每次上传显示数据稳定
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Get_Average(){
  

  Sensor *p = NULL,*q = NULL;//q记录error次数最大的传感器
  float temp_value = 0;//暂存数据
  int e = 0,error = 0;//e记录最大错误次数,error为累加错误次数
  char data[20] = {0};
  
  printf("Get_Average：开始计算数据均值\n");
  for(int i=0;i<5;i++){
    p = sensor_array[i].frist_node;
    if(!p){
        continue;
    }
    while(p){
      //考虑全错的情况,全错的设备不进行平均值计算或者错误次数过多
        if(p->amount == 0 || p->error >30){
            p->value = 0;
            //告警
            sprintf(data,"!%s",p->devices);
            HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
            printf("Get_Average：%d号传感器%s错误\n",i,p->devices);
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
  * 返 回 值: 返回错误码 -1，配置信息有误，返回0 配置信息OK
  * 说    明：无
  */
int Get_Data(int type,char * data){
  
    char *result = NULL;
    Sensor *q = NULL;
    char *temp_data[50] = NULL;
    int i = 0,num = 0,n = 0,m = 0;//计数
    double min = 0.0,max = 0.0,up = 0.0,down = 0.0;
    
    result = strtok(data,",");
    //result = strtok( NULL,",");
    //提取数据
    while( result != NULL ) {
       result = strtok( NULL,",");
       temp_data[num] = result;
       num++;
    }
  
    if(num<10 && type != 5){
      //配置信息有误，所以回退
      printf("Get_Data:%d 配置信息错误,错误码%d\n",type,num);
      //Backups_Data();
      return -1;
    }
    
    if(type == 5){
        //继电器
        n = atoi(temp_data[0]);
        for(i=0;i<n;i++){
            strcpy(delay[i].devices,temp_data[5*i+1]);
            delay[i].port = temp_data[5*i+2][0];
            if(delay[i].port < '1' || delay[i].port > '4'){
              printf("Get_Data:%d 配置端口信息错误,错误码%d\n",type,delay[i].port);
              return -1;
            }
            delay[i].control = atoi(temp_data[5*i+3]);
            if(delay[i].control < 0 || delay[i].control > 4){
              printf("Get_Data:%d 配置控制模式信息错误,错误码%d\n",type,delay[i].control);
              return -1;
            }
            if(delay[i].control == 4){   
                    temp_data[5*i+4][5] = '\0';
                    strcpy(delay[i].start_time,temp_data[5*i+4]); 
                    strcpy(delay[i].sustain_time,&temp_data[5*i+4][6]); 
            }
            else if(delay[i].control == 3){
                temp_data[5*i+4][5] = '\0';
                strcpy(delay[i].interval_time,temp_data[5*i+4]); 
                strcpy(delay[i].sustain_time,&temp_data[5*i+4][6]); 
                
            }
            else{
                m = strlen(temp_data[5*i+4]);
                if(m < 0 || m > 5){
                  printf("Get_Data:%d 配置控制传感器信息错误,错误码%d\n",type,m);
                  return -1;
                }
                delay[i].num = m;
                for(int j=0;j<m;j++){
                    delay[j].type[j] = temp_data[5*i+4][j] - 48;
                }
            }
          
            delay[i].state = 0;
       }
       return 0;
    }
    sensor_array[type].type = type;
    sensor_array[type].num = atoi(temp_data[0]);
    //传感器数量不超过10
    if(sensor_array[type].num < 0 || sensor_array[type].num > 20){
      printf("Get_Data:%d 配置传感器数量信息错误,错误码%d\n",type,sensor_array[type].num);
      return -1;
    }
    min = strtod(temp_data[1],NULL);
    max = strtod(temp_data[2],NULL);
    sensor_array[type].min = min;
    sensor_array[type].max = max;
    
    up = strtod(temp_data[4],NULL);
    down = strtod(temp_data[3],NULL);
    sensor_array[type].up = up;
    sensor_array[type].down = down;
    sensor_array[type].warn = 0;
    sensor_array[type].frist_node = NULL;
    
    //建立传感器链表
    for(i=0;i<sensor_array[type].num;i++){
       // printf("%s %c %c\n",temp_data[3*i+3],temp_data[3*i+4][0],temp_data[3*i+5][0]);
        q = (Sensor *)malloc(sizeof(Sensor));
        strcpy(q->devices,temp_data[9*i+5]);
        strcpy(q->command,temp_data[9*i+6]);
        strcpy(q->parsetype,temp_data[9*i+7]);
        q->startadder =  atoi(temp_data[9*i+8]);
        if(q->startadder < 0 || q->startadder > 100){
          printf("Get_Data:%d 配置起始地址信息错误,错误码%d\n",type,q->startadder);
          return -1;
        }
        q->datanum = atoi(temp_data[9*i+9]);
        if(q->datanum < 0 || q->datanum > 100){
          printf("Get_Data:%d 配置数据位数信息错误,错误码%d\n",type,q->datanum);
          return -1;
        }
        q->keep = atoi(temp_data[9*i+10]);
        if(q->keep < 0 || q->keep > 100){
          printf("Get_Data:%d 配置保持位数信息错误,错误码%d\n",type,q->keep);
          return -1;
        }
        strcpy(q->mode,temp_data[9*i+11]);
        
        q->elec_4ma = atoi(temp_data[9*i+12]);
        if(q->elec_4ma < 0 || q->elec_4ma > 1000000){
          printf("Get_Data:%d 配置4ma对应信息错误,错误码%d\n",type,q->elec_4ma);
          return -1;
        }
        q->elec_20ma = atoi(temp_data[9*i+13]);
        if(q->elec_20ma < 0 || q->elec_20ma > 1000000){
          printf("Get_Data:%d 配置20ma对应信息错误,错误码%d\n",type,q->elec_20ma);
          return -1;
        }
        q->error = 0;
        q->value = 0;
        q->amount = 0;
        q->next = NULL;
        //插入链表
        if(sensor_array[type].frist_node == NULL){
            sensor_array[type].frist_node = q;
           // p->next = NULL;
        }
        else{
            q->next = sensor_array[type].frist_node;
            sensor_array[type].frist_node = q;
        }
     }//for
   // free(temp_data);//释放数据
    return 0;
 }
    
/**
  * 函数功能: 检测传感器异常
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Detection(){
  Sensor * p = NULL;
  int i = 0;
  char data[20] = {0};
  printf("Detection：开始检测传感器异常\n");
  for(i=0;i<5;i++){
    p = sensor_array[i].frist_node;

    while(p){

      //判断是否超出正常范围，进行蜂鸣器报警
  
      if(p->value > sensor_array[i].up || p->value < sensor_array[i].down)
            sensor_array[i].warn++;
           //清除数据
      p->error = 0;
      p->amount = 0;
      p->value = 0;
      p->elec_4ma = 0;
      p->elec_20ma = 0;
      p = p->next;
    }
    
    if(sensor_array[i].warn == sensor_array[i].num && sensor_array[i].num != 0){
      //发出告警
      printf("Detection：%d号传感器发出告警\n",i);
      sensor_array[i].warn = 0;
      Close_Light();
      Open_Beep();
      //蜂鸣器响5s
      HAL_Delay(5000); 
      Close_Beep();
      Open_Light();
    }     
  }

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
  char data[50] = {0};
  int flag = 1;
  switch(port){
    case '1':
      OUTPUT1_GPIO_ON;
      delay[0].state = 1;
      sprintf(data,"*%s,1,%d",delay[0].devices,delay[0].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '2':
      OUTPUT2_GPIO_ON;
      delay[1].state = 1;
      sprintf(data,"*%s,2,%d",delay[1].devices,delay[1].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '3':
      OUTPUT3_GPIO_ON;
      delay[2].state = 1;
      sprintf(data,"*%s,3,%d",delay[2].devices,delay[2].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '4':
      OUTPUT4_GPIO_ON;
      delay[3].state = 1;
      sprintf(data,"*%s,4,%d",delay[3].devices,delay[3].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    default:
      flag = 0;
      printf("Open_Delay:打开继电器失败，继电器端口错误%c!\n",port);
      break;
  }
  if(flag){
    printf("Open_Delay:打开%c端口继电器",port);
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
    int hours = 0,minute = 0;
    char temp[20] = {0};
    RTC_TimeTypeDef stimestructureget;
    /* 获取当前时间 */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
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
  char data[50] = {0};
  int flag = 1;

  RTC_CalendarShow();
    switch(port){
    case '1':
      OUTPUT1_GPIO_OFF; 
      delay[0].state = 0;
      sprintf(data,"*%s,1,%d",delay[0].devices,delay[0].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '2':
      OUTPUT2_GPIO_OFF; 
      delay[1].state = 0;
      sprintf(data,"*%s,2,%d",delay[1].devices,delay[1].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '3':
      OUTPUT3_GPIO_OFF;
      delay[2].state = 0;
      sprintf(data,"*%s,3,%d",delay[2].devices,delay[2].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    case '4':
      OUTPUT4_GPIO_OFF; 
      delay[3].state = 0;
      sprintf(data,"*%s,4,%d",delay[3].devices,delay[3].state);
      HAL_UART_Transmit(&husart_debug,data,strlen((char *)data),1000);
      break;
    default:
      flag = 0;
      printf("Close_Delay:关闭继电器失败，端口错误:%c\n",port);
      break;
  }
  if(flag){
    printf("Close_Delay:关闭%c端口继电器",port);      
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
 
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  /* 获取当前时间 */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* 获取当前日期 */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);

  /* 显示日期*/
  printf("当前时间为: %02d年%02d月%02d日", 2000+sdatestructureget.Year,sdatestructureget.Month, sdatestructureget.Date);
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
  int j = 0,n = 0,type = 0;
  
  printf("Control_min_from_max:继电器控制端口%d,开始控制\n",i);
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
  int j = 0,n = 0,type = 0;
 
  printf("Control_max_from_min:继电器控制端口%d,开始控制\n",i);
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
    
    char start_time[50] = {0},sustain_time[50] ={0};
    char temp_data[5][10] ={0};
    int n = 0;//计数
    char *result = NULL;
    //char hours[5],minutes[5],seconds[5];
   
    printf("Control_temer:继电器控制端口%d,开始控制\n",i);
    printf("Control_temer:从当前时间开始定时加,每%s加%s时间\n",delay[i].interval_time,delay[i].sustain_time);
    RTC_TimeTypeDef stimestructureget;
    /* 获取当前时间 */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
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
      printf("Control_temer：输入异常!\n");
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
    char start_time[50] = {0},sustain_time[50] = {0};
    char temp_data[5][10] = {0};
    int n = 0;//计数
    char *result = NULL;
        
    printf("Control_temer:继电器控制端口%d,开始控制\n",i);
    printf("Control_temer:每天特定时间加,每天%s加%s时间\n",delay[i].start_time,delay[i].sustain_time);
    
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
      printf("Control_sys_temer：输入异常!\n");
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
  int i = 0,n = 0;
  for(i=0;i<4;i++){
    if(delay[i].devices[0] != '0'){
      //有配置，根据不同传感器选择控制方式
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
    //printf("......%.2f\n",f);
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
			printf("movedigit：num erro!\n");
			break;

	}
       // printf("......%.2f\n",f);
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

   float value = 0.0;
   float value_float = 0.0;
   int value_int = 0;
   uint8_t buf[50] = {0};
   //strcpy(buf,data);
   //int data_len = strlen(buf);
   for(int i=0;i<RS485_Rx_Count_Old;i++)
      buf[i] = data[i];
   
   if(CRC16_MODBUS(buf,RS485_Rx_Count_Old) == 0){
        printf("Modbusprocess：传感器接收数据正确\n");
        //验证通过
   }
   else{
        printf("Modbusprocess：传感器接收数据错误\n");
        sensor->error++;
        Clear_RS485Buf();
        return;
   }
   
   if(!strcmp(sensor->parsetype,"Float")){
        if(!strcmp(sensor->mode,"big")){
            if(sensor->datanum == 4){
                temp[2] = buf[sensor->startadder-1];
                temp[3] = buf[sensor->startadder];
                temp[0] = buf[sensor->startadder + 1];
                temp[1] = buf[sensor->startadder + 2];
            }
            else{
                temp[2] = buf[sensor->startadder];
                temp[3] = buf[sensor->startadder-1];
                temp[0] = 0x00;
                temp[1] = 0x00;
            }

        }
	else{
            if(sensor->datanum == 4){
                temp[0] = buf[sensor->startadder-1];
                temp[1] = buf[sensor->startadder];
                temp[2] = buf[sensor->startadder + 1];
                temp[3] = buf[sensor->startadder + 2];
            }
            else{
                temp[2] = buf[sensor->startadder-1];
                temp[3] = buf[sensor->startadder];
                temp[0] = 0x00;
                temp[1] = 0x00;
			}
        }
        value_float = BitToFloat(temp);	
	value = value_float;

   }
   else if(!strcmp(sensor->parsetype,"Integer")){
        if(sensor->datanum == 2){
            temp[0] = 0x00;
            temp[1] = 0x00;
            temp[2] = buf[sensor->startadder - 1];
            temp[3] = buf[sensor->startadder];
	}
	else{
            temp[0] = buf[sensor->startadder-1];
            temp[1] = buf[sensor->startadder];
            temp[2] = buf[sensor->startadder + 1];
            temp[3] = buf[sensor->startadder + 2];
	}
	value_int = BitToInt(temp);
	value = movedigit(value_int,sensor->keep);			
					
    }
    else{
      printf("\nModbusprocess:Not compatibility\n");
        //continue;
        return;
    }
   
   printf("\nModbusprocess采集数据 value:%.2f\n",value);
   
   if(sensor->elec_4ma != 0 && sensor->elec_20ma != 0){
      value = ((float)value / (float)20000) * abs(sensor->elec_20ma - sensor->elec_4ma);
      printf("\nvalue:%.2f\n",value);
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
              //value = value * 0.08 + 185;
                    
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
          printf("传感器参数错误！\n");
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
  * 函数功能: 保存配置信息(掉电保持)
  * 输入参数: 字节组
  * 返 回 值: 无
  * 说    明：无
  */
void Save_Device_Data(char* buf){
  
  int size = strlen(buf);
  size = size + 1;
  char c[20] = {0};
    
  uint8_t buffer[1000];
  
  itoa(size,c);

  strcpy(Rx_buf,buf);
  strcpy(buffer,buf);
  
  buffer[size - 1] = '#';
  if(SCM_state == SCM_RUN)
    buffer[size] = '1';
  else
    buffer[size] = '0';
  
  buffer[size + 1] = '\0';
  
   size++;
  
    /* 擦除SPI的扇区以写入 */
  SPI_FLASH_SectorErase(FLASH_SectorToErase);	
 
  SPI_FLASH_BufferWrite(c, FLASH_WriteAddress,strlen(c));

  SPI_FLASH_BufferWrite(buffer, FLASH_WriteAddress+10,strlen(buffer));
  
  printf("Save_Device_Data：保存配置：%s  \n大小：%d\n",buffer,size);

  
}

/**
  * 函数功能: 整数转字符串
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：最大10位数
  */
void itoa (int n,char s[]){
  
    int i = 0,j = 0,sign = 0;
    char temp[10] = {0};
    if((sign = n) < 0)//记录符号
        n = -n;//使n成为正数
    do{
       temp[i++] = n % 10 + '0';//取下一个数字
    }
    while ((n /= 10) > 0);//删除该数字
    if(sign<0)
        temp[i++]='-';
    temp[i]='\0';
    
    for(j=i;j>=0;j--)//生成的数字是逆序的，所以要逆序输出
      s[i-j] = temp[j-1];
} 

/**
  * 函数功能: 获取配置信息
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void Get_Device_Data(char* buf){
  
  int n = 0;
  char size[10] = {0};
  uint8_t buffer[500] = {0};

  SPI_FLASH_BufferRead(size, FLASH_WriteAddress,10);
  n = atoi(size);
  n++;
  SPI_FLASH_BufferRead(buffer, FLASH_WriteAddress+10,n);

  
  printf("\n%s\n",buffer);
  if(buffer[n-1] == '0'){
    SCM_state = SCM_STOP;
    printf("系统当前状态stop\n");
  }
  else if(buffer[n-1] == '1'){
    SCM_state = SCM_RUN;
    printf("系统当前状态run\n");

  }
  else{
    SCM_state = SCM_STOP;
    printf("读取系统状态失败，系统处于停止状态\n");
  }
  
  if(buffer[n-2] == '#'){
    buffer[n-2] = '\0';
    strcpy(buf,buffer);
    printf("获取配置：%s\n",buffer);
    Save_Data();
  }
  else
    printf("Get_Device_Data：本地获取配置失败\n");
}


/**
  * 函数功能: 模拟模式采集液位值
  * 输入参数: 端口（<13）
  * 返 回 值: 返回1 没有检测液位  返回0 检测液位
  * 说    明：无
  */
int Simulation_Level(int port){
  
    if(INPUT_StateRead(port) == High)
    {
      printf("Simulation_Level:检测到液位\n");
      return 0;
    } 
    else{
      printf("Simulation_Level:没有检测到液位\n");
      return 1;
    }
   
}

/**
  * 函数功能: 程序重启
  * 输入参数: 
  * 返 回 值: 
  * 说    明：无
  */
void Reboot(){
  
    __set_FAULTMASK(1);//关闭总中断
    NVIC_SystemReset();//请求单片机重启
   
}

/**
  * 函数功能: 打开蜂鸣器
  * 输入参数: 
  * 返 回 值: 
  * 说    明：蜂鸣器接在1012 D1输出口上
  */
void Open_Beep(){
  
  RS485_Send_Data(D1_Open,(uint8_t)8);
   
}

/**
  * 函数功能: 关闭蜂鸣器
  * 输入参数: 
  * 返 回 值: 
  * 说    明：蜂鸣器接在1012 D1输出口上
  */
void Close_Beep(){
  
   RS485_Send_Data(D1_Close,(uint8_t)8);
}

/**
  * 函数功能: 打开绿灯
  * 输入参数: 
  * 返 回 值: 
  * 说    明：蜂鸣器接在1012 D0输出口上
  */
void Open_Light(){
  
  RS485_Send_Data(D0_Open,(uint8_t)8);
   
}

/**
  * 函数功能: 关闭绿灯
  * 输入参数: 
  * 返 回 值: 
  * 说    明：蜂鸣器接在1012 D0输出口上
  */
void Close_Light(){
  
    RS485_Send_Data(D0_Open,(uint8_t)8);
  
}


