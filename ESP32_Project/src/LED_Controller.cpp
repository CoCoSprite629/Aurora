#include <Arduino.h>
#include <FastLED.h>
#include "dataTransmit.h"
//#define LED_TYPE WS2812 // 验证阶段采用WS2812，实际成品使用WS12812B
#define LED_TYPE WS2812B
#define LIGHT_PIN 18
#define BRIGHT 5

CRGB led[1]; // 定义一个数组存储led颜色
String LED_Status = "";

void LED_Setup()
{
    LEDS.addLeds<LED_TYPE, LIGHT_PIN, GRB>(led, 1); // WS2812的色彩排列为GRB
    LEDS.setBrightness(BRIGHT);
}

void LED_Controller()
{
    if (LED_Status == "SET_WIFI")
    {
        // Serial.printf("\nI am Running");
        fill_solid(led, 1, CRGB(204, 153, 255));
        for (int i = 128; i >= 0; i--)
        {
            LEDS.setBrightness(i);
            FastLED.show();
            delay(10);
        }
        delay(200);
        for (int i = 0; i <= 128; i++)
        {
            LEDS.setBrightness(i);
            FastLED.show();
            delay(10);
        }
    }
    else if (LED_Status == "WIFI_IS_OK")   // 此灯效为获取天气时的过渡效果，用于两个阶段 1.WiFi连接完成，等待爬取天气 2.WiFi连接中
    {
        LEDS.setBrightness(10);
        for (int i = 200; i <= 360; i++)
        {
            fill_solid(led, 1, CHSV(i, 153, 255));
            FastLED.show();
            delay(10);
        }
        for (int i = 360; i >= 200; i--)
        {
            fill_solid(led, 1, CHSV(i, 153, 255));
            FastLED.show();
            delay(10);
        }
    }
    else if (LED_Status == "WIFI_FAIL")
    {
        fill_solid(led, 1, CRGB(255, 64, 64));
        for (int i = 128; i >= 0; i--)
        {
            LEDS.setBrightness(i);
            FastLED.show();
            delay(10);
        }
        delay(200);
        for (int i = 0; i <= 128; i++)
        {
            LEDS.setBrightness(i);
            FastLED.show();
            delay(10);
        }
    }
    else if (LED_Status == "GOT_WEATHER")
    {
        // temperature = -1;
        if (temperature.toInt() > 0 && temperature.toInt() <= 20) // 适宜
        {
            fill_solid(led, 1, CRGB(140, 99, 237));
            for (int i = 128; i >= 0; i--)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
            delay(200);
            for (int i = 0; i <= 128; i++)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
        }
        else if (temperature.toInt() > 20 && temperature.toInt() <= 30) // 较热
        {
            fill_solid(led, 1, CRGB(255, 153, 51));
            for (int i = 128; i >= 0; i--)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
            delay(200);
            for (int i = 0; i <= 128; i++)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
        }
        else if (temperature.toInt() > 30) // 酷暑
        {
            fill_solid(led, 1, CRGB(255, 51, 0));
            for (int i = 128; i >= 0; i--)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
            delay(200);
            for (int i = 0; i <= 128; i++)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
        }
        else if (temperature.toInt() < 0 && temperature.toInt() >= -10) // 冷
        {
            fill_solid(led, 1, CRGB(51, 153, 255));
            for (int i = 128; i >= 0; i--)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
            delay(200);
            for (int i = 0; i <= 128; i++)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
        }
        else if (temperature.toInt() < -10) // 极寒
        {
            fill_solid(led, 1, CRGB(255, 255, 255));
            for (int i = 128; i >= 0; i--)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
            delay(200);
            for (int i = 0; i <= 128; i++)
            {
                LEDS.setBrightness(i);
                FastLED.show();
                delay(10);
            }
        }
    }
    Serial.printf("\nDid one while, LED_STATUS: %s", LED_Status);
}