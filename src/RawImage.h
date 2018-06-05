#ifndef RAWVIEWER2_RAWIMAGE_H
#define RAWVIEWER2_RAWIMAGE_H

#include "image.h"

class CRawImage : public IImage
{
public:
    CRawImage(std::string, int width, int height, int color);
    virtual bool open();
    virtual bool close();
    virtual bool getFrameInfo(FrameInfo& info);
    virtual int getFrameCount();
    virtual int getFrame(int nframe, CFrame& frame);
private:
    std::string m_filename;
    FILE*       m_fp;

    CFrame      m_frame;
    int         m_frameCount;
};


#endif //RAWVIEWER2_RAWIMAGE_H
