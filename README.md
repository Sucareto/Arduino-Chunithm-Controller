# Arduino-Chunithm-Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。

#### 原理：  
使用 mpr121 的 12 个io 口分别连接 12 块铝箔（或其他导电物体），arduino 用 i2c 和 mpr121 通信获取触摸状态。    

#### 文件结构：  
`ChuniController.ino` 最初版，实现按键功能；  
`ChuniController-EEPROM.ino` 添加了 EEPROM 存储键值功能；  
`ChuniController-EEPROM-Lighting.ino` 添加了驱动 WS2812B 灯带功能；  
`ShowLED.h` 灯效代码，在 ChuniController-EEPROM-Lighting.ino 引入。  

#### 可能需要修改的地方：  
Adafruit_MPR121.h：  
```
#define MPR121_TOUCH_THRESHOLD_DEFAULT 12 //按下敏感度
#define MPR121_RELEASE_THRESHOLD_DEFAULT 6 //松开敏感度
```  
Adafruit_MPR121.cpp：  
```
writeRegister(MPR121_CONFIG1, 0x20);//电流
writeRegister(MPR121_CONFIG2, 0x02);//延迟
```  

#### 资料：  
[pro micro 资料](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro)    

#### 引用库：  
[mpr121操作 Adafruit_MPR121.h](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project.h](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED.h](https://github.com/FastLED/FastLED)
