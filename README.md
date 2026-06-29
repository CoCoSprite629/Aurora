# 🌌 Aurora

> 基于 ESP32 的桌面智能天气终端 ｜ IoT Desktop Smart Weather Display

**Aurora**（曙光女神）是一款基于 **ESP32** 的桌面智能物联网终端。通过 WiFi 联网获取实时天气、未来预报、NTP 网络时间与节日信息，将数据呈现在 **HMI 串口触摸屏**上，同时以 **WS2812B 可编程 LED** 的呼吸灯效反馈系统状态。整机配备 3D 打印外壳，适合作为桌面摆件、电子手工作品或物联网学习项目。

---

![Aurora](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif)
![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?logo=arduino)
![License](https://img.shields.io/badge/License-GPL%202.0-green)
![Build](https://img.shields.io/badge/Build-PlatformIO-orange?logo=platformio)

---

## 功能特性

- 🌤️ **实时天气显示** — 调用高德天气 API，获取当前温度、湿度、风力及天气状况
- 📅 **未来三日预报** — 滚动展示未来三天的日间/夜间天气与温度
- ⏰ **NTP 网络时间** — 自动同步微软 NTP 服务器（`time.windows.com`），无需手动校准
- 🎉 **节日提醒** — 每日自动查询当天节日/纪念日，展示在屏幕底部
- 💡 **智能 LED 反馈** — 根据 WiFi 状态与温度范围变换颜色与呼吸节奏
- 📟 **HMI 串口屏交互** — 通过 Serial2 自定义协议驱动触摸屏，支持配网输入
- 📊 **双进度条** — 一天已过进度 + 自定义倒计时进度条
- 🧵 **FreeRTOS 多任务调度** — 3 个任务跨 Core 0 / Core 1 高效协同
- 🖨️ **3D 打印外壳** — 提供完整 STL 模型，可自行打印装配

---

## 硬件清单

| 组件 | 型号/规格 | 数量 | 说明 |
|------|----------|:----:|------|
| 主控 | ESP32 DOIT DevKit V1 | 1 | WiFi + 蓝牙双模 |
| LED | WS2812B 可编程灯珠 | 1 | 状态指示灯，GRB 色彩排列 |
| 触摸屏 | 陶晶驰 (TJC) HMI 串口屏 | 1 | 用户交互界面 |
| 电源 | 5V / Micro-USB | 1 | 供电 |
| 检测脚 | 外部传感器 / 按键 (可选) | 1 | GPIO 19 输入检测 |

### 引脚连接

| 功能 | ESP32 引脚 | 说明 |
|------|:--------:|------|
| WS2812B 数据线 (DI) | **GPIO 18** | FastLED 驱动 |
| 串口屏 RX (Serial2) | **GPIO 16** | ESP32 ← 屏幕 |
| 串口屏 TX (Serial2) | **GPIO 17** | ESP32 → 屏幕 |
| 电源检测 / 外部输入 | **GPIO 19** | 高电平触发屏幕弹窗 |

> ⚠️ **注意：** ESP32 与串口屏通信**务必共地**，否则可能烧毁串口。

---

## 项目结构

```
Aurora/
├── README.md                        # 项目说明（本文件）
├── ESP32_Project/                   # ESP32 PlatformIO 主工程
│   ├── platformio.ini               # PlatformIO 项目配置
│   ├── src/
│   │   ├── main.cpp                 # 主程序入口 + FreeRTOS 任务
│   │   ├── WiFi_init.cpp            # WiFi 连接与错误处理
│   │   ├── dataTransmit.cpp         # API 请求、NTP、HMI 发送
│   │   └── LED_Controller.cpp       # WS2812B 灯效控制
│   ├── include/
│   │   ├── WiFi_init.h              # WiFi 相关声明
│   │   ├── dataTransmit.h           # 数据获取/发送声明
│   │   └── LEDController.h          # LED 控制声明
│   ├── lib/                         # 项目私有库目录
│   └── test/                        # 单元测试目录
├── PCB_Files/                       # 立创 EDA 专业版工程
│   ├── 主控板.epro                   # 主控板 PCB
│   ├── 电源板.epro                   # 电源板 PCB
│   └── 连接板.epro                   # 连接板 PCB
├── UartHmi_Project/                 # 陶晶驰串口屏工程
│   ├── RainbowMedia.HMI             # HMI 工程主文件
│   ├── *.zi                         # 字库资源文件
│   └── 天气icons/                   # 天气图标素材
├── model/                           # 3D 打印外壳模型
│   ├── 屏幕外壳.stl
│   ├── 屏幕外壳盖板.stl
│   ├── 屏幕支架.stl
│   ├── 底座.stl
│   ├── 底座盖板.stl
│   └── 主控屏幕连接.stl
├── fonts/                           # 中文字体源文件
│   ├── 字由文艺黑.ttf
│   ├── 懒设计字由公益体.ttf
│   ├── 阿里妈妈数黑体 Bold.ttf
│   └── 阿里巴巴普惠体-B.ttf
├── icons/                           # 天气图标（备用）
└── info/                            # 项目规划 XMind 脑图
    ├── Aurora总规划.xmind
    ├── Aurora代码逻辑.xmind
    ├── Aurora主控文件结构.xmind
    └── Aurora灯效.xmind
```

---

## 系统架构

```
┌──────────────────────────────────────────────────┐
│                    Aurora                        │
│                                                  │
│  ┌──────────────┐    Serial2     ┌────────────┐  │
│  │  HMI 触摸屏  │◄──────────────►│  ESP32     │  │
│  │  (陶晶驰TJC) │   115200 bps   │  DOIT V1   │  │
│  └──────────────┘                │            │  │
│                                  │  WS2812B ◄─┼── GPIO 18
│  ┌──────────────────────────┐    │            │  │
│  │      互联网 API          │    │  Detec ◄───┼── GPIO 19
│  │                          │    └──────┬─────┘  │
│  │  • 高德天气 API          │           │ WiFi    │
│  │  • vore.top IP 定位     │◄──────────┘        │
│  │  • songzixian 节日查询  │                    │
│  │  • time.windows.com NTP │                    │
│  └──────────────────────────┘                   │
└──────────────────────────────────────────────────┘
```

### 数据流

```
HMI 屏幕 ──(配网信息)──► Serial2 ──► ESP32 ──(WiFi)──► 互联网 API
                                                         │
HMI 屏幕 ◄──(天气/时间)── Serial2 ◄── ESP32 ◄──(JSON)───┘
                                                         
WS2812B LED ◄── 呼吸灯效 ── LED_Controller() (Core 1)
```

---

## 快速上手

### 前置要求

| 工具 | 用途 |
|------|------|
| [Visual Studio Code](https://code.visualstudio.com/) | 代码编辑器 |
| [PlatformIO IDE 插件](https://platformio.org/install/ide?install=vscode) | ESP32 编译烧录 |
| [立创 EDA 专业版](https://pro.lceda.cn/) | PCB 工程查看/编辑 |
| [陶晶驰上位机 (USART HMI)](http://www.tjc1688.com/) | 串口屏工程导入/编辑 |
| Micro-USB 数据线 | 烧录与供电 |

### 编译烧录

```bash
# 1. 克隆仓库
git clone https://github.com/your-username/Aurora.git
cd Aurora/ESP32_Project

# 2. 修改高德 API Key（重要！）
# 编辑 src/dataTransmit.cpp 第 55 行
# 将 key 变量替换为你自己的高德开放平台 API Key
String key = "你的高德API Key";

# 3. （可选）修改高中倒计时参数
# 编辑 src/dataTransmit.cpp 第 1057-1059 行，调整入学与毕业日期

# 4. 用 VS Code 打开 ESP32_Project 目录，点击 PlatformIO: Upload
```

> 也可以在终端中使用 PlatformIO Core CLI：
> ```bash
> cd ESP32_Project
> pio run --target upload
> ```

### 首次配网

1. 设备上电后，HMI 屏幕自动进入主页，LED 呈**紫色呼吸**（等待配网状态）
2. 通过屏幕输入 WiFi SSID 与密码
3. 屏幕向 ESP32 发送 `SERIAL_INIT` 指令，ESP32 开始连接
4. 连接成功后，LED 切换为**青紫色循环**（正在获取天气）
5. 获取天气数据后，LED 切换为**温度对应颜色呼吸**（正常运行）

---

## 配置说明

### 必须修改的参数

| 参数 | 文件 | 行号 | 说明 |
|------|------|:----:|------|
| `key` | `dataTransmit.cpp` | L55 | 高德开放平台 API Key，**必须替换** |

### 可调参数

| 参数 | 文件 | 行号 | 默认值 | 说明 |
|------|------|:----:|--------|------|
| `BRIGHT` | `LED_Controller.cpp` | L7 | `5` | LED 全局亮度 (0-255) |
| `LIGHT_PIN` | `LED_Controller.cpp` | L6 | `18` | WS2812B 数据引脚 |
| `POWER_DETEC` | `main.cpp` | L14 | `19` | 外部检测引脚 |
| `ntp_Sever` | `dataTransmit.cpp` | L68 | `time.windows.com` | NTP 时间服务器 |
| `gmt_Offset_sec` | `dataTransmit.cpp` | L70 | `3600 * 8` | 时区偏移（东八区） |
| 高中倒计时 | `dataTransmit.cpp` | L1057-1059 | 2023.9.1 → 2026.6.9 | 自定义倒计时参数 |

### LED 灯效颜色

| 温度范围 | 颜色 | RGB |
|----------|------|-----|
| `> 30°C` 酷暑 | 🔴 红色 | `(255, 51, 0)` |
| `20 ~ 30°C` 较热 | 🟠 橙色 | `(255, 153, 51)` |
| `0 ~ 20°C` 适宜 | 🟣 紫色 | `(140, 99, 237)` |
| `-10 ~ 0°C` 冷 | 🔵 蓝色 | `(51, 153, 255)` |
| `< -10°C` 极寒 | ⚪ 白色 | `(255, 255, 255)` |

---

## 通信协议

### 串口屏幕指令（ESP32 → HMI）

屏幕与 ESP32 通过 **Serial2**（GPIO 16/17，115200 bps）通信。

| 指令格式 | 说明 | 示例 |
|----------|------|------|
| `控件.属性=值\xff\xff\xff` | 控制屏幕控件 | `main.t1.txt="Mon·26/6/29"\xff\xff\xff` |
| `页面.图片.pic=N\xff\xff\xff` | 切换图片资源 | `main.p1.pic=7\xff\xff\xff` |
| `页面.进度条.val=N\xff\xff\xff` | 设置进度条 | `main.j0.val=75\xff\xff\xff` |
| `页面.检测.val=N\xff\xff\xff` | 触发检测动画 | `main.detection3.val=1\xff\xff\xff` |

### 串口屏幕指令（HMI → ESP32）

屏幕通过 `\r`（回车符）分隔的字符串向 ESP32 发送信号：

| 指令 | 后续参数 | 含义 |
|------|---------|------|
| `SERIAL_INIT` | SSID `\r` Password | 主页配网（pageID = 1） |
| `WIFI_INFO` | SSID `\r` Password | WiFi 页面配网（pageID = 3） |

### HMI 页面结构

| 页面名 | 用途 | 主要控件 |
|--------|------|----------|
| `main` | 主页 | 时间 `t0`、日期 `t1`、城市 `t7`、天气 `t5`、温度 `t11`、湿度 `t10`、风力 `t9`、节日 `t4`、天气图标 `p1`、进度条 `j0`/`j1` |
| `WiFi` | 配网页面 | 连接状态 `error.val`、错误图标 `p0` |
| `MoreWeather` | 天气预报 | 今日详情 + 未来三天 `p3`/`p4`/`p5`、日期星期、温度风力 |

### API 接口

| 函数 | 请求 URL | 用途 |
|------|----------|------|
| `get_ADcode()` | `https://api.vore.top/api/IPdata?ip=` | 获取 IP 对应行政区划编码 |
| `getTodayWeather()` | `https://restapi.amap.com/v3/weather/weatherInfo?city=ADcode&key=KEY&extensions=base` | 高德实时天气（base 模式） |
| `getFutureWeather()` | `https://restapi.amap.com/v3/weather/weatherInfo?city=ADcode&key=KEY&extensions=all` | 高德未来三日预报（all 模式） |
| `getFestival()` | `https://api.songzixian.com/api/holiday?dataSource=local_holiday&date=yyyy-MM-dd` | 查询当天节日 |

---

## LED 动画参考

| 状态 | 颜色 | 效果 | 触发条件 |
|------|------|------|---------|
| 🔌 **等待配网** `SET_WIFI` | 紫色 `(204, 153, 255)` | 呼吸脉冲 (0→128→0) | 上电初始化 |
| 📡 **WiFi 连接中** `WIFI_IS_OK` | HSV 色相循环 (200→360) | 低亮度色相扫描 | 发起 WiFi 连接 / 等待天气 |
| ❌ **连接失败** `WIFI_FAIL` | 红色 `(255, 64, 64)` | 呼吸脉冲 (0→128→0) | 5 秒内未连接成功 |
| ✅ **天气就绪** `GOT_WEATHER` | 根据温度变化 (见上表) | 呼吸脉冲 (0→128→0) | 天气数据获取完成 |
| 🚨 **外部触发** | 屏幕弹窗 `detection3.val=1` | — | GPIO 19 高电平 |

---

## 软件架构

```
setup()
  ├── Serial.begin(115200)          // USB 调试串口
  ├── Serial2.begin(115200)         // HMI 屏幕串口
  ├── Serial2.onReceive(onDataRecv) // 注册串口中断回调
  ├── LED_Setup()                   // 初始化 WS2812B / FastLED
  ├── LED_Status = "SET_WIFI"       // 初始灯效状态
  └── xTaskCreatePinnedToCore × 3   // 创建 FreeRTOS 任务

┌─ Task1: getInfo() [Core 0, Priority 1] ─────────────────────┐
│  wifi == 1 → WiFi_Connect()                                  │
│  WiFiStatus == 1 → get_ADcode() → getTodayWeather()         │
│                  → todayWeatherTransmit() → getFestival()    │
│                  → festivalTransmit() → getFutureWeather()  │
│                  → futureWeatherTransmit() → delay(48s)      │
└──────────────────────────────────────────────────────────────┘

┌─ Task2: setNTPServer() [Core 1, Priority 1] ────────────────┐
│  WiFiStatus == 1 → setTime() → setTimeTransmit()  (每 1s)    │
│  digitalRead(POWER_DETEC) == HIGH → 触发检测弹窗             │
└──────────────────────────────────────────────────────────────┘

┌─ Task3: LEDControl() [Core 1, Priority 2] ──────────────────┐
│  LED_Controller() → 根据 LED_Status 执行对应灯效  (每 20ms)  │
└──────────────────────────────────────────────────────────────┘

Serial2 中断回调 (onDataRecv)
  解析屏幕指令 → 提取 SSID/Password → 置 wifi = 1
```

> `loop()` 函数为空 — 所有逻辑均由 FreeRTOS 调度。

---

## 依赖库

| 库 | 版本 | 用途 |
|----|------|------|
| [FastLED](https://github.com/FastLED/FastLED) | ^3.7.3 | WS2812B LED 驱动（GRB 色彩顺序） |
| [ArduinoJson](https://arduinojson.org/) | 6.20.0 | JSON 解析（天气/节日 API 响应） |
| WiFi (ESP32 内置) | — | WiFi 连接与 HTTP 请求 |
| FreeRTOS (ESP32 内置) | — | 多任务调度 |

---

## 开发环境

- **IDE**: VS Code + PlatformIO
- **开发板**: ESP32 DOIT DevKit V1 (`esp32doit-devkit-v1`)
- **分区方案**: `huge_app.csv`
- **框架**: Arduino for ESP32
- **串口速率**: 115200 bps (Serial + Serial2)

---

## 工程文件打开说明

### PCB 工程

`PCB_Files/` 目录下包含三块电路板的 PCB 设计文件：

| 文件 | 说明 |
|------|------|
| `主控板.epro` | ESP32 主控板 |
| `电源板.epro` | 电源管理板 |
| `连接板.epro` | 板间连接板 |

> 📐 **请使用[立创 EDA 专业版](https://pro.lceda.cn/)** 打开 `.epro` 格式的工程文件。立创 EDA 标准版无法识别此格式。

### 串口屏工程

`UartHmi_Project/` 目录包含陶晶驰 (TJC) 串口屏的完整工程：

| 文件 | 说明 |
|------|------|
| `RainbowMedia.HMI` | HMI 工程主文件（可编译/烧录） |
| `*.zi` | 字库资源文件（main, para, big 等多尺寸字库） |
| `天气icons/` | 天气图标素材（PNG 格式） |

> 🖥️ **请使用[陶晶驰 USART HMI 上位机](http://www.tjc1688.com/)** 导入 `.HMI` 工程文件。导入后可编辑界面、编译并烧录到屏幕。

### 3D 打印外壳

`model/` 目录下包含 6 个 STL 文件，构成完整外壳：

1. `屏幕外壳.stl` + `屏幕外壳盖板.stl` — 屏幕外壳组件
2. `屏幕支架.stl` — 屏幕固定支架
3. `底座.stl` + `底座盖板.stl` — 底座组件
4. `主控屏幕连接.stl` — 主控板与屏幕的连接件

> 建议使用 PLA 材质打印，0.2mm 层高，无需支撑（具体视模型结构而定）。

---

## 使用场景

1. 将 ESP32 烧录固件后，装入 3D 打印外壳
2. 上电后，LED 呈紫色呼吸，等待配网
3. 在 HMI 触摸屏输入 WiFi 账号密码完成配网
4. 自动获取 IP 地理位置 → 查询当地天气 → 展示在主页
5. 左滑进入 `MoreWeather` 页面查看未来三日详细预报
6. LED 根据当前温度自动切换颜色呼吸
7. 每日自动展示当天节日/纪念日信息

---

## 项目历程

| 日期 | 里程碑 |
|------|--------|
| 7.10 | RainbowTimer 的 Plus Ultra 版本正式立项，取名 **RainbowMedia** |
| 7.15 | 舍弃 LVGL 图形库，全面转向串口屏方案 |
| 7.22 | 正式更名为 **Aurora**（曙光女神），完成初步功能验证 |
| 8.22 | 3D 外壳建模完成，项目进入收尾阶段 |

---

## 开源协议

本项目采用 **GNU General Public License v2.0 (GPL 2.0)** 开源协议。

> 你可以自由地使用、修改和分发本项目，但衍生作品也必须以 GPL 2.0 协议开源。

详细信息请参阅 [LICENSE](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html) 文件。

---

## 致谢

- [高德开放平台](https://lbs.amap.com/) — 天气数据 API
- [FastLED](https://github.com/FastLED/FastLED) — 强大的 LED 驱动库
- [ArduinoJson](https://arduinojson.org/) — 轻量级 JSON 解析
- [PlatformIO](https://platformio.org/) — 嵌入式开发生态

---

<p align="center">
  <sub>Made with ❤️ and ESP32</sub>
</p>
