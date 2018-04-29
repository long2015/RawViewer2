# RawViewer2
YUV、RGB格式图像显示与转换工具.

## 需求分析
### 图像显示
1. 显示RGB格式图像
    - RGB24、BGR24
    - RGBP
    - RGB_float、RGB_fp16
2. 显示YUV格式图像
    - NV21、NV12
    - YUV420
    - YUV422
3. 支持文件拖动显示,并根据文件名解析图像信息:宽高，格式
4. 支持float和fp16格式的RGB图像
5. 支持显示jpg,png格式

### 图像转换
1. 支持转换jpg,png为YUV、RGB格式, 支持YUV与RGB互转
2. 支持缩放

### 视频流处理
1. 支持播放YUV,RGB格式裸视频流，并支持帧定位

### 抠图
1. 截取区域图像并保存

## 模块设计
1. 裸数据图片解析模块
    1. 文件名解析
    2. 图像数据解析
    3. rgb/yuv视频流格式解析
    3. jpg/avi解析

2. 格式转换
    1. 格式转换
    2. ROI区域转换

2. GUI模块
    1. 主界面MainWindow：管理子窗口
        1. 主菜单
    2. 显示界面ViewerWindow： 显示与选取区域
        1. 右键菜单
    3. 转换界面ConvertWindow： 转换数据

## 关键代码
1. 正则模块匹配文件名
2. 文件拖动实现
3. DMI窗口
4. ffmpeg格式转换
