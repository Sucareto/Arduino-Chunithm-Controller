# Arduino-Chunithm-Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。  
有包含 PCB 和 亚克力的[设计图](设计图)，[PCB 设计文件](设计图/PCB.json)使用[立创EDA](https://lceda.cn/)生成，程序基于此 PCB 开发测试。 

#### 版本分别：
##### [ChuniKeyController](ChuniKeyController/ChuniKeyController.ino)
只支持 SparkFun Pro Micro（ATmega32U4）  
只支持 16k 触摸 + 6k AIR 按键方案 
16k 灯光通过串口从[已修改的 chuniio](ChuniKeyController/chuniio.dll) 读取  
不再更新。

##### [ChuniSerialController](ChuniSerialController/ChuniSerialController.ino)
支持以下开发板（通过预编译标识符区分）：
- SparkFun Pro Micro（ATmega32U4），需要发送 DTR/RTS  
- SparkFun SAMD21 Dev Breakout（ATSAMD21G18）  
- NodeMCU 1.0（ESP-12E + CP2102 & CH340），无按钮和 AIR  

通过串口 COM1 直接和主程序通信传输触摸数值和灯光数据，AIR 使用 HID，需要[新版已修改的 chuniio](ChuniSerialController/chuniio)  
串口部分代码参考了 [ilufang](https://github.com/ilufang) 的实现  
可支持 32k 触摸和 16 按键 + 15 分割的灯光（默认是 16k 的按键和灯光，可修改代码实现）  

#### 参考 & 引用：  
[OpeNITHM](https://github.com/jmontineri/OpeNITHM)  
[mpr121操作 Adafruit_MPR121](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED](https://github.com/FastLED/FastLED)  
