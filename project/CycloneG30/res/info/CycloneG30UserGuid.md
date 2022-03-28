<font size="5" Cyclone G30 用戶使用手冊</font>

<div style="text-align:right;border:1px solid red">
version v1.7.8<br>
zhangxianda 2022-02-16<br>
</div>


| 序号 | 版本号 | 更新内容          | 更改人 | 更改时间   |
| ---- | ------ | ----------------- | ------ | ---------- |
| 0    | v1.0.6 | CycloneG30 V1.0.6 | 张献达 | 2021-07-07 |
| 1    | v1.0.7 | CycloneG30 V1.0.7 | 张献达 | 2021-07-08 |
| 2    | v1.0.9 | CycloneG30 V1.0.9 | 张献达 | 2021-08-02 |
| 3    | v1.1.0 | CycloneG30 V1.1.0 | 张献达 | 2021-08-18 |
| 4    | v1.1.2 | CycloneG30 V1.1.2 | 张献达 | 2021-08-26 |
| 5    | v1.1.5 | CycloneG30 V1.1.5 | 张献达 | 2021-09-11 |
| 6    | v1.7.8 | CycloneG30 V1.7.8 | 张献达 | 2021-02-16 |


# 目录


[TOC]





## 1.设备连接

### 1.1硬件连接:

用USB设备线缆连接主控板USB口和PC USB3.0设备口，在计算机，设备管理窗口查看是否显示正常USB设备，如x下图正常显示FTDI FT601 USB 3.0Bridge Device，则表示设备正常，如其他请考虑是否设备驱动未正常安装

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627223651288.png" alt="image-20210627223651288" style="zoom: 80%;" />

### 1.2 USB驱动安装

如若未正常安装设备驱动，显示如图![image-20210707101912256](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707101912256.png)

请前网FTD官网下载D3XX驱动包进行安装http://www.ftdichip.cn/Drivers/D3XX.htm

请右键选择->更新驱动程序->浏览我的计算机以查找我的驱动程序文件->选择驱动包位置->下一步->完成安装

![image-20210707102404521](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707102404521.png)!![image-20210707102455208](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707102455208.png)

### 1.3软件连接：

命令控制：工具栏**搜索连接USB设备**->**连接设备**即可，目前**命令控制** 通过USB设备下发给FPGA。

数据流收发：工具栏**搜索网口IP**->**开始接收数据**即可，LVDS数据流通过网口上传至PC；

![image-20220214161506983](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214161506983.png)

## 2.界面UI介绍

主界面如下图，包括菜单栏，工具栏，界面选择列表，界面功能以及状态栏

**界面包括**

①配置界面Configuration：主要的板级初始化设置，加载配置设置，AFE模拟设置，其他设置

②寄存器映射表Register：有关寄存器的设置和读取

③数据界面Data analysis:  LVDS的数据抓取和相应的测序功能

④日志界面 LogPanel ： 程序诊断日志

![image-20220214162126227](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214162126227.png)

## 3.菜单栏介绍

### 3.1 文件菜单

#### 3.1.1 开启OpenGl

![image-20220214163354646](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214163354646.png)	

File->Enable OpenGL 开启硬件加速，再次点击关闭该功能；

#### 3.1.2 退出

![image-20220214163529578](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214163529578.png)	

File->Exit 软件退出；

### 3.2 Tool菜单
<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214163839156.png" alt="image-20220214163839156"  style="float: left;"/>

Tool 工具栏包括:Console;CRC校验;使能数据流打印，温控，UDP测试模式，模拟数据模式，

#### 3.2.1 Console调试窗口

调试窗口，便于显示打印采集数据，问题定位，可在**菜单栏Tool->Console 选择是否显示调试窗口**。

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210802164757855.png" alt="image-20210802164757855" style="zoom:80%; float:left;" />

#### 	3.2.2 LVDS数据CRC Check

CRC Check 勾选是否使能： 如果使能，则原始数据则会通过CRC8校验法则进行校验，通过校验则进一步解析，不通过则可选择打印；具体校验规则详见ASIC芯片规格书；

##### 3.2.2.1通过CRC校验数据

可通过菜单栏Tool->CRC Check->**Pass CRC correct data**，则只通过CRC校验通过的数据，再次点击则取消，允许全部数据通过，**默认通过所有数据**。

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214170206573.png" alt="image-20220214170206573" style= "float:left;" />

##### 3.2.2.2 打印校验失败的记录

可通过菜单栏Tool->CRC Check->**Enable failed CRC print**，点击打印记录，再次点击取消打印

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210818163728187.png" alt="image-20210818163728187" style="zoom:50%;float:left;"/>

#### 3.2.3 Eanble all stream print

Eanble all stream print勾选是否使能： 使能->打印所有原始数据流	不使能->不打印

#### 3.2.4 Temperature

温控模块为外部单独程序支持，在开启温控进程时，每秒更新温度值到当前主程序界面；<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216112413737.png" alt="image-20220216112413737" style="zoom:80%;" />

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216112450128.png" alt="image-20220216112450128" style= "zoom:100%; float:left;" />



- 状态解释
  - Error 外部温控进程未开启
  - Off    进程开启，温控设备未连接；

#### 3.2.5 Udp Test Mode

Udp测试模块，勾选是否使能；会发送测试命令给FPGA，FPGA会返回一系列连续自增的数据流给PC，此功能用来验证数据流是否正常和连续；调试收发数据性能；打印TestMode 下的原始数据，为验证Udp所有数据，所以不画曲线，全打印Hex值。需配合FPGA测试固件使用；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210826170842060.png" alt="image-20210826170842060" style="zoom: 80%;float:left;" />

#### 3.2.6 Simulate Data Mode

模拟数据流功能，勾选是否使能；勾选后数据流将会停止从ASIC中读取，而是由SavePanel中加载数据文件作为输入源替代，以适应某些开发调试或者测试环境使用；

Tool->Simulate Data Mode->SavePanel 选择数据流文件夹-> 开始收集数据

![image-20220214171600632](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214171600632.png)

### 3.3 Help帮助菜单

#### 3.3.1 用户操作手册

Help->User Guide，点击即可出现用户手册PDF

#### 3.3.2 软件信息

Help->About，点击即可出现关于此软件的版本信息，升级日志；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214173553804.png" alt="image-20220214173553804" style="float: left;" />

## 4 . 配置界面Configuration

配置模块包括初始化配置，ASIC-AFE配置，自定义配置，其他配置；

#### 4.1初始化配置

SysConfig->Initial 点击即可发送初始化指令至ASIC板，同时读取ASIC AFE中的配置，具体发送内容在软件中固定；

#### 4.2 自定义配置

##### 4.2.1编辑自定义配置文件

选择软件运行目录下 etc/Conf/LoadReg.csv，用EXCEL打开编辑， destination 目前支持ASIC，ID代号为1，自行增加修改ASIC addr和Value 列值，第四列是每条命令之间的**间隔延时(单位ms)，不延时填写0，请勿空着，空着则格式无效**，填写合格范围值保存即可。

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707100132883.png" alt="image-20210707100132883" style= "float:left;" />

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627234156246.png" alt="image-20210627234156246" style= "float:left;"   />

##### 4.2.2 加载自定义配置文件

为方便快速调试，添加刚刚编辑的CSV表进行快速指令下发，点击LoadReg按钮->选择etc/Conf/LoadReg.csv文件->打开即发送指令。

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627233833371.png" alt="image-20210627233833371" style="zoom:80%;" /><img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627234002507.png" alt="image-20210627234002507" style="zoom:80%;" />

#### 4.3 AFE模拟转换配置

![image-20210911170825654](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210911170825654.png)

**ADC sample rate**: 请选择采样率：5K/10K/20K/3.75K,默认是5K  
**Raw or ADC Mode** :选择RawData模式(范围0-65536) ,还是ADC out模式(-32768 - 32768) 对应ASIC addr A0 bit[23] SARADC_RAW;
**Convert to current:** 选择是否转换为pA电流
**Input current range:** 选择电流对应关系进行转换，具体参考下表格，详见LVDSTransform.csv

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214174738446.png" alt="image-20220214174738446"  style= "float:left;" />

写入： 配置相应转换选项-> Write Configuration；

读取:	点击即可Read Configuration

#### 4.4 其他配置

##### 4.4.1 ASIC高低功耗配置

写入：Power consumption: **Low**->低功耗 **High**->高功耗，选择相应功耗选项-> Write Configuration；

读取:	点击即可Read Configuration

功耗标识：低功耗 ![image-20220214175703649](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214175703649.png) 高功耗![image-20220214175724893](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214175724893.png)

## 5.寄存器界面Register

左侧列表第二项->寄存器操作界面,主要为ASIC寄存器进行查看和配置

![image-20220214175930658](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220214175930658.png)

左表为所有的寄存器，右表为每个寄存器所对应的段域。

### 5.1寄存器表加载功能

1. 为方便编辑，寄存器表是通过**加载XML表进行解析的**，点击Reload Register table 可重新加载寄存器表格

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707101229201.png" alt="image-20210707101229201" style="float: left;" />

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627231300314.png" alt="image-20210627231300314" style="zoom: 100%; float: left;" />

   全部恢复默认值，按钮点击**All reset default**

### 5.2寄存器搜索功能：

左边下方，选择搜索的类型，可以填入寄存器对应的**地址/寄存器名称/Id**, 点击**Search**，便可进行筛选符合的结果，如果需要重新显示全部寄存器，点击**Display Whole Table**即可

![image-20210627225226341](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210627225226341.png)

### 5.3 寄存器表读写操作

#### 5.3.1修改寄存器值

1. 左表中选择相应的寄存器，在**Value**列双击即可修改其值；

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708171151547.png" alt="image-20210708171151547" style= "zoom:100%; float:left;"  />

2. 值**修改后**颜色为**黄色**，表示值曾被修改过；点击Submit Change提交修改，点解Revert Change撤销修改，颜色恢复绿色

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708171428241.png" alt="image-20210708171428241" style= "zoom:100%; float:left;"   />

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708171434678.png" alt="image-20210708171434678" style= "zoom:100%; float:left;"   />

3. 修改的**Value值**将会**自动更新到**右表所有的段域，又或者点击**Sync** 按键强制刷新同样达到更新右表的效果；

4. 点击**All reset default** 按钮恢复所有默认值到寄存器表。

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708171613688.png" alt="image-20210708171613688" style="float: left;" />

#### 5.3.2 寄存器值读写功能

1. 支持**一键全读/全写**，点击**All read /All write** 即可。

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708171924408.png" alt="image-20210708171924408" style= "zoom:100%; float:left;"   />

2. 支持**选择性读/写/写读**，**右键选择菜单Read-Only/ Write-Only /Write-Read**

3. **读失败标识为红色**，重新读取成功取消标红。

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210709111855502.png" alt="image-20210709111855502" style= "zoom:100%; float:left;"   />

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708172239198.png" alt="image-20210708172239198" style= "zoom:100%; float:left;"   />



### 5.3 段域表读写操作

#### 5.3.1修改域表位值

1. 右表相应行中**双击Value**值进行更改值，其大小限制为取决于Bits 列的限制；

   ![image-20210708172542795](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708172542795.png)

2. 值**修改后**颜色为**黄色**，表示值曾被修改过；点击Submit Change提交修改，点解Revert Change撤销修改，颜色恢复绿色

   

   ![image-20210708172615802](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708172615802.png)

   ![image-20210707100736154](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707100736154.png)

3. 按键说明：

   **sync**：将左表中寄存器的值按照右表位的分布进行全段域更新；

   **Reset Default**：恢复所有段域位的Value值为默认值

   **Submit Change**：确认更改，若有黄色则消失

   **Revert Change**:撤销更改，恢复上次的值，若有黄色则消失

   

#### 5.3.2 段域读写功能

按键说明：

**Read-Only**: 只读该寄存器值

**Write-Only**:只写该寄存器值, 在**写之前需提交是否修改更改的域**；

**Write-Read**:写完再读该寄存器值

![image-20210708173253701](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210708173253701.png)

### 5.4 寄存器读写功能

1. 可在左表对相应寄存器进行**读写**，**右键菜单栏选择S**，然后选择：

   **Send: 只写不读**
   **Read:只读不写**
   **WriteRead:即写即读(目前不支持)**

   ![image-20210707102751251](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707102751251.png)

2. 支持全读/全写，通过按钮点击**All read  , All write**

![image-20210707101027296](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707101027296.png)

②或者**直接点击右表中的按钮进行读写当前寄存器的地址和值**

支持单个寄存器独立读/写/读写，通过点击按钮**Read-Only , Write-Only, Single Write Read**

![image-20210707103403883](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210707103403883.png)

### 5.5寄存器命令调试

Register->Comunicate->输入Hex命令(eg: 55 AA FF 02 00 00 00 02 FA FA) -> Write-> Read

![image-20220215101805356](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215101805356.png)

## 6.Data analysis数据分析

Data analysis数据分析主要是抓取LVDS数据进行进一步分析以及相应的控制面板；

功能面板包括：

- Save保存数据面板
- Mux Sensor选择面板
- Volt 电压控制面板
- Pore 孔保护面板
- MuxScan 多路扫描面板
- Simulate 电化学活化面板
- AutoScan 自动扫描面板

图像分析面板：

- Curves 电流曲线面板

- Mapping 状态映射面板

- Chart 图标面板

  <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215104701356.png" alt="image-20220215104701356" style="zoom:80%;" />

### 6.1 Save 保存数据面板

功能用途：保存原始数据/配置实时分析模块/加载模拟数据

![image-20220215105055607](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215105055607.png)

#### 6.1.1 保存原始数据

ChooseFloder-> Start->开始保存数据，Stop停止保存数据；

#### 6.1.2 保存选项Json设置

##### 6.1.2.1 Config选项

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215105520904.png" alt="image-20220215105520904" style="zoom:80%; float:left;" />

- “SequencerID” 测序机器编号
- “StartTime”、“ EndTime “、” FolderPath”为自动 生成 ；
- 可选择是否启用实时测序分析
-  联网后的配置信息从服务器下载更新；

##### 6.1.2.2 加载现有的参数配置

“Load…”可以加载现有的 json参数配置文件

##### 6.1.2.3 保存配置参数

“Save “保存当前的配置参数到当前系统运行目录的 ./etc/ Data/ata/ “子目录下的 json文件

#### 6.1.3 保存设置

##### 6.1.3.1 保存文件大小设置

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215110340091.png" alt="image-20220215110340091" style="zoom:80%;float:left;" />

默认大小值为128M，可以根据需要设置每个文件保存的大小。

##### 6.1.3.2 选择数据保存目录

“ChooseFolder“可选择数据保存目录，不选择则保存到默认路径；按钮左边文本框显示为保存路径。

##### 6.1.3.3 打开数据保存目录

“OpenFolder“可打开数据保存的目录；

 3 自动停止数据保存 

#### 6.1.4 设置倒计时时间

编辑框内可以设置倒计时的时间，单位为分钟，范围为1min到1439min；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215110833638.png" alt="image-20220215110833638" style="zoom:80%;float:left;" />

##### 6.1.4.1显示倒计时

LED框可显示倒计时时间

##### 6.1.4.2重置倒计时时间

“ResetAuto“可重置倒计时时间；

##### 6.1.4.3自动停止并保存数据

“StartAuto“点击开始启用自动停止收取数据和保存数据；

##### 6.1.4.4 自动停止数据保存

  “StartAuto“点击开始启用自动停止收取数据和保存数据；                             

##### 6.1.4.5 其他说明

启用自动停止并保存的同时，系统状态栏页会同步显示倒计时时间；

#### 6.1.5 数据翻译转换

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215111056812.png" alt="image-20220215111056812" style= "zoom:100%; float:left;"   />

对话框大小可以改变。

##### 6.1.5.1 将二进制数据转换为文本数据

点击“Binary Translate to Text”按钮实现转换。

##### 6.1.5.2 打开文本数据文件

点击“Open Text File”按钮实现。

#### 6.1.6 数据波形图回放

- 对话框大小可以改变；
- 可显示加载的数据文件路径；
- <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215111418313.png" alt="image-20220215111418313" style= "zoom:100%; float:left;"   />

##### 6.1.6.1 加载数据文件

点击“Load Data”按钮加载数据，加载好如上图所示。此时可用鼠标操作：

- 鼠标左键框选放大；
- 鼠标中键滚动缩放；
- 鼠标右键平移拖拽；

##### 6.1.6.2 重置数据

点击“Reset Data”按钮重置数据为最初加载的样子。

### 6.2 Mux Sensor选择面板

功能包括：所有通道/部分通道Sensor状态切换以及 定时做所有通道做degating；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215111923358.png" alt="image-20220215111923358" style= "zoom:100%; float:left;"   />

#### 6.2.1 所有通道Sensor切换

所有通道切换状态选项： A/B/C/D/Off/ Internal sources/Mux

- A: SensorA nomal，所有通道切换至SensorA
- B: SensorB nomal，所有通道切换至SensorB
- C: SensorC nomal，所有通道切换至SensorC
- D: SensorD nomal，所有通道切换至SensorD
- Off: Off ，所有通道切换至Off
- Internal sources: internal current source，所有通道切换至内部电流源
- Mux : 切换至混合模式，所有通道根据Valid表进行切换对应sensor

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215112506060.png" alt="image-20220215112506060" style= "zoom:100%; float:left;"   />

#### 6.2.2 自选通道Sensor切换

输入Start开始通道号->End 结束通道号->选择State->applay

State: Normal/Unblock/InternalSources/Switch off

- Normal，所选相应通道号切换至Normal
- Unblock，所选相应通道号切换至Unblock
- InternalSources，所选相应通道号切换至InternalSources内部电流源
- Switch off，所选相应通道号切换至Off

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215112811225.png" alt="image-20220215112811225" style= "zoom:100%; float:left;"   />

#### 6.2.3 定时degating模块

输入定时时间->Auto unblock使能，再次点击关闭使能

功能描述：当计时时间到，即开启一次将所有通道状态由当前Sensor状态设置为Unblock状态,持续一段时间(Degating设置延时时间ms)后再次恢复原来状态；此目的是为了定时处理堵孔情况；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215113141801.png" alt="image-20220215113141801" style= "zoom:100%; float:left;"   />

### 6.3 Volt电压控制面板

设置DAC0,DAC1的电压与读取

#### 6.3.1 DAC电压的设置和读取

**人为设定**最小最大值： 输入DAC0/DAC1的Min/Max值->Apply

通过**软件读回**设置最小最大值： **Read back**按钮；

![image-20220215134105914](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215134105914.png)

#### 6.3.2 Normal/Unblock电压设置

在Amplitude框输入电压值->Apply设置Normal状态下通道所受电压，在Volt输入电压->Apply可设置Unblock状态下通道所受电压；

![image-20220215134115986](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215134115986.png)

### 6.4 Pore孔保护面板

功能描述：单孔保护模块以及其他保护模块；

#### 6.4.1 孔保护模块

##### 6.4.1.1孔保护模块说明

![image-20220215135225215](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215135225215.png)

**阈值**：Single pore单孔、Multi pore 多孔、Membrane broken 膜破；

**孔保护模块逻辑**： 

1. 每秒获取通道平均值，当通道每秒电流平均值超过Single pore阈值，则开始进入下一步判断，否则判断为Nonepore无孔，结束判断；

2. 步骤1中通道Avg超过Single pore的阈值后，首先判断是否大于Membrane broken，若大于膜破电流则判断为Saturated pore孔状态；否则进入下一步判断；

3. 步骤2中若电流Avg超过Multi pore的阈值，则判断为Multi pore孔状态，否则判断为Single pore孔状态，结束判断；

4. **只要判定为非Nonepore,无论是Single/Multi/Saturated均会将该通道置为Off**(不锁定)；

   （Nonepore/Single pore/Multi pore/Saturate pore 参考Mapping中的Pore面板）

##### 6.4.1.2 孔保护模块开启

点击Start 使能该模块，点击Stop则停止该模块；

##### 6.4.1.3 通道锁定

勾选Porelock只要判定为非Nonepore,无论是Single/Multi/Saturated均会将该通道**置为Off且锁定**；

Lock即为软件逻辑锁定，当锁定该通道后无法再人为切换Sensor状态，除非清楚该通道的lock标识，具体参考Mapping中的Lock面板；

#### 6.4.2 Std过滤

功能描述：每秒获取通道的Std值，Off不满足Min-Max范围内的通道；

![image-20220215140056130](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215140056130.png)

点击Start开启使能该模块，点击Stop则停止该模块；

### 6.5 MuxScan多路扫描模块

功能描述：根据Lab256pro的Muxscan模块功能而来，仿照模块流程，**实际上此模块并未用**；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215140520804.png" alt="image-20220215140520804" style= "zoom:100%; float:left;"   />

#### 6.5.1 模块说明

- Timer Period: 每个周期时间
- Cycle ：总共周期数
- Next: 继续此周期
- Start:开启/关闭该模块

总体流程：设置倒计时->计时到->**周期模块流程**->开始计时->Cycle -1 > 0?->继续下一周期

**周期模块流程**： 停止保存数据，停止degting->Pore Qc->Seq->开启degating->开始保存数据；

#### 6.5.2 Pore Qc

Qc逻辑： 设置Backward 反向电压 Volt,持续Duration一段时间后再设置PoreQc Volt再持续Duration一段时间后，开始以每秒的平均值/标准差Std来筛选，若同时满足Current avg和std Min-Max范围，则认为符合筛选条件则标识为Seq且不予操作，不符合则置为off;

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215141347606.png" alt="image-20220215141347606" style= "zoom:100%; float:left;"   />

#### 6.5.3 Seq

软件逻辑：设置Zerovolt 电压为0，持续duration一段时间，再施加Seq 电压为volt,持续duration一段时间，若Enable勾选则开启degating模块，若不勾选则不开启degating模块；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215141618119.png" alt="image-20220215141618119" style= "zoom:100%; float:left;"   />

### 6.6 Simulate电化学活化模块

功能描述：仿真三角波电压，对通道进行依次施加，从而达到活化膜特性；

#### 6.6.1 Simulation

##### 6.6.1.1 参数说明

![image-20220215144002604](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215144002604.png)

- CurVolt  :显示当前电压；
- Volt Start: 仿三角波起始电压；
- Volt End：仿三角波终止电压；
- Volt Step: 仿三角波电压步长；
- Time Step: 仿三角波时间步长；
- Ramp pattern: Rise/Fall/Mirror模式,上升/下降/对称
- Cycle：总周期数
- Remain:剩余周期数
- Default：默认参数

##### 6.6.1.1 开启模块

点击Start Simulation开启仿真模块，点击Stop停止仿真模块

##### 6.6.1.2 软件逻辑

1. 设置好参数后，首先所有通道设置为SensorA

2. 设置为VoltStart电压，持续Time step，后再设置为VoltStart+VoltStep，持续Time step时间...直到达到VoltEnd;

3. 依次切换所有通道为SensorB、SensorC、SensorD,重复2步骤；

   (当前电压显示再CurVolt中，Remian显示已处理的Sensor 模块过程中存在进度条，完成则显示100%)

   Rise模式仿三角波电压类似如下图：

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215145435074.png" alt="image-20220215145435074" style= "zoom:100%; float:left;"   />

   Mirror模式仿三角波电压类似如下图：

   <img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215145546718.png" alt="image-20220215145546718" style= "zoom:100%; float:left;"   />

#### 6.6.2 Polymer simulate

![image-20220215145359878](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215145359878.png)

##### 6.6.2.1 参数说明

- Pore Limit： 孔保护电流阈值
- Pore protect ：孔保护时Sensor状体
- Volt Start: 起始电压
- Volt End ：终止电压
- Volt Step：步进电压
- Time Step：步进时长
- CurVolt: 当前电压
- Default：默认参数

##### 6.6.2.2 开启模块

点击Start Simulation开启仿真模块，点击Stop停止仿真模块

##### 6.6.2.3 软件逻辑

1. 设置好参数，设置单孔保护阈值，达到单孔阈值后设置为off，开启单孔保护模块；
2. 设置为VoltStart电压，持续Time step，后再设置为VoltStart+VoltStep，持续Time step时间...直到达到VoltEnd;
3. 重复2步骤直到手动停止

### 6.7 AutoScan自动扫描模块

<img src="C:\Users\ZHANGX~1\AppData\Local\Temp\Image.png" alt="Image" style= "zoom:100%; float:left;"   />

<img src="C:\Users\zhangxianda\AppData\Local\Temp\Image2.png" alt="Image2" style= "zoom:100%; float:left;"   />

#### 6.7.1 功能用途

自动扫描测序，过滤电流溢出通道，筛选出ABCD组有效测序Sensor，并在筛选的结果上选择合适的Sensor进行测序，开启Degating：

#### 6.7.2 功能使用

设置好上图的各个模块参数，点击最右边Start按钮开始启动；Timer Period和Cycle对应扫描间隔时间和周期数，达到设置周期数自动停止模块功能；Start按钮会显示剩余周期数；

#### 6.7.3 功能模块流程

**停止保存数据和Degating模块->Scan Group->ValidQc->PoreFilt->Degating开启->开启新保存数据**

#### 6.7.4 Scan Group

##### 6.7.4.1参数解释

- Dc volt:  设置直流电压
- Ac volt:  设置三角波电压
- Allow Times:  直流或三角波设置下扫描次数

##### 6.7.4.2目的

轮流扫描SensorABCD组，Lock并Off掉有问题的通道

##### 6.7.4.3内部操作逻辑      

先设置所有通道为A组，设置直流0.18V，调用Mapping中Lock面板Scan按钮，扫描10次，再设置为三角波0.05V，调用Mapping中Lock面板Scan按钮，扫描10次，扫描过程中会使用OVF面板中记录哪些channel是否有溢出，溢出则关闭通道状态，并增加已锁定标志； 再设置所有通道为B组，重复上述步骤；直到4组Sensor扫描完毕；

#### 6.7.5 Valid Qc

##### 6.7.5.1参数解释

- Volt:      设置直流电压
- Wait:      等待时间/s后以Current和Std为标准进行1s的筛选
- Current:   平均电流筛选范围设置
- Std:       标准差筛选范围设置

##### 6.7.5.2目的

筛选哪些Sensor为有效，并作Mux多路混合切换

##### 6.7.5.3内部操作逻辑   

1. 设置所有通道为A组Sensor,设置直流电压后，等待Wait时间后，获取所有通道的平均电流和Std值，符合两者范围的通道识别为Valid有效通道，否则识别为InValid，结果更新在Mapping中的Valid面板; 再设置所有通道为BSensor组，重复上述步骤，直至ABCD组Sensor筛选完毕
2. 根据Mapping中Valid的映射表，作所有通道的Sensor切换，如通道1的ABCD中，A有效而BCD无效，则优先使用A作为当前状态进行测序，其他通道同理；

#### 6.7.6 PoreFilt

##### 6.7.6.1 参数解释

- DegatVolt:  正反向电压的绝对值
- Delay:       作正反向电压持续的时间
- Times:       正反向电压施加的次数
- PoreProtect  单孔保护的电流阈值->关闭gating的多孔

##### 6.7.6.2 目的

未知，详细情况问陈玮

##### 6.7.6.3 内部操作逻辑

设置正反向电压的绝对值，对所有通道施加正电压持续Delay时间，然后施加反向电压持续Delay时间，如此重复Times次后开启单孔保护功能，设置单孔保护阈值为PoreProtect电流；

#### 6.7.7 Degating

##### 6.7.7.1 参数解释

将Degating面板参数放置在此，参数与Lab256无区别

##### 6.7.7.2 目的

开启Degating功能

##### 6.7.7.3 内部操作逻辑

将此参数设置同步到Degating模块后开启Degating功能

### 6.8 电流曲线模块

采集LVDS数据，并将所有通道的电流曲线描绘至面板上，最大支持1024通道；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215152254635.png" alt="image-20220215152254635" style= "zoom:100%; float:left;"   />

#### 6.8.1 设置XY轴坐标

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215152317489.png" alt="image-20220215152317489" style= "zoom:100%; float:left;"   />

- Ymin : Y轴最小值
- Ymax: Y轴最大值
- Times:X轴显示范围，单位为秒
- Apply： 设置按钮

#### 6.8.2 设置可视化通道

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215152432864.png" alt="image-20220215152432864" style= "zoom:100%; float:left;"   />

- Start: 起始通道号
- End:终止通道号
- Apply： 可视化通道设置

### 6.9 Curves configuration

对每一通道的可视化，颜色，平均值，标准差Std, Valid标识进行查看和设置

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215152750649.png" alt="image-20220215152750649" style= "zoom:100%; float:left;"   />

#### 6.9.1 曲线颜色及可见性设置

右键菜单选择Channel setting，

- **Visible代表该通道曲线是否可见，勾选则可见，不勾选则不可见，**
- **曲线颜色可通过双击通道对应Color面板进行颜色更换。**

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20210818163403170.png" alt="image-20210818163403170" style= "zoom:100%; float:left;"   />

#### 6.9.2 平均值和标准差查看

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111140195.png" alt="image-20220216111140195" style= "zoom:100%; float:left;"  />

每个通道的平均值Avg和标准差Std可以在第四列Average和第五列Std中查看，每秒更新一次；

#### 6.9.3 通道Sensor组和Valid查看

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111456336.png" alt="image-20220216111456336" style= "zoom:100%; float:left;"  />

每个通道的处于哪个Sensor组别以及在该组别中是否有效可以在第七列Valid中查看；

#### 6.9.4 筛选模块

根据**Type 过滤可选类型，将符合类型筛选条件的通道保留在该列表，每秒更新一次并统计通道数Count**(**切记请勿停止Refresh按钮**)

- Channel	保留所有通道在此列表
- Visible 	只保留可见的通道在此列表
- AvgStd	只保留满足Avg/Std范围的通道在此列表
- Seq	只保留标识为Seq的通道在此列表
- Valid	只保留标识为Valid的通道在此列表

#### 6.9.5 AvgStd筛选模块

将Type过滤类型选为AvgStd即可；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215171206068.png" alt="image-20220215171206068" style= "zoom:100%; float:left;"   />

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215171144183.png" alt="image-20220215171144183" style= "zoom:100%; float:left;"  />

参数说明：

- Avg :	每一秒电流平均值阈值
- Std:	每一秒电流Std标准差阈值
- OpenporeAvg: 开孔电流平均值阈值，取决于OpenporeFilt是否勾选
- OpenporeAvg: 开孔电流Std标准差阈值，取决于OpenporeFilt是否勾选

按钮说明：

- Hide按钮: 按键按下时，当这一秒平均值和标准差同时满足所设置范围则可见，不满足则不可见，对Sensor状态不操作；（Openpore是否纳入筛选条件，让其同时满足，取决于OpenporeFilt是否勾选）
- Shutoff按钮：按键按下时，当这一秒平均值和标准差同时满足所设置范围则不操作，不满足则置为off；（Openpore是否纳入筛选条件，让其同时满足，取决于OpenporeFilt是否勾选）
- Lock按钮：按键按下时，当这一秒平均值和标准差同时满足所设置范围则不操作，不满足则Lock并置为off,(Lock参考Lock面板功能介绍)；（Openpore是否纳入筛选条件，让其同时满足，取决于OpenporeFilt是否勾选）
- Valid 按钮： 按键按下时，当这一秒平均值和标准差同时满足所设置范围则置为Valid，不满足则置为InValid且置为Off；（Openpore是否纳入筛选条件，让其同时满足，取决于OpenporeFilt是否勾选）
- Only valid按钮： 按键按下时，当已标识为Valid的通道设置为Normal，Invalid则置为Off；

### 6.10 Mapping映射图表

Mapping图表主要为一张32*32的表格，映射1024通道的所有的状态；

包括Sensor状态、OVF溢出状态、Cap电容状态、Pore状态、Lock状态、Valid状态、Mux状态

![image-20220215172317719](C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215172317719.png)

#### 6.10.1 通道号显示/隐藏

Display mode -> Content->Pure隐藏通道号

Display mode -> Content->Channel num 显示通道号

#### 6.10.2 功能状态统计

左边为对应功能的状态，右边为该状态Count统计数

#### 6.10.3 功能状态颜色更改

点击对应功能状态左边的方框，即可改变状态颜色

#### 6.10.4 Sensor状态表

Mapping图表映射1024通道的Sensor状态；

##### 6.10.4.1 Sensor状态描述

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215173406003.png" alt="image-20220215173406003" style= "zoom:100%; float:left;"   />

根据ASIC芯片规格，每个通道有16种sensor状态，且状态只能在16种选择一种状态

- Off ：开关断开状态
- A：通道置为NormalA状态；
- B：通道置为NormalB状态；
- C:	通道置为NormalC状态；
- D:	通道置为NormalD状态；
- A unblock:通道置为A unblock状态；
- B unblock:通道置为B unblock状态；
- C unblock:通道置为C unblock状态；
- D unblock:通道置为D unblock状态；
- 其他状态：与测序无关，无需了解

##### 6.10.4.2 Sensor状态更新

点击Refresh 则从下位机更新当前Sensor状态

#### 6.10.5 OVF状态表

Mapping图表映射1024通道的OVF 标识状态；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215174427289.png" alt="image-20220215174427289" style= "zoom:100%; float:left;"   />

##### 6.10.5.1 OVF状态描述

- Normal: 通道无溢出
- Over_flow： 通道标识为溢出

此标识溢出为下位机FPGA从ASIC中读回，为寄存器中的值；

##### 6.10.5.2 OVF状态更新

点击Refreshf按钮 则可获取对应最新的状态

##### 6.10.5.3 关断OVF通道

点击 Shut off按钮，若某通道标识为Over_flow则关断该通道为Off，若无标识则不动作，**Ps：不会点击refresh按钮**

##### 6.10.5.4 Lock OVF通道

点击 Lock按钮,若某通道标识为Over_flow则关断该通道为Off并lock，若无标识则不动作，**Ps：不会点击refresh按钮**

##### 6.10.5.5 清除OVF标识

点击Clear按钮,清除映射表中所有over_flow标识，均值为normal

#### 6.10.6 Cap状态表

Mapping图表映射1024通道的Cap状态；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215174501927.png" alt="image-20220215174501927" style= "zoom:100%; float:left;"  />

##### 6.10.6.1 Cap状态描述

- Cap_level1 电容状态level1	level1阈值范围设置
- Cap_level2 电容状态level2	level2阈值范围设置
- Cap_level3 电容状态level3	level3阈值范围设置
- Cap_level4 电容状态level4	level4阈值范围设置
- Cap_level5 电容状态level5	level5阈值范围设置

##### 6.10.6.2 Cap电容检测

设置好电容阈值范围，点击Start按钮，开始检测电容，根据电容值判断该通道电容处于哪个状态；实时更新，点击stop关闭电容检测模块；

##### 6.10.6.3 Cap电容值显示

Display mode ->选择Cap value;

##### 6.10.6.4 Cap电容保存

点击Save as...按钮,选择保存路径，可将当前电容值保存为txt文档

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215175059541.png" alt="image-20220215175059541" style="zoom: 67%;float:left;" />

##### 6.10.6.5 Cap状态清除

点击Clear ，则将所有通道状态清除Cap_level1

#### 6.10.7 Pore状态表

Mapping图表映射1024通道的OVF 标识状态；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220215175914666.png" alt="image-20220215175914666" style= "zoom:100%; float:left;"   />

##### 6.10.7.1 Pore状态描述

- None pore ： 无孔
- Single pore:	单孔
- Multi pore:	多孔
- Saturated pore:	膜破
- Inactive pore:	死孔
- Slip pore:	滑孔

##### 6.10.7.2 Pore标识组

Pore标识根据Sensor组分为四组，分别为PoreA组，PoreB组，PoreC组，PoreD组

##### 6.10.7.3 Pore状态更新

开启单孔保护模块，即可将判断的结果更新到此表中，**关于单孔保护模块参考6.4**

##### 6.10.7.4 Pore状态统计

点击Count按钮，则可统计各种状态通道数

##### 6.10.7.5 Pore状态清除

点击Clear按钮，则可清除各种状态为Nonepore

#### 6.10.8 Lock状态表

Mapping图表映射1024通道的Lock标识状态；

##### 6.10.8.1 Lock标识解释

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216094534354.png" alt="image-20220216094534354" style= "zoom:100%; float:left;"  />

- Allow:	允许
- Prohibited:	禁止

**Lock标识在ASIC中并不存在该硬件标识，只是软件定义的逻辑标识，可以认为是给软件操作的枷锁；**

- 当通道标识手动操作或者模块功能标识为Prohibited时，则意味着该通道会置为Sensor state 为off ，软件将锁定该通道状态，任何Sensor切换操作都不可以改变Sensor状态，除非取消Prohibited标识，变为Allow标识；

##### 6.10.8.2 Lock标识组

Lock标识根据Sensor组分为四组，分别为LockA组，LockB组，LockC组，LockD组

##### 6.10.8.3 自动扫描Scan

###### 6.10.8.3.1 扫描顺序

扫描顺序：扫描通道分为16组，

第1组的排列通道号为：1，17，33.....1009，共64通道;

第2组的排列通道号为：2，18，34.....1010，共64通道;

第3组的排列通道号为：3，19，35.....1011，共64通道;

............

第16组的排列通道号为：16，32，64.....1024，共64通道;

###### 6.10.8.3.2 软件逻辑

软件内部逻辑：根据扫描顺序将扫描通道分为16组，切换至每一组时，  连续调用三次OVF模块的Refresh，如果某通道连续出现三次OVF中的over_flow，则将该通道置为Lock标识并关断为Off State;扫描完16组为一次time,直至满足times次数；

###### 6.10.8.3.3 一键操作

填写次数times，**点击按钮Scan**,即可自动扫描溢出通道，有问题通道标识为Prohibited并关断为off,无问题通道则标识为Allow;

##### 6.10.8.4 手动修改标识

###### 6.10.8.4.1 手动标识为Allow

在表中点击某通道或者部分通道->点击按钮Allow->即可标识为Allow;

###### 6.10.8.4.1 手动标识为Prohibited

在表中点击某通道或者部分通道->点击按钮Prohibited->即可标识为Prohibited;

##### 6.10.8.5 其他功能

###### 6.10.8.5.1 手动置为Normal

在表中点击某通道或者部分通道->点击按钮Normal->即可修改当前通道SensorState标识为Normal;

###### 6.10.8.5.2 手动置为Unblock

在表中点击某通道或者部分通道->点击按钮Normal->即可修改当前通道SensorState标识为Normal;

##### 6.10.8.6 重新统计标识

点击按钮Count即可重新统计每种状态的通道数；

##### 6.10.8.6 清除标识

点击按钮Clear即可将所有通道标识改为Allow；



#### 6.10.9 Valid状态表

Mapping图表映射1024通道的Valid标识状态；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216100808716.png" alt="image-20220216100808716" style= "zoom:100%; float:left;"   />

##### 6.10.9.1 Valid标识描述

- Invalid:	无效

- Valid:	有效

  Valid标识也是软件的逻辑标识，跟硬件ASIC无关，当在测序过程中，根据筛选标准，如在Curves configuration中根据AvgStd的Min-Max范围进行筛选，满足即可标识为Valid，不满足标识为Invalid;

  用途：标识为Valid为Mux状态组中使用；

##### 6.10.9.2 Valid标识组

Lock标识根据Sensor组分为四组，分别为ValidA组，ValidB组，ValidC组，ValidD组

##### 6.10.9.3 手动标识Valid

1. 方法1：
   1. 在表中点击某通道或者部分通道->点击按钮Valid->即可修改当前通道标识为Valid;
   2. 在表中点击某通道或者部分通道->点击按钮InValid->即可修改当前通道标识为InValid;
2. 方法2：
   1. 在Curves configuration模块中，AvgStd的Min-Max范围进行筛选，点击Transform to valid;

##### 6.10.9.4 自动标识Valid

使用AutoScan模块功能中的ValidQc模块会标识Valid还是InValid;

##### 6.10.9.4 重新统计标识Valid

点击按钮Count，即可重新统计标识Valid的通道数

##### 6.10.9.5 清除统计标识Valid

点击按钮Clear，即可将所有通道默认标识为Invalid；

#### 6.10.10 Mux状态表

Mapping图表映射1024通道的Mux标识状态；

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216102110653.png" alt="image-20220216102110653" style= "zoom:100%; float:left;"   />

##### 6.10.10.1 Mux状态标识描述

- Sensor A	归类为SensorA组
- Sensor B    归类为SensorB组
- Sensor C    归类为SensorC组
- Sensor D   归类为SensorD组

解释：Mux标识为软件逻辑标识，即将Sensor分为ABCD组，只有这四组状态，当归类为A组时，normal将Sensor State设置为NormalA,unblock将Sensor State设置为A unblock;BCD以此类推；

##### 6.10.10.2 Mux状态标识目的用途

目的：1024通道中每一通道都有ABCD，所以Mux是为了混合模式使用，即某些通道在A组，某些通道在B组，依此类推；

##### 6.10.10.3 Mux状态标识软件逻辑

通过Valid中四组的标识状态Valid，重新生成一张Mux标识表，举例如：

- 1-10通道只有A,B组是Valid的->	1-10通道会将其归类SensorA和SensorB两组的其中一组(随机分配或者顺移分配)；
- 11-20通道只有C组Valid-> 11-20通道会将其归类为SensorC组，且不会变成其他Sensor组
- 21-30通道ABCD组均InValid-> 21-30通道会将其归类为SensorABCD组中的一组(随机分配或者顺移分配)；

##### 6.10.10.4 标识随机分配Random

点击Random 按钮，根据 Mux状态标识软件逻辑，分配规则为随机分配；

##### 6.10.10.5 标识顺移分配Mux

点击Mux按钮，根据 Mux状态标识软件逻辑，分配规则为顺移分配；

##### 6.10.10.6 Mux标识重新统计

点击按钮Count，即可重新统计Mux标识对应的通道数

##### 6.10.10ss.7 Mux标识清除

点击按钮Clear，即可清除所有通道标识为SensorA组

### 6.11 Chart图表统计

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216105418802.png" alt="image-20220216105418802" style="zoom:80%;" />

图表统计目前分为： Sensor统计，Cap电容统计，Pore孔统计，Valid有效统计，Reads统计；

#### 6.11.1 Sensor占比统计

##### 6.11.1.1 Sensor统计更新

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111747011.png" alt="image-20220216111747011" style= "zoom:80%; "  />

点击Update按钮即可

#### 6.11.2 Cap Level统计

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111713382.png" alt="image-20220216111713382" style= "zoom:80%;"   />

##### 6.11.2.1  Cap Level统计更新

点击Update按钮即可

#### 6.11.3 Pore 占比统计

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111816484.png" alt="image-20220216111816484" style="zoom:80%;" />

##### 6.11.3.1  Pore 占比统计更新

点击Update按钮即可

#### 6.11.4 Valid统计

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111829252.png" alt="image-20220216111829252" style="zoom:80%;" />

##### 6.11.4.1  Valid统计更新

点击Update按钮即可

#### 6.11.5 Reads Adapters统计

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111902898.png" alt="image-20220216111902898" style="zoom: 80%;" />

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216111918819.png" alt="image-20220216111918819" style="zoom:80%;" />

##### 6.11.5.1  Reads Adapters记录

勾选SavePanel的Record选择框，勾选则在此刻创建csv文件，不勾选则关闭该csv文件，当开启Autoscan模块，停止保存数据时会记录当前的Autoseq的reads和adapters数目，输出至文件中，**文件保存在etc\ReadsAdapterRecord目录中**；

##### 6.11.5.2 Reads Adapters统计更新

Choose按钮选择 ReadsAdapters的csv记录文件，点击Update按钮即可在每一时间段的reads和adapter的统计和累计的统计中切换；

## 7 Log日志

<img src="C:\Users\zhangxianda\AppData\Roaming\Typora\typora-user-images\image-20220216112957888.png" alt="image-20220216112957888" style= "zoom:80%; float:left;"  />

Log日志会记录文件在etc\Logs目录下；