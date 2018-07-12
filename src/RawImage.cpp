#include <assert.h>
#include <stdio.h>
#include <QtCore/QRegExp>
#include <QDebug>
#include "RawImage.h"


bool CRawImageFile::isSupport(std::string filename, std::string extName)
{
    FrameInfo info;
    return parseFileName(filename, info);
}

bool CRawImageFile::parseFileName(std::string filename, FrameInfo &info)
{
    QRegExp exp(".*_([0-9]+)x([0-9]+).([a-zA-Z0-9_]+)");
    bool match = exp.exactMatch(filename.c_str());

    if( !match )
    {
        qDebug() << "parser filename failed." << exp.capturedTexts();
        return false;
    }

    int width = exp.cap(1).toInt();
    int height = exp.cap(2).toInt();
    std::string color_str = exp.cap(3).toStdString();

    printf("width:%d height:%d color:%s\n", width, height, color_str.c_str());

    int color_space = toColor(color_str);
    if( color_space == RV_COLOR_SPACE_UNKNOWN )
    {
        printf("parser error. RV_COLOR_SPACE_UNKNOWN\n");
        return false;
    }

    info.width = width;
    info.height = height;
    info.color = color_space;
    info.dataType = 1;

    return true;
}

CRawImageFile::CRawImageFile(std::string filename)
    : m_filename(filename), m_frameCount(0)
{
    parseFileName(filename, m_frameInfo);
    m_curFrame = CFrame(m_frameInfo);

    m_frameInfo = m_curFrame.info;
}

CRawImageFile::CRawImageFile(std::string filename, int width, int height, int color)
    : m_filename(filename), m_frameCount(0), m_curFrame(width, height, color)
{
    m_frameInfo = m_curFrame.info;
}

bool CRawImageFile::open()
{
    m_fileHandler = fopen(m_filename.c_str(), "rb");
    if( m_fileHandler == NULL )
    {
        printf("[%s] open %s failed.\n", __FUNCTION__, m_filename.c_str());
        return false;
    }

    fseek((FILE*)m_fileHandler, 0, SEEK_END);

    long long int pos = ftell((FILE*)m_fileHandler);
    size_t frameLen = m_frameInfo.frameLen;
    m_frameCount = pos / frameLen;
    printf("m_frameCount:%d len:%lld frameLen:%u\n", m_frameCount, pos, frameLen);

    return true;
}

bool CRawImageFile::close()
{
    if( m_fileHandler != NULL )
    {
        fclose((FILE*)m_fileHandler);
        m_fileHandler = NULL;
    }
    return true;
}

bool CRawImageFile::getFrameInfo(FrameInfo& info)
{
    info = m_curFrame.info;
    return true;
}

int CRawImageFile::getFrameCount()
{
    return m_frameCount;
}

int CRawImageFile::getFrame(int nframe, CFrame& frame)
{
    int64_t len = m_frameInfo.frameLen;
    fseeko64((FILE*)m_fileHandler, len*nframe, SEEK_SET);
    fread(m_curFrame.data[0], len, 1, (FILE*)m_fileHandler);

    frame = m_curFrame;

    return 0;
}
