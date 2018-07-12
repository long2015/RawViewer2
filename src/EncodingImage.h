/*
**	********************************************************************************
**
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**	File		: EncodingImage.h
**	Description	: 
**  Create		: 18-6-6      LDL		Create the file
**
**	********************************************************************************
*/

#ifndef RAWVIEWER2_ENCODINGIMAGE_H
#define RAWVIEWER2_ENCODINGIMAGE_H

#include <opencv2/videoio.hpp>
#include "image.h"

class CEncodingImageFile : public IImageFile
{
public:
    static bool isSupport(std::string filename, std::string extName);

public:
    CEncodingImageFile(std::string filename);

    virtual bool open();
    virtual bool close();
    virtual bool getFrameInfo(FrameInfo& info);
    virtual int getFrameCount();
    virtual int getFrame(int nframe, CFrame& frame);

private:
    std::string m_filename;
    cv::VideoCapture m_capture;
    cv::Mat          m_mat;

    CFrame      m_frame;
    int         m_frameCount;
};


#endif //RAWVIEWER2_ENCODINGIMAGE_H
