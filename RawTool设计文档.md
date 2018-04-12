# RawViewer
显示RGB和YUV裸数据图片格式；后续增加格式转换功能。
候选名
- RYUViewer
- RGB-YUV-Viewer
- RY-Tool
- RawTool
- Raw Pic Viewer
- Raw Image Viewer
- RGB YUV Tool
- RawViewer 2
- easyRawViewer

## 需求分析
1. 显示RGB格式图像
    - RGB24、BGR24
    - RGBP
2. 显示YUV格式图像
    - NV21、NV12
    - YUV420
3. 支持文件拖动显示，并根据文件名解析图像信息:宽高，格式
4. 支持float和fp16格式的RGB图像
5. 支持显示归一化后的图像
6. 支持jpg,png转YUV和RGB
7. 支持播放YUV格式视频流

## 模块设计
1. 裸数据图片解析模块
    1. 文件名解析
    2. 格式加载
    3. 格式转换
2. GUI模块

## 重难点代码
1. 文件拖动实现
2. 正则模块