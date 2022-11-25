# Arduino Chunithm Controller
使用 Arduino + mpr121 + WS2812B 制作的 Chunithm 控制器。包含键盘和串口两个版本。  
**该项目的开发已完成，这是最终版本。**  
有包含 PCB 和 亚克力的[设计图](设计图)，[PCB 设计文件](设计图/PCB.json)使用[立创EDA](https://lceda.cn/)生成，程序基于此 PCB 开发测试。  
**本项目的 PCB 和外壳是非常过时的设计，请勿直接使用。社区已有更加成熟的方案。**

### 版本区别：
#### [ChuniKeyController](ChuniKeyController/ChuniKeyController.ino)
- 键盘按键方案，只支持 16k 触摸 + 6k AIR，需要使用定制的 chuniio
- 只对 SparkFun Pro Micro（ATmega32U4）进行了适配和测试  
- 16k 灯光通过串口从[已修改的 chuniio](ChuniKeyController/chuniio.dll) 读取  

#### [ChuniSerialController](ChuniSerialController/ChuniSerialController.ino)
串口代码参考了 [ilufang](https://github.com/ilufang) 和 segatools 的实现  
- 原生串口方案，支持 SDBT、SDHD、SDGS，通过串口 COM1 直接和主程序通信传输触摸数值和灯光数据
- 对以下开发板进行了适配和测试：
  - SparkFun Pro Micro（ATmega32U4），需要发送 DTR/RTS
  - SparkFun SAMD21 Dev Breakout（ATSAMD21G18）
  - NodeMCU 1.0（ESP-12E + CP2102 & CH340），无按钮和 AIR
- AIR 使用 HID，需要[新版已修改的 chuniio](ChuniSerialController/chuniio)
- 可完整支持 32k 触摸和 16 按键 + 15 分割的灯光，需要修改代码

### 参考 & 引用库：  
[OpeNITHM](https://github.com/jmontineri/OpeNITHM)  
[mpr121操作 Adafruit_MPR121](https://github.com/adafruit/Adafruit_MPR121)  
[NKRO键盘 HID-Project](https://github.com/NicoHood/HID)  
[驱动WS2812B FastLED](https://github.com/FastLED/FastLED)  
