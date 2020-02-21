# Arduino-Chunithm-Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。
部分代码参考了 [OpeNITHM](https://github.com/jmontineri/OpeNITHM)。

#### 原理：  
使用 mpr121 的 io 口分别连接铝箔（或其他导电物体），arduino 用 i2c 和 mpr121 通信获取触摸状态。  

#### 文件结构：  
多个文件是不同时期的代码，请始终参考最后更新的版本！
`ChuniController.ino` 最初版，实现按键功能；  
`ChuniController-EEPROM.ino` 添加了 EEPROM 存储键值功能；  
`ChuniController-EEPROM-Lighting.ino` 添加了驱动 WS2812B 灯带功能；  
`ChuniController-full_version` 全功能版本，添加了串口配置键值、灯效的功能；  
`ShowLED.h` 统一灯效代码。  

`Example` 一个简单的示例，包括 PCB 设计图（使用 [立创EDA](https://lceda.cn/) 制作）、外壳设计图，对应的代码。


#### 资料：  
[pro micro 资料](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro)    

#### 引用库：  
[mpr121操作 Adafruit_MPR121.h](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project.h](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED.h](https://github.com/FastLED/FastLED)
