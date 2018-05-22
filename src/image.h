#ifndef RAWVIEWER2_IIMAGE_H
#define RAWVIEWER2_IIMAGE_H

#include <string>
#include <libavutil/pixfmt.h>


enum
{
    AV_PIX_FMT_RGB24P  = (AV_PIX_FMT_NB + 100),
    AV_PIX_FMT_RGB24_FLOAT,
    AV_PIX_FMT_BGR24_FLOAT,
    AV_PIX_FMT_RGB24P_FLOAT,

    AV_PIX_FMT_RGB24_FP16,
    AV_PIX_FMT_BGR24_FP16,
    AV_PIX_FMT_RGB24P_FP16
};


std::string toColorStr(int color);
int toColor(std::string str);
int getFrameLen(int width, int height, int color);

struct FrameInfo
{
    int width;
    int height;
    int color;
    int dataType;
    int frameLen;
};

class CFrame
{
public:
    CFrame();
    CFrame(int width, int height, int color);
    CFrame(const FrameInfo info);

    //øΩ±¥ππ‘Ï
    CFrame(const CFrame& frame);

    void* getData(int index);
    void* getRGBData(bool shift = 0);

public:
    FrameInfo info;
    int frameId;

    unsigned char* data[4];
    int stride[4];

    unsigned char* dataRGB[4];
    int strideRGB[4];
};

class IImage
{
public:
    static IImage* createImage(std::string filename);
    static IImage* createImage(std::string filename, int width, int height, int color);

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool getFrameInfo(FrameInfo& info) = 0;
    virtual int getFrameCount() = 0;
    virtual int getFrame(int nframe, CFrame& frame) = 0;
};

#endif //RAWVIEWER2_IIMAGE_H
