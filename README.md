YUV Player -- YUV播放器
=======================

工程说明
=======================
VS2010 MFC工程，源码有部分参考Yuvviewer代码，界面参考YUV Player Deluxe风格。

功能
=======================
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
    * YYUV422平面格式：UV422(I422);YV16;
    * YUV444格式：YUV444;
    * YUV422交织格式：YUY2(YUYV);YVYU;UYVY;VYUY;
    * YUV420半平面格式：NV12(YUV420SP);NV21;
    * YUV422半平面格式：NV16(YUV422SP);NV61

用法
=======================
右键Open选项；菜单File->Open选项；直接拖曳文件播放器界面。

界面
=======================
参数设置界面：
![Setting](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_setting.png)
关于界面：
![About](https://github.com/latelee/YUVPlayer/blob/master/screenshots/yuvplayer_about.png)
声明
=======================
本程序仅限于学习交流。
程序所用部分代码为网络上可搜索到的片段。
    
版本
=======================
* v1.0 临时版本，支持各类格式的播放，可用版本，初具雏形。
* v2.0 具备播放器基本功能。未添加参数保存功能。
* v4.0 具备播放器基本功能。具备参数保存功能(在Setting界面点击Apply或OK才可生效)，支持添加自定义分辨率。

作者主页
=======================
[迟思堂工作室](http://www.latelee.org)