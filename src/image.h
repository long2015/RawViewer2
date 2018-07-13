#ifndef RAWVIEWER2_IIMAGE_H
#define RAWVIEWER2_IIMAGE_H

#include <string>
#include <opencv2/opencv.hpp>


enum RV_FILE_TYPE
{
    RV_FILE_TYPE_UNKNOWN = 0,

    RV_FILE_TYPE_BMP = 1,
    RV_FILE_TYPE_PNG,
    RV_FILE_TYPE_JPG,
    RV_FILE_TYPE_GIF,
    RV_FILE_TYPE_AVI,

    RV_FILE_TYPE_RAW,
};

enum RV_COLOR_SPACE
{
    RV_COLOR_SPACE_UNKNOWN = 0,

    RV_COLOR_SPACE_RGB24 = 100,
    RV_COLOR_SPACE_BGR24,
    RV_COLOR_SPACE_RGB24P,
    RV_COLOR_SPACE_RGB24_FP16,
    RV_COLOR_SPACE_BGR24_FP16,
    RV_COLOR_SPACE_RGB24P_FP16,
    RV_COLOR_SPACE_RGB24_FLOAT,
    RV_COLOR_SPACE_BGR24_FLOAT,
    RV_COLOR_SPACE_RGB24P_FLOAT,

    RV_COLOR_SPACE_YUV420 = 200,
    RV_COLOR_SPACE_I420 = RV_COLOR_SPACE_YUV420,
    RV_COLOR_SPACE_NV12,
    RV_COLOR_SPACE_YUV420SP = RV_COLOR_SPACE_NV12,
    RV_COLOR_SPACE_NV21,
    RV_COLOR_SPACE_YUV420SP_VU = RV_COLOR_SPACE_NV21,
    RV_COLOR_SPACE_GRAY,
    RV_COLOR_SPACE_Y = RV_COLOR_SPACE_GRAY,
};

enum RV_DATA_TYPE
{
    RV_DATA_CHAR,
    RV_DATA_FP16,
    RV_DATA_FLOAT,
};

std::string toColorStr(int color);
int toColor(std::string str);
int getFrameLen(int width, int height, int color);

typedef struct FrameInfo
{
    int width;
    int height;
    int color;
    int dataType;
    int frameLen;
}FrameInfo;

class CFrame
{
public:
    CFrame();
    CFrame(int width, int height, int color);
    CFrame(const FrameInfo info);
    void init(int width, int height, int color);

    //øΩ±¥ππ‘Ï
    CFrame(const CFrame& frame);

    void* getData(int index);
    void* getRGBData(bool shift = 0);

public:
    FrameInfo info;
    int frameId;

    void* data[4];
    int stride[4];

    unsigned char* dataRGB[4];
    int strideRGB[4];
};

class IImageFile
{
public:
    static IImageFile* createImage(std::string filename);
    static bool isSupport(std::string filename);

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool getFrameInfo(FrameInfo& info) = 0;
    virtual int getFrameCount() = 0;
    virtual int getFrame(int nframe, CFrame& frame) = 0;
//    virtual int exportPicture(std::string filename, int x, int y, int width, int height) = 0;
//    virtual int exportVideo(std::string filename, int start, int end, int x, int y, int width, int height) = 0;
};

#endif //RAWVIEWER2_IIMAGE_H
