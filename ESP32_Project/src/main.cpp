/*
万物之宗
包含串口输入和处理函数
    api最终调用函数
    NTP时间服务更新函数
该项目采用FreeRTOS进行任务调度，loop函数不为最终单片机运行的总进程
*/

#include <Arduino.h>
#include <FastLED.h>
#include "WiFi_init.h"
#include "dataTransmit.h"
#include "LEDController.h"
#define POWER_DETEC 19
/*串口通信务必共地*/
/*定时器使用务必喂狗*/

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
// 配置多进程

//hw_timer_t *timer  = NULL;

String recvData;
int wifi = 0;
int getIP = 1;
int postpone = 0;
int dect = 0;


void onDataRecv() // 串口中断回调函数
{
  if (Serial2.available() > 0)
  {
    recvData = Serial2.readStringUntil('\r');
    Serial.printf("%s", recvData);
    if (recvData == "SERIAL_INIT")
    {
      ssid = Serial2.readStringUntil('\r');
      passwords = Serial2.readStringUntil('\r');
      // Serial.printf("\nSSID: %s\nPassword: %s", ssid, passwords);
      wifi = 1;
      pageID = 1;
    }
    else if (recvData == "WIFI_INFO")
    {
      ssid = Serial2.readStringUntil('\r');
      passwords = Serial2.readStringUntil('\r');
      wifi = 1;
      pageID = 3;
      // Serial.printf("\nSSID: %s\nPassword: %s", ssid, passwords);
    }
  }
}

void LEDControl(void *pvParameters)
{
  while (1)
  {
    LED_Controller();
    delay(20);
  }
}


void getInfo(void *pvParameters)
{
  while (1)
  {
    if (wifi == 1)
    {
      WiFi_Connect();
      wifi = 0;
    }
    else if (wifi == 2)
    {
      Serial.printf("SSID: %s\nPassword: %s", ssid, passwords);
      WiFi_Connect();
    }

    if (WiFiStatus == 1)
    {
      delay(2000);
      if (getIP == 1)
      {
        get_ADcode();
      }
      getTodayWeather();
      todayWeatherTransmit();
      getFestival();
      festivalTransmit();
      delay(1000);
      getFutureWeather();
      futureWeatherTransmit();
      delay(48000);
    }
    delay(300);  //防止死循环看门狗超时
  }
}

void setNTPServer(void *pvParameters)
{
  while (1)
  {
    if (WiFiStatus == 1)
    {
      if (postpone == 0)
      {
        delay(2000);
        postpone = 1;
      }
      setTime();
      setTimeTransmit();
      delay(1000);
    }
    delay(200);
    if (digitalRead(POWER_DETEC) == HIGH && dect == 0)
    {
      Serial2.printf("main.detection3.val=1\xff\xff\xff");
      dect = 1;
    }
    if (digitalRead(POWER_DETEC) == LOW)
    {
      dect = 0;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.onReceive(onDataRecv);
  LED_Setup();
  LED_Status = "SET_WIFI";

  /*
  timer = timerBegin(0, 80, true);  // 80分频，一次计数等于1us
  timerAttachInterrupt(timer, timerCallBack, true);  //边沿触发
  timerAlarmWrite(timer, 200000, true);
  timerAlarmEnable(timer);  //使能定时器
  */

  xTaskCreatePinnedToCore(
      getInfo, //配置进程运行主函数
      "Task1",  //进程名
      10000,    //设置堆栈大小
      NULL,     
      1,        //设置优先级
      &Task1,   //任务句柄
      0);       //在core 0运行
  delay(500);

  xTaskCreatePinnedToCore(
      setNTPServer,   
      "Task2", 
      10000,   
      NULL,    
      1,       
      &Task2,  
      1);      //在core 1运行
  delay(500);

  xTaskCreatePinnedToCore(
      LEDControl,   
      "Task3", 
      10000,   
      NULL,    
      2,       
      &Task3,  
      1);      //在core 1运行
  delay(500);
}

void loop()
{
  //任务使用FreeRTOS调度
}
