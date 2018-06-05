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

#include "image.h"

class CEncodingImage : public IImage
{
public:
    CEncodingImage(std::string filename);

    virtual bool open();
    virtual bool close();
    virtual bool getFrameInfo(FrameInfo& info);
    virtual int getFrameCount();
    virtual int getFrame(int nframe, CFrame& frame);
};


#endif //RAWVIEWER2_ENCODINGIMAGE_H
