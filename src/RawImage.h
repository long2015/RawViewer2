#ifndef RAWVIEWER2_RAWIMAGE_H
#define RAWVIEWER2_RAWIMAGE_H

#include "image.h"

class CRawImageFile : public IImageFile
{
public:
    static bool isSupport(std::string filename, std::string extName);

public:
    CRawImageFile(std::string filename);
    CRawImageFile(std::string filename, int width, int height, int color);

    virtual bool open();
    virtual bool close();
    virtual bool getFrameInfo(FrameInfo& info);
    virtual int getFrameCount();
    virtual int getFrame(int nframe, CFrame& frame);

private:
    static bool parseFileName(std::string filename, FrameInfo& info);

private:
    std::string m_filename;
    int         m_filetype;
    void*       m_fileHandler;

    FrameInfo   m_frameInfo;
    int         m_frameCount;
    CFrame      m_curFrame;

    static std::map<std::string, int> m_supportExtName;
};


#endif //RAWVIEWER2_RAWIMAGE_H
