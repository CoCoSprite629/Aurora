#include <Arduino.h>
#include <WiFi.h>
#include "LEDController.h"

String ssid;
String passwords;
const char *SSID = "";     // 账号
const char *Password = ""; // 密码
//int send = 0;
int pageID = 0;
int WiFiStatus = 0;

void WiFi_Error1()   //在main页面的时候执行此错误函数
{
    Serial2.begin(115200);
    Serial.begin(115200);

    Serial2.printf("main.b6.x=235\xff\xff\xff");
    Serial2.printf("main.b6.y=169\xff\xff\xff");
    Serial2.printf("main.b5.x=128\xff\xff\xff");
    Serial2.printf("main.b5.y=169\xff\xff\xff");
    Serial2.printf("main.p2.x=98\xff\xff\xff");
    Serial2.printf("main.p2.y=36\xff\xff\xff");
    Serial2.printf("main.detection.val=1\xff\xff\xff");
    Serial2.printf("WiFi.p0.pic=32\xff\xff\xff");
    Serial.printf("\nSent");
}

void WiFi_Error2()       //处在WiFi页面的时候执行此错误函数
{
    Serial2.begin(115200);
    Serial.begin(115200);

    Serial2.printf("WiFi.error.val=1\xff\xff\xff");
    Serial2.printf("WiFi.p0.pic=32\xff\xff\xff");
}

void WiFi_Connected()
{
    Serial.begin(115200);
    Serial2.begin(115200);

    Serial2.printf("WiFi.error.val=2\xff\xff\xff");
}

void WiFi_Connect()
{
    LED_Status = "WIFI_IS_OK";
    String temp1 = ssid.substring(1); // 去除所得数据前面的\r字符
    String temp2 = passwords.substring(1);
    SSID = temp1.c_str();
    Password = temp2.c_str();
    Serial.printf("SSID: %sPasswords: %s", SSID, Password);
    WiFi.begin(SSID, Password);
    Serial.printf("\nConnecting");
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
        counter++;
        if (counter == 5)
        {
            break;
        }
    }
    // 阻塞程序
    // 直到连接成功
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected!");
        WiFiStatus = 1;
        if (pageID == 3)
        {
            WiFi_Connected();
        }
    }
    else
    {
        Serial.print("Connect Failed");
        LED_Status = "WIFI_FAIL";
        switch (pageID)
        {
            case 1:
                WiFi_Error1();
                break;
            case 3:
                WiFi_Error2();
                break;
        }
    }
}