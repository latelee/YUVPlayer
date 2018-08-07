# YUV Player -- A YUV player based on MFC

[![GitHub stars](https://img.shields.io/github/stars/latelee/YUVPlayer.svg)](https://github.com/latelee/YUVPlayer)[![GitHub forks](https://img.shields.io/github/forks/latelee/YUVPlayer.svg)](https://github.com/latelee/YUVPlayer)

Stargazers over time  
[![Stargazers over time](https://starcharts.herokuapp.com/latelee/YUVPlayer.svg)](https://starcharts.herokuapp.com/latelee/YUVPlayer)

## Project
VS2010 MFC project, ref YUV Player Deluxe.

## Project Feature
* support different YUV format.
* support common resolution, support custom width&height, and can add to the system.
* support different rate, loop play.
* support play frame by frame, jump to first frame and last frame.
* window scale.
* support save to yuv file and bmp file.
* support yuv format:
  * y, ie: YUV400.
  * YUV420 planar, ie: YUV420(I420), YV12.
  * YUV422 planar, ie: YUV422(I422), YV16.
  * YUV444 planar, ie: YUV444.
  * YUV422 interleaved, ie: YUY2(YUYV), YVYU, UYVY, VYUY.
  * YUV420 semi-planar, ie: NV12(YUV420SP), NV21.
  * YUV420 semi-planar, ie: NV16(YUV422SP), NV61.
  * RGB24, BGR24.

## Usage
Click menu File->Open option, or drag file to the main window, <br>

## Window view
Setting window: <br>
![Setting](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_setting.png)

About windows: <br>
![About](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_about.png)
  
## Changelog
The binary file will locate in release directory. <br>
* v1.0 first version, can play different yuv format. <br>
* v2.0 impletement basic function for a player.<br>
* v4.0 save parameter(Setting->Apply or OK). 
* v4.1 fix some bug.
* v4.2 add play and save for rgb(24 bit) file

## Testing
Only test under Windows 7 64bit OS, and may run on other Windows OS.<br>

## Some bug
Not found yet.

## Protocol
* Copyright [CST studio Late Lee](http://www.latelee.org)
* Total code is BSP.
* You can use the code for study, and commercial purposes, but give no guarantee.

## TODOs
* split yuv file for Y, U, V.
* support different yuv format transformation. 

## Author
CST studio Late Lee<br>
[CST studio](http://www.latelee.org) <br>
Donate the author <br>
![Donate](https://github.com/latelee/YUVPlayer/blob/master/screenshots/latelee_pay_small.png)

# YUV Player -- 基于MFC界面的YUV播放器

## 工程说明
VS2010 MFC工程，界面参考YUV Player Deluxe风格。

## 功能
* 可播放各种格式的YUV文件。
* 默认预留常见分辨率，同时支持自定义高宽，可添加至系统。
* 支持不同帧率播放，支持循环播放。
* 支持前后逐帧跳转；支持跳转首帧、尾帧。
* 支持窗口缩放。
* 支持保存YUV文件或BMP图片文件。
* 自动解析文件名，得到分辨率、YUV格式。
* 目前支持格式有：
    * Y亮度分量：即YUV400;
    * YUV420平面格式：YUV420(I420);YV12;
    * YYUV422平面格式：YUV422(I422);YV16;
    * YUV444格式：YUV444;
    * YUV422交织格式：YUY2(YUYV);YVYU;UYVY;VYUY;
    * YUV420半平面格式：NV12(YUV420SP);NV21;
    * YUV422半平面格式：NV16(YUV422SP);NV61;
    * RGB24、BGR24格式

## 用法
右键Open选项；菜单File->Open选项；直接拖曳文件播放器界面。

## 界面
参数设置界面：<br>
![Setting](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_setting.png)

关于界面：<br>
![About](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_about.png)
  
## 版本变更
编译好的工具位于release目录中。<br>
* v1.0 临时版本，支持各类格式的播放，可用版本，初具雏形。
* v2.0 具备播放器基本功能。未添加参数保存功能。
* v4.0 具备播放器基本功能。具备参数保存功能(在Setting界面点击Apply或OK才可生效)，支持添加自定义分辨率。
* v4.1 bug fix。
* v4.2 添加rgb(24位)文件播放、保存功能。

## 测试
本工具仅在Windows 7 64bit操作系统中运行测试通过。但理论上应该可以在其它Windows系列系统上运行。<br>

## 可能潜在问题
暂无发现。

## 协议
* 版权所有 [迟思堂工作室 李迟](http://www.latelee.org)
* 本工程源码使用协议：BSP。
* 本软件可用于包括但不限于学习、研究、商业，但随之而来的成果/后果与软件作者无关。

## 待完善功能
* 添加YUV分量分离功能。
* 多种YUV格式相互转换功能。

## 作者
思堂工作室 李迟<br>
[迟思堂工作室](http://www.latelee.org) <br>
如果觉得本软件不错，欢迎捐赠支持作者 <br>
![捐赠](https://github.com/latelee/YUVPlayer/blob/master/screenshots/latelee_pay_small.png)

