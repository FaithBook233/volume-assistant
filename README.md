# VolumeAssistant

#### 介绍
音量助手，帮助自动判别音频设备，选择音量设置。




#### 安装教程
1. 安装
2. 设置后台启动（自动））

#### 执行流程
1. 程序启动
2. 识别当前设备
3. 如果当前设备没有在程序中备份，则保存一份当前设备对应地文件，保存当前的音量
4. 如果有备份，则直接读取保存文件，设置相应的音量
5. 循环（频率有待探究，可用Sleep控制）
6. 检测设备变化，如果设备变化则执行2
7. 如果音量变化则设置当前音量变量
