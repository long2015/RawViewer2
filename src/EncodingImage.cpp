/*
**	********************************************************************************
**
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2012, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**	File		: EncodingImage.cpp
**	Description	: 
**  Create		: 18-6-6      LDL		Create the file
**
**	********************************************************************************
*/

#include "EncodingImage.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>


bool CEncodingImageFile::isSupport(std::string filename, std::string extName)
{
    cv::Mat img = cv::imread(filename.c_str());
    if( img.empty() )
    {
        cv::VideoCapture capture;
        capture.open(filename.c_str());
        int frames = capture.get(CV_CAP_PROP_FRAME_COUNT);
        if( frames == 0 )
        {
            printf("open failed. maybe not a picture or video.\n");
            return false;
        }
        capture.read(img);
    }

    printf("img width:%d height:%d\n", img.cols, img.rows);

    return true;
}

CEncodingImageFile::CEncodingImageFile(std::string filename)
{
    m_filename = filename;
}

bool CEncodingImageFile::open()
{
    m_mat = cv::imread(m_filename.c_str());
    if( !m_mat.empty() )
    {
        m_frameCount = 1;
    }
    else
    {
        m_capture.open(m_filename.c_str());
        int frames = m_capture.get(CV_CAP_PROP_FRAME_COUNT);
        if( frames == 0 )
        {
            return false;
        }
        m_frameCount = frames;
        printf("frames:%d\n", frames);
        m_capture.read(m_mat);
    }

    printf("img width:%d height:%d\n", m_mat.cols, m_mat.rows);
    m_frame = CFrame(m_mat.cols/2*2, m_mat.rows/2*2, RV_COLOR_SPACE_BGR24);

    return true;
}

bool CEncodingImageFile::close()
{
    return false;
}

bool CEncodingImageFile::getFrameInfo(FrameInfo &info)
{
    info = m_frame.info;

    return true;
}

int CEncodingImageFile::getFrameCount()
{
    return m_frameCount;
}

int CEncodingImageFile::getFrame(int nframe, CFrame &frame)
{
    if( nframe >= m_frameCount )
        return -1;

    if( m_frameCount > 1 )
    {
//        printf("[%s] nframe£º%d frameCount:%d\n", __FUNCTION__, nframe, m_frameCount);
        m_capture.set(CV_CAP_PROP_POS_FRAMES, nframe);
        m_capture.read(m_mat);
    }

//    memcpy(m_frame.data[0], m_mat.data, m_mat.cols*m_mat.rows*3);
    cvMatToFrame(m_mat, m_frame);

    frame = m_frame;
    return 0;
}
