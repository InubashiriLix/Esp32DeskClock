# EspDeskWeatherClock

- A Arduino driven clock that can display the datetime, waather and envorinement in your surrounding
- require nodeMCU ESP32
- oled

## INSTALLATION & CONFIG

- writing
# Esp32DeskClock

flowchart TD
  %% 定义泳道
  subgraph Setup["🛠️ Setup"]
    A([Start]) --> B[初始化外设 & OLED]
    B --> C[WiFi 连接]
    C --> D[创建 FreeRTOS 任务]
  end

  subgraph Sensors["🌡️ Sensor Tasks"]
    direction TB
    DHT[dhtUpdateTask] -- update → EnvData
    BMP[bmp280UpdateTask] -- update → EnvData
  end

  subgraph Network["🌐 Network Tasks"]
    direction TB
    NTP[updateDateTimeItqTask] -- time → TimeData
    HTTP[updateWeatherTask] -- weather → WeatherData
  end

  subgraph Control["⚙️ Control Tasks"]
    direction TB
    MODE[modeChangeTask] -- mode → DisplayMode
    ALARM[ClockMonitorTask] -- alarm → AlarmEvent
    TOMATO[tomatoTimerTask] -- timer → TomatoEvent
  end

  subgraph Display["🖥️ Display Tasks"]
    direction TB
    TIME[displayTimeTask] -- uses TimeData, DisplayMode → OLED
    WDISP[updateWeatherDisplayTask] -- uses WeatherData, DisplayMode → OLED
    ENVDISP[envDisplayTask] -- uses EnvData, DisplayMode → OLED
    TOMADISP[tomatoDisplayTask] -- uses TomatoEvent, DisplayMode → OLED
    CFGDISP[dispalyConfigPageTask] -- uses DisplayMode → OLED
    ALMDISP[ClockTimeUpItrDisplayTask] -- uses AlarmEvent → OLED
  end

  subgraph Action["🔔 Output Tasks"]
    direction TB
    BUZZ[runBuzzerTask] -- on TomatoEvent or AlarmEvent → Buzzer
    STEP[StepperMotorTask] -- (你如果有单独步进任务就放这)
  end

  %% 数据 / 事件 流
  D --> DHT
  D --> BMP
  D --> NTP
  D --> HTTP
  D --> MODE
  D --> ALARM
  D --> TOMATO
  D --> TIME
  D --> WDISP
  D --> ENVDISP
  D --> TOMADISP
  D --> CFGDISP
  D --> ALMDISP
  D --> BUZZ

  EnvData --> ENVDISP
  TimeData --> TIME
  WeatherData --> WDISP
  DisplayMode --> TIME & WDISP & ENVDISP & TOMADISP & CFGDISP
  AlarmEvent --> ALMDISP & BUZZ
  TomatoEvent --> TOMADISP & BUZZ
