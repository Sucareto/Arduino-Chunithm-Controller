# Arduino-Chunithm-Controller
使用 Arduino + mpr121 制作的 Chunithm 控制器。

#### 原理：  
使用 mpr121 的 12 个io 口分别连接 12 块铝箔（或其他导电物体），arduino 用 i2c 和 mpr121 通信获取触摸状态。    

#### 可能需要修改的地方：  
Adafruit_MPR121.h 中的：  
```
#define MPR121_TOUCH_THRESHOLD_DEFAULT 12
#define MPR121_RELEASE_THRESHOLD_DEFAULT 6
```
用于修改触摸敏感度。  

#### 资料：  
[pro micro 资料](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro)    

#### 引用库：  
[mpr121操作 Adafruit_MPR121.h](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project.h](https://github.com/NicoHood/HID)  

