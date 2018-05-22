#include <assert.h>
#include <stdio.h>
#include "RawImage.h"

CRawImage::CRawImage(std::string filename, int width, int height, int color)
    : m_frame(width, height, color)
{
    m_filename = filename;
}

bool CRawImage::open()
{
    m_fp = fopen(m_filename.c_str(), "rb");
    assert(m_fp);

    fseek(m_fp, 0, SEEK_END);

    long long int pos = ftell(m_fp);
    size_t frameLen = m_frame.info.frameLen;
    m_frameCount = pos / frameLen;
    printf("m_frameCount:%d len:%d frameLen:%d\n", m_frameCount, pos, frameLen);

    return true;
}

bool CRawImage::close()
{
    if( m_fp != NULL )
    {
        fclose(m_fp);
        m_fp = NULL;
    }
    return true;
}

bool CRawImage::getFrameInfo(FrameInfo& info)
{
    info = m_frame.info;
    return true;
}

int CRawImage::getFrameCount()
{
    return m_frameCount;
}

int CRawImage::getFrame(int nframe, CFrame& frame)
{
    int len = m_frame.info.frameLen;
    fseek(m_fp, len*nframe, SEEK_SET);

    fread(m_frame.data[0], len, 1, m_fp);

    frame = m_frame;

    return 0;
}

