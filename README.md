# Arduino-Chunithm-Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。

#### 原理：  
使用 mpr121 的 io 口分别连接铝箔（或其他导电物体），arduino 用 i2c 和 mpr121 通信获取触摸状态。    

#### 版本分别：
##### [ChuniKeyController](ChuniKeyController/ChuniKeyController.ino)
只支持 SparkFun Pro Micro（ATmega32U4）  
触摸和 AIR 使用 HID 按键方案，灯光通过串口从[已修改的 chuniio](ChuniKeyController/chuniio.dll) 读取。  
不再更新。
##### [ChuniSerialController](ChuniSerialController/ChuniSerialController.ino)
支持以下开发板：
- SparkFun Pro Micro（ATmega32U4），需要发送 DTR/RTS  
- SparkFun SAMD21 Dev Breakout（ATSAMD21G18）  
- NodeMCU 1.0（ESP-12E + CP2102 & CH340），无按钮和AIR  

通过串口 COM1 直接和主程序通信传输触摸数值和灯光数据，AIR 使用 HID 和 [新版已修改的 chuniio](ChuniSerialController/chuniio)。  
串口部分代码参考了 [ilufang](https://github.com/ilufang) 的实现。  
触摸部分未完成。  

#### 参考 & 引用：  
[OpeNITHM](https://github.com/jmontineri/OpeNITHM)  
[mpr121操作 Adafruit_MPR121](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED](https://github.com/FastLED/FastLED)  
