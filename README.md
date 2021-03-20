# Arduino-Chunithm-Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。
是用了Sucareto/Arduino-Chunithm-Controller（https://github.com/Sucareto/Arduino-Chunithm-Controller）的固件

#### 原理：  
使用 mpr121 的 io 口分别连接铝箔（或其他导电物体），arduino 用 i2c 和 mpr121 通信获取触摸状态。  

#### 资料：  
[pro micro 资料](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro)    

#### 引用库：  
[mpr121操作 Adafruit_MPR121.h](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project.h](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED.h](https://github.com/FastLED/FastLED)

#### 更改：
缩小了PCB体积将键盘拆分成四块一样的10x10cmPCB，可以使用嘉立创的5块钱打样和捷配的免费打样
