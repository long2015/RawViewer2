#ifndef RAWVIEWER2_IIMAGE_H
#define RAWVIEWER2_IIMAGE_H


struct FrameInfo
{
    int width;
    int height;
    int color;
};

class IImage
{
    virtual bool open(char* filename) = 0;
    virtual bool open(char* filename, int width, int height, int color) = 0;
    virtual bool Reload(int width, int height, int color) = 0;
    virtual bool close() = 0;
    virtual bool getFrameInfo(FrameInfo& info) = 0;
    virtual int getFrameCount() = 0;
    virtual void* getFrame(int nframe) = 0;
};

#endif //RAWVIEWER2_IIMAGE_H
