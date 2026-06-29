/*
项目核心库
包含NTP时间获取
    api信息的爬取
    主控-串口屏通信
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "WiFi_init.h"
#include "LEDController.h"
//#define debug

String result1; // 邮政编码
String result2; // 今日天气
String result3; // 未来天气
String result4;// 今日节日

String Location;
String province;
String city;
String weather;
String temperature;
String windPower;
String humidity;

String dayWeather1;
String nightWeather1;
String dayWeather2;
String nightWeather2;
String dayWeather3;
String nightWeather3;
String windPower1;
String windPower2;
String windPower3;

String festival;

String dat_week;
String time_;
String updateTime;
String allDate = "";
String formatMon;
String formatDay;
int passTime;
int seniorPassTime;
String week1, week2, week3;

String URL1 = "https://api.vore.top/api/IPdata?ip="; // 邮编获取
String URL2;                                         // 今日天气获取
String URL3;                                         // 天气预报获取
String URL4;
String key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

const char *location = "";
String dayTemperature1;
String nightTemperature1;
String dayTemperature2;
String nightTemperature2;
String dayTemperature3;
String nightTemperature3;
String updateTim;


/***********NTP时间同步配置***********/
const char *ntp_Sever = "time.windows.com"; // 微软服务器就是靠谱
// NTP服务器地址
const long gmt_Offset_sec = 3600 * 8;
// 时区，以中时区为基准，东八区为3600 * 8
const int daylightOffset_sec = 0;
// 不使用夏令时
/*********************************/

void get_ADcode()
{
    HTTPClient http1;
    // 初始化http
    http1.begin(URL1);
    // 发送请求
    int HttpCode1 = http1.GET();
    // 获取状态码
    while (1)
    {
        if (HttpCode1 > 0)
        {
            //Serial.println();
            // Serial.printf("httpCode:%d", HttpCode1);
            if (HttpCode1 == HTTP_CODE_OK)
            { // 200为成功接收
                result1 = http1.getString();
                // Serial.println(Result1);
                // Debug使用
                //  设置AruinoJson内存
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, result1);
                // 将结果存入doc
                location = doc["adcode"]["a"];
                // city = doc["ipdata"]["info3"];  //获取城市(不带所属等级)
                Location = String(location);
                Serial.printf("Location ADCode: %s", Location);
                break;
            }
        }
        else
        {
            Serial.println("Get Failed");
            Serial.printf("Error Code: %d", HttpCode1);
            // LED_Status = "WIFI_FAIL";
        }
        delay(1000);
    }
    http1.end();
}

void getTodayWeather()
{
    HTTPClient http2;
    URL2 = "https://restapi.amap.com/v3/weather/weatherInfo?city=" +
        Location + "&key=" + key + "&extensions=base";
    // Serial.println(URL2);
    http2.begin(URL2);
    int HttpCode2 = http2.GET();
    if (HttpCode2 > 0)
    {
        Serial.println();
        // Serial.printf("httpCode:%d", HttpCode2);
        if (HttpCode2 == HTTP_CODE_OK)
        {
            result2 = http2.getString();
            // Serial.println(Result2);
            DynamicJsonDocument doc(1024);
            // 设置Ardeclarations]uinoJson内存
            deserializeJson(doc, result2);
            // 将结果存入doc
            const char* City = doc["lives"][0]["city"];                // 获取所在城市
            city = String(City);
            const char* Province = doc["lives"][0]["province"];        // 获取所在省份
            province = String(Province);
            const char* temp = doc["lives"][0]["temperature"];  // 获取温度
            temperature = String(temp);
            const char* temp1 = doc["lives"][0]["weather"]; // 获取天气
            weather = String(temp1);                        // 转换成String型供图标检测
            const char* temp2 = doc["lives"][0]["humidity"];        // 获取空气湿度
            humidity = String(temp2);
            const char* temp3 = doc["lives"][0]["windpower"];      // 获取当前风力大小
            windPower = String(temp3);
            Serial.printf("Today's temperature: %s", String(weather));
            Serial.printf("\nAir Humidity is: %s", String(humidity));
            Serial.printf("\nLocation: %s·%s", province, city);
        }
        else
        {
            Serial.println("GET failed");
            Serial.printf("Error Code: %d", HttpCode2);
            // LED_Status = "WIFI_FAIL";
        }
    }
    http2.end();
}


void getFutureWeather()
{
    HTTPClient http3;
    URL3 = "https://restapi.amap.com/v3/weather/weatherInfo?city=" +
        Location + "&key=" + key + "&extensions=all";
    #if (debug)
        Serial.printf("\nURL3: %s", URL3);
    #endif
    http3.begin(URL3);
    int HttpCode3 = http3.GET();
    if (HttpCode3 > 0)
    {
        Serial.println();
        // Serial.printf("httpCode:%d", HttpCode2);
        if (HttpCode3 == HTTP_CODE_OK)
        {
            result3 = http3.getString();
            #if (debug)
                Serial.println(result3);
            #endif
            DynamicJsonDocument doc(2048);
            // 设置Ardeclarations]uinoJson内存
            deserializeJson(doc, result3);
            // 将结果存入doc
            updateTim = doc["forecasts"][0]["reporttime"].as<String>();
            dayTemperature1 = doc["forecasts"][0]["casts"][1]["daytemp"].as<String>();          // 获取日间温度
            nightTemperature1 = doc["forecasts"][0]["casts"][1]["nighttemp"].as<String>();      // 获取夜间温度
            dayTemperature2 = doc["forecasts"][0]["casts"][2]["daytemp"].as<String>();          // 获取日间温度
            nightTemperature2 = doc["forecasts"][0]["casts"][2]["nighttemp"].as<String>();      // 获取夜间温度
            dayTemperature3 = doc["forecasts"][0]["casts"][3]["daytemp"].as<String>();          // 获取日间温度
            nightTemperature3 = doc["forecasts"][0]["casts"][3]["nighttemp"].as<String>();      // 获取夜间温度
            #if (debug)
                Serial.printf("Weather: %s", dayTemperature3.c_str());
            #endif
            dayWeather1 = doc["forecasts"][0]["casts"][1]["dayweather"].as<String>();   // 获取日间天气
            dayWeather2 = doc["forecasts"][0]["casts"][2]["dayweather"].as<String>();   // 获取日间天气
            dayWeather3 = doc["forecasts"][0]["casts"][3]["dayweather"].as<String>();   // 获取日间天气
            windPower1 = doc["forecasts"][0]["casts"][1]["daypower"].as<String>();
            windPower2 = doc["forecasts"][0]["casts"][2]["daypower"].as<String>();
            windPower3 = doc["forecasts"][0]["casts"][3]["daypower"].as<String>();
        }
        else
        {
            Serial.println("GET failed");
            Serial.printf("Error Code: %d", HttpCode3);
            // LED_Status = "WIFI_FAIL";
        }
    }
    //Serial.println("Ending");
    http3.end();
}

void getFestival()
{
    HTTPClient http4;
    URL4 = "https://api.songzixian.com/api/holiday?dataSource=local_holiday&date=" + allDate;
    #if (debug)
        Serial.println(URL4);
    #endif
    http4.begin(URL4);
    int HttpCode4 = http4.GET();
    if (HttpCode4 > 0)
    {
        Serial.println();
        // Serial.printf("httpCode:%d", HttpCode2);
        if (HttpCode4 == HTTP_CODE_OK)
        {
            result4 = http4.getString();
            // Serial.println(Result2);
            DynamicJsonDocument doc(1024);
            // 设置Ardeclarations]uinoJson内存
            deserializeJson(doc, result4);
            // 将结果存入doc
            festival = doc["data"]["description"].as<String>();
        }
        else
        {
            Serial.println("GET failed");
            Serial.printf("Error Code: %d", HttpCode4);
        }
    }
    http4.end();
}

void festivalTransmit()
{
    Serial.begin(115200);
    Serial2.begin(115200);       
    Serial2.printf("main.t4.txt=\"%s\"\xff\xff\xff", festival.c_str());
}


void setWeatherPic1() // main主页大号气象图标
{
    if (weather == "晴")
    {
        Serial2.printf("main.p1.pic=7\xff\xff\xff");
    }
    else if (weather == "少云" || weather == "晴间多云" || weather == "多云")
    {
        Serial2.printf("main.p1.pic=101\xff\xff\xff");
    }
    else if (weather == "阴" || weather == "有风" || weather == "平静" || weather == "微风" ||
        weather == "和风" || weather == "清风" || weather == "强风" || weather == "劲风" ||
        weather == "疾风" || weather == "大风" || weather == "烈风" || weather == "强风/劲风")
    {
        Serial2.printf("main.p1.pic=102\xff\xff\xff");
    }
    else if (weather == "风暴" || weather == "狂暴风" || weather == "飓风" || weather == "热带风暴" ||
        weather == "龙卷风")
    {
        Serial2.printf("main.p1.pic=126\xff\xff\xff");
    }
    else if (weather == "霾" || weather == "中度霾" || weather == "重度霾" || weather == "严重霾")
    {
        Serial2.printf("main.p1.pic=117\xff\xff\xff");
    }
    else if (weather == "阵雨")
    {
        Serial2.printf("main.p1.pic=103\xff\xff\xff");
    }
    else if (weather == "强阵雨")
    {
        Serial2.printf("main.p1.pic=104\xff\xff\xff");
    }
    else if (weather == "雷阵雨")
    {
        Serial2.printf("main.p1.pic=105\xff\xff\xff");
    }
    else if (weather == "雷阵雨并伴有冰雹" || weather == "强雷阵雨")
    {
        Serial2.printf("main.p1.pic=106\xff\xff\xff");
    }
    else if (weather == "小雨")
    {
        Serial2.printf("main.p1.pic=107\xff\xff\xff");
    }
    else if (weather == "中雨" || weather == "小雨-中雨")
    {
        Serial2.printf("main.p1.pic=108\xff\xff\xff");
    }
    else if (weather == "大雨" || weather == "中雨-大雨")
    {
        Serial2.printf("main.p1.pic=109\xff\xff\xff");
    }
    else if (weather == "暴雨" || weather == "特大暴雨" || weather == "大暴雨" || weather == "极端降雨" ||
        weather == "大雨-暴雨" || weather == "暴雨-大暴雨" || weather == "大暴雨-特大暴雨")
    {
        Serial2.printf("main.p1.pic=110\xff\xff\xff");
    }
    else if (weather == "毛毛雨/细雨" || weather == "毛毛雨" || weather == "细雨" || weather == "雨")
    {
        Serial2.printf("main.p1.pic=111\xff\xff\xff");
    }
    else if (weather == "雨夹雪" || weather == "阵雨夹雪" || weather == "冻雨")
    {
        Serial2.printf("main.p1.pic=115\xff\xff\xff");
    }
    else if (weather == "小雪" || weather == "雨雪天气" || weather == "雪" || weather == "阵雪")
    {
        Serial2.printf("main.p1.pic=112\xff\xff\xff");
    }
    else if (weather == "中雪" || weather == "小雪-中雪")
    {
        Serial2.printf("main.p1.pic=113\xff\xff\xff");
    }
    else if (weather == "大雪" || weather == "暴雪" || weather == "大雪-暴雪" || weather == "中雪-大雪")
    {
        Serial2.printf("main.p1.pic=114\xff\xff\xff");
    }
    else if (weather == "浮尘")
    {
        Serial2.printf("main.p1.pic=119\xff\xff\xff");
    }
    else if (weather == "扬沙")
    {
        Serial2.printf("main.p1.pic=118\xff\xff\xff");
    }
    else if (weather == "沙尘暴")
    {
        Serial2.printf("main.p1.pic=120\xff\xff\xff");
    }
    else if (weather == "强沙尘暴")
    {
        Serial2.printf("main.p1.pic=121\xff\xff\xff");
    }
    else if (weather == "雾" || weather == "轻雾")
    {
        Serial2.printf("main.p1.pic=116\xff\xff\xff");
    }
    else if (weather == "浓雾" || weather == "大雾")
    {
        Serial2.printf("main.p1.pic=122\xff\xff\xff");
    }
    else if (weather == "强浓雾")
    {
        Serial2.printf("main.p1.pic=123\xff\xff\xff");
    }
    else if (weather == "特强浓雾")
    {
        Serial2.printf("main.p1.pic=124\xff\xff\xff");
    }
    else if (weather == "热")
    {
        Serial2.printf("main.p1.pic=131\xff\xff\xff");
    }
    else if (weather == "冷")
    {
        Serial2.printf("main.p1.pic=132\xff\xff\xff");
    }
    else if (weather == "未知")
    {
        Serial2.printf("main.p1.pic=125\xff\xff\xff");
    }
}
void todayWeatherTransmit()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial2.printf("main.t7.txt=\"%s·%s\"\xff\xff\xff", province, city);
    Serial2.printf("main.t5.txt=\"%s\"\xff\xff\xff", weather);
    Serial2.printf("main.t11.txt=\"%s\"\xff\xff\xff", temperature);
    Serial2.printf("main.t10.txt=\"%s\"\xff\xff\xff", humidity);
    Serial2.printf("main.t9.txt=\"%s\"\xff\xff\xff", windPower);
    Serial.printf("\n%s", weather);
    setWeatherPic1();
    LED_Status = "GOT_WEATHER";  
    Serial.printf("\nTodayWeatherSent");
}


void setWeatherPic2()
{
    if (weather == "晴")
    {
        Serial2.printf("MoreWeather.p1.pic=7\xff\xff\xff");
    }
    else if (weather == "少云" || weather == "晴间多云" || weather == "多云")
    {
        Serial2.printf("MoreWeather.p1.pic=101\xff\xff\xff");
    }
    else if (weather == "阴" || weather == "有风" || weather == "平静" || weather == "微风" ||
        weather == "和风" || weather == "清风" || weather == "强风" || weather == "劲风" ||
        weather == "疾风" || weather == "大风" || weather == "烈风" || weather == "强风/劲风")
    {
        Serial2.printf("MoreWeather.p1.pic=102\xff\xff\xff");
    }
    else if (weather == "风暴" || weather == "狂暴风" || weather == "飓风" || weather == "热带风暴" ||
        weather == "龙卷风")
    {
        Serial2.printf("MoreWeather.p1.pic=126\xff\xff\xff");
    }
    else if (weather == "霾" || weather == "中度霾" || weather == "重度霾" || weather == "严重霾")
    {
        Serial2.printf("MoreWeather.p1.pic=117\xff\xff\xff");
    }
    else if (weather == "阵雨")
    {
        Serial2.printf("MoreWeather.p1.pic=103\xff\xff\xff");
    }
    else if (weather == "强阵雨")
    {
        Serial2.printf("MoreWeather.p1.pic=104\xff\xff\xff");
    }
    else if (weather == "雷阵雨")
    {
        Serial2.printf("MoreWeather.p1.pic=105\xff\xff\xff");
    }
    else if (weather == "雷阵雨并伴有冰雹" || weather == "强雷阵雨")
    {
        Serial2.printf("MoreWeather.p1.pic=106\xff\xff\xff");
    }
    else if (weather == "小雨")
    {
        Serial2.printf("MoreWeather.p1.pic=107\xff\xff\xff");
    }
    else if (weather == "中雨" || weather == "小雨-中雨")
    {
        Serial2.printf("MoreWeather.p1.pic=108\xff\xff\xff");
    }
    else if (weather == "大雨" || weather == "中雨-大雨")
    {
        Serial2.printf("MoreWeather.p1.pic=109\xff\xff\xff");
    }
    else if (weather == "暴雨" || weather == "特大暴雨" || weather == "大暴雨" || weather == "极端降雨" ||
        weather == "大雨-暴雨" || weather == "暴雨-大暴雨" || weather == "大暴雨-特大暴雨")
    {
        Serial2.printf("MoreWeather.p1.pic=110\xff\xff\xff");
    }
    else if (weather == "毛毛雨/细雨" || weather == "毛毛雨" || weather == "细雨" || weather == "雨")
    {
        Serial2.printf("MoreWeather.p1.pic=111\xff\xff\xff");
    }
    else if (weather == "雨夹雪" || weather == "阵雨夹雪" || weather == "冻雨")
    {
        Serial2.printf("MoreWeather.p1.pic=115\xff\xff\xff");
    }
    else if (weather == "小雪" || weather == "雨雪天气" || weather == "雪" || weather == "阵雪")
    {
        Serial2.printf("MoreWeather.p1.pic=112\xff\xff\xff");
    }
    else if (weather == "中雪" || weather == "小雪-中雪")
    {
        Serial2.printf("MoreWeather.p1.pic=113\xff\xff\xff");
    }
    else if (weather == "大雪" || weather == "暴雪" || weather == "大雪-暴雪" || weather == "中雪-大雪")
    {
        Serial2.printf("MoreWeather.p1.pic=114\xff\xff\xff");
    }
    else if (weather == "浮尘")
    {
        Serial2.printf("MoreWeather.p1.pic=119\xff\xff\xff");
    }
    else if (weather == "扬沙")
    {
        Serial2.printf("MoreWeather.p1.pic=118\xff\xff\xff");
    }
    else if (weather == "沙尘暴")
    {
        Serial2.printf("MoreWeather.p1.pic=120\xff\xff\xff");
    }
    else if (weather == "强沙尘暴")
    {
        Serial2.printf("MoreWeather.p1.pic=121\xff\xff\xff");
    }
    else if (weather == "雾" || weather == "轻雾")
    {
        Serial2.printf("MoreWeather.p1.pic=116\xff\xff\xff");
    }
    else if (weather == "浓雾" || weather == "大雾")
    {
        Serial2.printf("MoreWeather.p1.pic=122\xff\xff\xff");
    }
    else if (weather == "强浓雾")
    {
        Serial2.printf("MoreWeather.p1.pic=123\xff\xff\xff");
    }
    else if (weather == "特强浓雾")
    {
        Serial2.printf("MoreWeather.p1.pic=124\xff\xff\xff");
    }
    else if (weather == "热")
    {
        Serial2.printf("MoreWeather.p1.pic=131\xff\xff\xff");
    }
    else if (weather == "冷")
    {
        Serial2.printf("MoreWeather.p1.pic=132\xff\xff\xff");
    }
    else if (weather == "未知")
    {
        Serial2.printf("MoreWeather.p1.pic=125\xff\xff\xff");
    }

    if (dayWeather1 == "晴")
    {
        Serial2.printf("MoreWeather.p3.pic=128\xff\xff\xff");
    }
    else if (dayWeather1 == "少云" || dayWeather1 == "晴间多云" || dayWeather1 == "多云")
    {
        Serial2.printf("MoreWeather.p3.pic=61\xff\xff\xff");
    }
    else if (dayWeather1 == "阴" || dayWeather1 == "有风" || dayWeather1 == "平静" || dayWeather1 == "微风" ||
        dayWeather1 == "和风" || dayWeather1 == "清风" || dayWeather1 == "强风" || dayWeather1 == "劲风" ||
        dayWeather1 == "疾风" || dayWeather1 == "大风" || dayWeather1 == "烈风" || dayWeather1 == "强风/劲风")
    {
        Serial2.printf("MoreWeather.p3.pic=75\xff\xff\xff");
    }
    else if (dayWeather1 == "风暴" || dayWeather1 == "狂暴风" || dayWeather1 == "飓风" || dayWeather1 == "热带风暴" ||
        dayWeather1 == "龙卷风")
    {
        Serial2.printf("MoreWeather.p3.pic=127\xff\xff\xff");
    }
    else if (dayWeather1 == "霾" || dayWeather1 == "中度霾" || dayWeather1 == "重度霾" || dayWeather1 == "严重霾")
    {
        Serial2.printf("MoreWeather.p3.pic=64\xff\xff\xff");
    }
    else if (dayWeather1 == "阵雨")
    {
        Serial2.printf("MoreWeather.p3.pic=133\xff\xff\xff");
    }
    else if (dayWeather1 == "强阵雨")
    {
        Serial2.printf("MoreWeather.p3.pic=77\xff\xff\xff");
    }
    else if (dayWeather1 == "雷阵雨")
    {
        Serial2.printf("MoreWeather.p3.pic=63\xff\xff\xff");
    }
    else if (dayWeather1 == "雷阵雨并伴有冰雹" || dayWeather1 == "强雷阵雨")
    {
        Serial2.printf("MoreWeather.p3.pic=67\xff\xff\xff");
    }
    else if (dayWeather1 == "小雨")
    {
        Serial2.printf("MoreWeather.p3.pic=72\xff\xff\xff");
    }
    else if (dayWeather1 == "中雨" || dayWeather1 == "小雨-中雨")
    {
        Serial2.printf("MoreWeather.p3.pic=78\xff\xff\xff");
    }
    else if (dayWeather1 == "大雨" || dayWeather1 == "中雨-大雨")
    {
        Serial2.printf("MoreWeather.p3.pic=60\xff\xff\xff");
    }
    else if (dayWeather1 == "暴雨" || dayWeather1 == "特大暴雨" || dayWeather1 == "大暴雨" || dayWeather1 == "极端降雨" ||
        dayWeather1 == "大雨-暴雨" || dayWeather1 == "暴雨-大暴雨" || dayWeather1 == "大暴雨-特大暴雨")
    {
        Serial2.printf("MoreWeather.p3.pic=58\xff\xff\xff");
    }
    else if (dayWeather1 == "毛毛雨/细雨" || dayWeather1 == "毛毛雨" || dayWeather1 == "细雨" || dayWeather1 == "雨")
    {
        Serial2.printf("MoreWeather.p3.pic=65\xff\xff\xff");
    }
    else if (dayWeather1 == "雨夹雪" || dayWeather1 == "阵雨夹雪" || dayWeather1 == "冻雨")
    {
        Serial2.printf("MoreWeather.p3.pic=76\xff\xff\xff");
    }
    else if (dayWeather1 == "小雪" || dayWeather1 == "雨雪天气" || dayWeather1 == "雪" || dayWeather1 == "阵雪")
    {
        Serial2.printf("MoreWeather.p3.pic=71\xff\xff\xff");
    }
    else if (dayWeather1 == "中雪" || dayWeather1 == "小雪-中雪")
    {
        Serial2.printf("MoreWeather.p3.pic=134\xff\xff\xff");
    }
    else if (dayWeather1 == "大雪" || dayWeather1 == "暴雪" || dayWeather1 == "大雪-暴雪" || dayWeather1 == "中雪-大雪")
    {
        Serial2.printf("MoreWeather.p3.pic=59\xff\xff\xff");
    }
    else if (dayWeather1 == "浮尘")
    {
        Serial2.printf("MoreWeather.p3.pic=62\xff\xff\xff");
    }
    else if (dayWeather1 == "扬沙")
    {
        Serial2.printf("MoreWeather.p3.pic=74\xff\xff\xff");
    }
    else if (dayWeather1 == "沙尘暴")
    {
        Serial2.printf("MoreWeather.p3.pic=70\xff\xff\xff");
    }
    else if (dayWeather1 == "强沙尘暴")
    {
        Serial2.printf("MoreWeather.p3.pic=69\xff\xff\xff");
    }
    else if (dayWeather1 == "雾" || dayWeather1 == "轻雾")
    {
        Serial2.printf("MoreWeather.p3.pic=57\xff\xff\xff");
    }
    else if (dayWeather1 == "浓雾" || dayWeather1 == "大雾")
    {
        Serial2.printf("MoreWeather.p3.pic=66\xff\xff\xff");
    }
    else if (dayWeather1 == "强浓雾")
    {
        Serial2.printf("MoreWeather.p3.pic=68\xff\xff\xff");
    }
    else if (dayWeather1 == "特强浓雾")
    {
        Serial2.printf("MoreWeather.p3.pic=73\xff\xff\xff");
    }
    else if (dayWeather1 == "热")
    {
        Serial2.printf("MoreWeather.p3.pic=130\xff\xff\xff");
    }
    else if (dayWeather1 == "冷")
    {
        Serial2.printf("MoreWeather.p3.pic=129\xff\xff\xff");
    }
    else if (dayWeather1 == "未知")
    {
        Serial2.printf("MoreWeather.p3.pic=100\xff\xff\xff");
    }

    if (dayWeather2 == "晴")
    {
        Serial2.printf("MoreWeather.p4.pic=128\xff\xff\xff");
    }
    else if (dayWeather2 == "少云" || dayWeather2 == "晴间多云" || dayWeather2 == "多云")
    {
        Serial2.printf("MoreWeather.p4.pic=61\xff\xff\xff");
    }
    else if (dayWeather2 == "阴" || dayWeather2 == "有风" || dayWeather2 == "平静" || dayWeather2 == "微风" ||
        dayWeather2 == "和风" || dayWeather2 == "清风" || dayWeather2 == "强风" || dayWeather2 == "劲风" ||
        dayWeather2 == "疾风" || dayWeather2 == "大风" || dayWeather2 == "烈风" || dayWeather2 == "强风/劲风")
    {
        Serial2.printf("MoreWeather.p4.pic=75\xff\xff\xff");
    }
    else if (dayWeather2 == "风暴" || dayWeather2 == "狂暴风" || dayWeather2 == "飓风" || dayWeather2 == "热带风暴" ||
        dayWeather2 == "龙卷风")
    {
        Serial2.printf("MoreWeather.p4.pic=127\xff\xff\xff");
    }
    else if (dayWeather2 == "霾" || dayWeather2 == "中度霾" || dayWeather2 == "重度霾" || dayWeather2 == "严重霾")
    {
        Serial2.printf("MoreWeather.p4.pic=64\xff\xff\xff");
    }
    else if (dayWeather2 == "阵雨")
    {
        Serial2.printf("MoreWeather.p4.pic=133\xff\xff\xff");
    }
    else if (dayWeather2 == "强阵雨")
    {
        Serial2.printf("MoreWeather.p4.pic=77\xff\xff\xff");
    }
    else if (dayWeather2 == "雷阵雨")
    {
        Serial2.printf("MoreWeather.p4.pic=63\xff\xff\xff");
    }
    else if (dayWeather2 == "雷阵雨并伴有冰雹" || dayWeather2 == "强雷阵雨")
    {
        Serial2.printf("MoreWeather.p4.pic=67\xff\xff\xff");
    }
    else if (dayWeather2 == "小雨")
    {
        Serial2.printf("MoreWeather.p4.pic=72\xff\xff\xff");
    }
    else if (dayWeather2 == "中雨" || dayWeather2 == "小雨-中雨")
    {
        Serial2.printf("MoreWeather.p4.pic=78\xff\xff\xff");
    }
    else if (dayWeather2 == "大雨" || dayWeather2 == "中雨-大雨")
    {
        Serial2.printf("MoreWeather.p4.pic=60\xff\xff\xff");
    }
    else if (dayWeather2 == "暴雨" || dayWeather2 == "特大暴雨" || dayWeather2 == "大暴雨" || dayWeather2 == "极端降雨" ||
        dayWeather2 == "大雨-暴雨" || dayWeather2 == "暴雨-大暴雨" || dayWeather2 == "大暴雨-特大暴雨")
    {
        Serial2.printf("MoreWeather.p4.pic=58\xff\xff\xff");
    }
    else if (dayWeather2 == "毛毛雨/细雨" || dayWeather2 == "毛毛雨" || dayWeather2 == "细雨" || dayWeather2 == "雨")
    {
        Serial2.printf("MoreWeather.p4.pic=65\xff\xff\xff");
    }
    else if (dayWeather2 == "雨夹雪" || dayWeather2 == "阵雨夹雪" || dayWeather2 == "冻雨")
    {
        Serial2.printf("MoreWeather.p4.pic=76\xff\xff\xff");
    }
    else if (dayWeather2 == "小雪" || dayWeather2 == "雨雪天气" || dayWeather2 == "雪" || dayWeather2 == "阵雪")
    {
        Serial2.printf("MoreWeather.p4.pic=71\xff\xff\xff");
    }
    else if (dayWeather2 == "中雪" || dayWeather2 == "小雪-中雪")
    {
        Serial2.printf("MoreWeather.p4.pic=134\xff\xff\xff");
    }
    else if (dayWeather2 == "大雪" || dayWeather2 == "暴雪" || dayWeather2 == "大雪-暴雪" || dayWeather2 == "中雪-大雪")
    {
        Serial2.printf("MoreWeather.p4.pic=59\xff\xff\xff");
    }
    else if (dayWeather2 == "浮尘")
    {
        Serial2.printf("MoreWeather.p4.pic=62\xff\xff\xff");
    }
    else if (dayWeather2 == "扬沙")
    {
        Serial2.printf("MoreWeather.p4.pic=74\xff\xff\xff");
    }
    else if (dayWeather2 == "沙尘暴")
    {
        Serial2.printf("MoreWeather.p4.pic=70\xff\xff\xff");
    }
    else if (dayWeather2 == "强沙尘暴")
    {
        Serial2.printf("MoreWeather.p4.pic=69\xff\xff\xff");
    }
    else if (dayWeather2 == "雾" || dayWeather2 == "轻雾")
    {
        Serial2.printf("MoreWeather.p4.pic=57\xff\xff\xff");
    }
    else if (dayWeather2 == "浓雾" || dayWeather2 == "大雾")
    {
        Serial2.printf("MoreWeather.p4.pic=66\xff\xff\xff");
    }
    else if (dayWeather2 == "强浓雾")
    {
        Serial2.printf("MoreWeather.p4.pic=68\xff\xff\xff");
    }
    else if (dayWeather2 == "特强浓雾")
    {
        Serial2.printf("MoreWeather.p4.pic=73\xff\xff\xff");
    }
    else if (dayWeather2 == "热")
    {
        Serial2.printf("MoreWeather.p4.pic=130\xff\xff\xff");
    }
    else if (dayWeather2 == "冷")
    {
        Serial2.printf("MoreWeather.p4.pic=129\xff\xff\xff");
    }
    else if (dayWeather2 == "未知")
    {
        Serial2.printf("MoreWeather.p4.pic=100\xff\xff\xff");
    }

    if (dayWeather3 == "晴")
    {
        Serial2.printf("MoreWeather.p5.pic=128\xff\xff\xff");
    }
    else if (dayWeather3 == "少云" || dayWeather3 == "晴间多云" || dayWeather3 == "多云")
    {
        Serial2.printf("MoreWeather.p5.pic=61\xff\xff\xff");
    }
    else if (dayWeather3 == "阴" || dayWeather3 == "有风" || dayWeather3 == "平静" || dayWeather3 == "微风" ||
        dayWeather3 == "和风" || dayWeather3 == "清风" || dayWeather3 == "强风" || dayWeather3 == "劲风" ||
        dayWeather3 == "疾风" || dayWeather3 == "大风" || dayWeather3 == "烈风" || dayWeather3 == "强风/劲风")
    {
        Serial2.printf("MoreWeather.p5.pic=75\xff\xff\xff");
    }
    else if (dayWeather3 == "风暴" || dayWeather3 == "狂暴风" || dayWeather3 == "飓风" || dayWeather3 == "热带风暴" ||
        dayWeather3 == "龙卷风")
    {
        Serial2.printf("MoreWeather.p5.pic=127\xff\xff\xff");
    }
    else if (dayWeather3 == "霾" || dayWeather3 == "中度霾" || dayWeather3 == "重度霾" || dayWeather3 == "严重霾")
    {
        Serial2.printf("MoreWeather.p5.pic=64\xff\xff\xff");
    }
    else if (dayWeather3 == "阵雨")
    {
        Serial2.printf("MoreWeather.p5.pic=133\xff\xff\xff");
    }
    else if (dayWeather3 == "强阵雨")
    {
        Serial2.printf("MoreWeather.p5.pic=77\xff\xff\xff");
    }
    else if (dayWeather3 == "雷阵雨")
    {
        Serial2.printf("MoreWeather.p5.pic=63\xff\xff\xff");
    }
    else if (dayWeather3 == "雷阵雨并伴有冰雹" || dayWeather3 == "强雷阵雨")
    {
        Serial2.printf("MoreWeather.p5.pic=67\xff\xff\xff");
    }
    else if (dayWeather3 == "小雨")
    {
        Serial2.printf("MoreWeather.p5.pic=72\xff\xff\xff");
    }
    else if (dayWeather3 == "中雨" || dayWeather3 == "小雨-中雨")
    {
        Serial2.printf("MoreWeather.p5.pic=78\xff\xff\xff");
    }
    else if (dayWeather3 == "大雨" || dayWeather3 == "中雨-大雨")
    {
        Serial2.printf("MoreWeather.p5.pic=60\xff\xff\xff");
    }
    else if (dayWeather3 == "暴雨" || dayWeather3 == "特大暴雨" || dayWeather3 == "大暴雨" || dayWeather3 == "极端降雨" ||
        dayWeather3 == "大雨-暴雨" || dayWeather3 == "暴雨-大暴雨" || dayWeather3 == "大暴雨-特大暴雨")
    {
        Serial2.printf("MoreWeather.p5.pic=58\xff\xff\xff");
    }
    else if (dayWeather3 == "毛毛雨/细雨" || dayWeather3 == "毛毛雨" || dayWeather3 == "细雨" || dayWeather3 == "雨")
    {
        Serial2.printf("MoreWeather.p5.pic=65\xff\xff\xff");
    }
    else if (dayWeather3 == "雨夹雪" || dayWeather3 == "阵雨夹雪" || dayWeather3 == "冻雨")
    {
        Serial2.printf("MoreWeather.p5.pic=76\xff\xff\xff");
    }
    else if (dayWeather3 == "小雪" || dayWeather3 == "雨雪天气" || dayWeather3 == "雪" || dayWeather3 == "阵雪")
    {
        Serial2.printf("MoreWeather.p5.pic=71\xff\xff\xff");
    }
    else if (dayWeather3 == "中雪" || dayWeather3 == "小雪-中雪")
    {
        Serial2.printf("MoreWeather.p5.pic=134\xff\xff\xff");
    }
    else if (dayWeather3 == "大雪" || dayWeather3 == "暴雪" || dayWeather3 == "大雪-暴雪" || dayWeather3 == "中雪-大雪")
    {
        Serial2.printf("MoreWeather.p5.pic=59\xff\xff\xff");
    }
    else if (dayWeather3 == "浮尘")
    {
        Serial2.printf("MoreWeather.p5.pic=62\xff\xff\xff");
    }
    else if (dayWeather3 == "扬沙")
    {
        Serial2.printf("MoreWeather.p5.pic=74\xff\xff\xff");
    }
    else if (dayWeather3 == "沙尘暴")
    {
        Serial2.printf("MoreWeather.p5.pic=70\xff\xff\xff");
    }
    else if (dayWeather3 == "强沙尘暴")
    {
        Serial2.printf("MoreWeather.p5.pic=69\xff\xff\xff");
    }
    else if (dayWeather3 == "雾" || dayWeather3 == "轻雾")
    {
        Serial2.printf("MoreWeather.p5.pic=57\xff\xff\xff");
    }
    else if (dayWeather3 == "浓雾" || dayWeather3 == "大雾")
    {
        Serial2.printf("MoreWeather.p5.pic=66\xff\xff\xff");
    }
    else if (dayWeather3 == "强浓雾")
    {
        Serial2.printf("MoreWeather.p5.pic=68\xff\xff\xff");
    }
    else if (dayWeather3 == "特强浓雾")
    {
        Serial2.printf("MoreWeather.p5.pic=73\xff\xff\xff");
    }
    else if (dayWeather3 == "热")
    {
        Serial2.printf("MoreWeather.p5.pic=130\xff\xff\xff");
    }
    else if (dayWeather3 == "冷")
    {
        Serial2.printf("MoreWeather.p5.pic=129\xff\xff\xff");
    }
    else if (dayWeather3 == "未知")
    {
        Serial2.printf("MoreWeather.p5.pic=100\xff\xff\xff");
    }
}


void futureWeatherTransmit()
{
    Serial.begin(115200);
    Serial.begin(115200);
    Serial2.printf("MoreWeather.t7.txt=\"%s·%s\"\xff\xff\xff", province, city);
    Serial2.printf("MoreWeather.t5.txt=\"%s\"\xff\xff\xff", weather);
    Serial2.printf("MoreWeather.t10.txt=\"%s\"\xff\xff\xff", temperature);
    Serial2.printf("MoreWeather.t9.txt=\"%s\"\xff\xff\xff", humidity);
    Serial2.printf("MoreWeather.t0.txt=\"%s\"\xff\xff\xff", updateTim.c_str());
    Serial.println("Sent apart");
    Serial2.printf("MoreWeather.t15.txt=\"%s\"\xff\xff\xff", dayTemperature1.c_str());
    Serial2.printf("MoreWeather.t16.txt=\"%s\"\xff\xff\xff", nightTemperature1.c_str());
    Serial2.printf("MoreWeather.t17.txt=\"%s\"\xff\xff\xff", dayTemperature2.c_str());
    Serial2.printf("MoreWeather.t18.txt=\"%s\"\xff\xff\xff", nightTemperature2.c_str());
    Serial2.printf("MoreWeather.t19.txt=\"%s\"\xff\xff\xff", dayTemperature3.c_str());
    Serial2.printf("MoreWeather.t20.txt=\"%s\"\xff\xff\xff", nightTemperature3.c_str());
    setWeatherPic2();
    Serial2.printf("MoreWeather.t12.txt=\"%s\"\xff\xff\xff", dayWeather1.c_str());
    Serial2.printf("MoreWeather.t13.txt=\"%s\"\xff\xff\xff", dayWeather2.c_str());
    Serial2.printf("MoreWeather.t14.txt=\"%s\"\xff\xff\xff", dayWeather3.c_str());
    Serial2.printf("MoreWeather.t21.txt=\"%s\"\xff\xff\xff", windPower1.c_str());
    Serial2.printf("MoreWeather.t22.txt=\"%s\"\xff\xff\xff", windPower2.c_str());
    Serial2.printf("MoreWeather.t23.txt=\"%s\"\xff\xff\xff", windPower3.c_str());
    //Serial2.printf("MoreWeather.t0.txt=\"%s\"\xff\xff\xff", updateTime);
}

void setTime()
{
    struct tm timeinfo;
    /*
    struct tm {
    int tm_sec;    // 秒，正常范围从 0 到 59，但允许至 61
    int tm_min;    // 分，范围从 0 到 59
    int tm_hour;   // 小时，范围从 0 到 23
    int tm_mday;   // 一月中的第几天，范围从 1 到 31
    int tm_mon;    // 一年中的第几月，范围从 0 到 11
    int tm_year;   // 自 1900 年起的年数
    int tm_wday;   // 一周中的第几天，范围从 0 到 6，从星期日算起
    int tm_yday;   // 一年中的第几天，范围从 0 到 365，从 1 月 1 日算起
    int tm_isdst;  // 夏令时
    };*/
    configTime(gmt_Offset_sec, daylightOffset_sec, ntp_Sever);
    int failure = 5;
    // 设置阻塞程序
    // 能在大多时候起作用但不排除少数情况
    while (failure >= 0)
    {
        if (getLocalTime(&timeinfo))
        {
            #if (debug)
                Serial.printf("\nSetTime Succeeded");
            #endif
            break;
        }
        else
        {
            configTime(gmt_Offset_sec, daylightOffset_sec, ntp_Sever);
            #if (debug)
                Serial.printf("\nSetTime_Failed");
            #endif
        }
        delay(500);
        failure -= 1;
    }
    // 如果获取失败，退出函数
    // Serial.println(&timeinfo, "%R %a");
    int year = (timeinfo.tm_year + 1900) % 100;
    int fullYear = timeinfo.tm_year + 1900;
    int month = timeinfo.tm_mon + 1;
    int day = timeinfo.tm_mday;
    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;
    int temp = timeinfo.tm_wday;
    String week;
    switch (temp)
    {
    case 0:
        week = "Sun";
        week1 = "星期一";
        week2 = "星期二";
        week3 = "星期三";
        break;
    case 1:
        week = "Mon";
        week1 = "星期二";
        week2 = "星期三";
        week3 = "星期四";
        break;
    case 2:
        week = "Tue";
        week1 = "星期三";
        week2 = "星期四";
        week3 = "星期五";
        break;
    case 3:
        week = "Wed";
        week1 = "星期四";
        week2 = "星期五";
        week3 = "星期六";
        break;
    case 4:
        week = "Thur";
        week1 = "星期五";
        week2 = "星期六";
        week3 = "星期天";
        break;
    case 5:
        week = "Fri";
        week1 = "星期六";
        week2 = "星期天";
        week3 = "星期一";
        break;
    case 6:
        week = "Sat";
        week1 = "星期天";
        week2 = "星期一";
        week3 = "星期二";
        break;
    default:
        break;
    }
    dat_week = String(week) + "·" + String(year) + "/" + String(month) + "/" + String(day);
    if (hour < 10 && minute >= 10)
    {
        time_ = 0 + String(hour) + ":" + String(minute);
    }
    else if (hour < 10 && minute < 10)
    {
        time_ = 0 + String(hour) + ":" + 0 + String(minute);
    }
    else if (hour >= 10 && minute >= 10)
    {
        time_ = String(hour) + ":" + String(minute);
    }
    else if (hour >= 10 && minute < 10)
    {
        time_ = String(hour) + ":" + 0 + String(minute);
    }
    #if (debug)
        Serial.println(time_);
    #endif

    updateTime = String(timeinfo.tm_year) + "/" + String(timeinfo.tm_mon) + "/" + String(timeinfo.tm_mday);

    if (timeinfo.tm_mon < 9)
    {
        formatMon = "0" + String(timeinfo.tm_mon + 1);
    }
    else
    {
        formatMon = String(timeinfo.tm_mon + 1);
    }
    if (timeinfo.tm_mday < 10)
    {
        formatDay = "0" + String(timeinfo.tm_mday);
    }
    else
    {
        formatDay = String(timeinfo.tm_mday);
    }

    allDate = String(timeinfo.tm_year + 1900) + "-" + formatMon + "-" + 
        formatDay;
    // Serial.println(dat_week);

    if (timeinfo.tm_min > 30)
    {
        passTime = ((hour + 1) * 100) / 24;
    }
    else
    {
        passTime = (hour * 100) / 24;
    }

    //入学时间：2023.9.1
    //结算时间: 2026.6.9
    //大约1045天
    seniorPassTime = (((month * 30) + (fullYear - 2024) * 365 + 125) * 100) / 1045;
    #if (debug)
        Serial.println(seniorPassTime);
    #endif


}

void setTimeTransmit()
{
    Serial2.flush();
    Serial2.printf("main.t1.txt=\"%s\"\xff\xff\xff", dat_week); // 传日期星期年份
    Serial2.printf("main.t0.txt=\"%s\"\xff\xff\xff", time_);    // 传时间
    Serial2.printf("main.j0.val=%d\xff\xff\xff", passTime);    // 传一天已过去进度条数据
    Serial2.printf("main.j1.val=%d\xff\xff\xff", seniorPassTime);  // 传高中已过去进度条数据
    Serial2.printf("MoreWeather.t1.txt=\"%s\"\xff\xff\xff", week1);  // 传高中已过去进度条数据
    Serial2.printf("MoreWeather.t4.txt=\"%s\"\xff\xff\xff", week2);
    Serial2.printf("MoreWeather.t11.txt=\"%s\"\xff\xff\xff", week3);
    #if (debug)
        Serial.println("TimeSet");
    #endif
}
