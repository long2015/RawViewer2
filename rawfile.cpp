#include "rawfile.h"
#include <assert.h>
#include <string.h>
#include <color_space.h>
#include "libavformat/avformat.h"


CRawFile::CRawFile()
{

}

bool CRawFile::Open(std::string filename)
{
    if( !ParserFileName(filename, m_image_info) )
    {
        return false;
    }

    FILE* fp = fopen(filename.c_str(), "rb");
    assert(fp);

    m_data = (char*)malloc(m_data_len);
    m_rgb_data = (char*)malloc(m_rgb_data_len);
    assert(m_data);
    fread(m_data, m_data_len, 1, fp);
    fclose(fp);

    Reload(m_cur_color);

    return true;
}

bool CRawFile::Reload(int color_space)
{
    int ret =  YUVToRGB((unsigned char*)m_data, m_image_info.width, m_image_info.height, (AVPixelFormat)m_cur_color,
             (unsigned char*)m_rgb_data);

    if( m_cur_color == AV_PIX_FMT_RGB24 )
    {
        color_space = AV_PIX_FMT_BGR24;
    }
    else
    {
        color_space = AV_PIX_FMT_RGB24;
    }
    m_cur_color = color_space;

    return ret == 0;
}

bool CRawFile::ParserFileName(std::string filename, ImageInfo &imageInfo)
{
    int width;
    int height;
    char color_str[16];

    int index = filename.size();
    while( index >= 0 )
    {
        if( filename[index] == '_' || filename[index] == '/' )
            break;
        index--;
    }
    index++;

    int ret = sscanf(filename.c_str()+index, "%dx%d.%s", &width, &height, color_str);
    //printf("ret:%d width:%d height:%d color:%s\n", ret, width, height, color_str);
    if( ret != 3 )
    {
        printf("parser error.\n");
        return false;
    }

    if( std::string("nv21") == color_str )
    {
        imageInfo.color_space = AV_PIX_FMT_NV21;
        m_data_len = width*height*3/2;
    }
    else if( std::string("nv12") == color_str )
    {
        imageInfo.color_space = AV_PIX_FMT_NV12;
        m_data_len = width*height*3/2;
    }
    else if( std::string("i420") == color_str )
    {
        imageInfo.color_space = AV_PIX_FMT_YUV420P;
        m_data_len = width*height*3/2;
    }
    else if( std::string("rgb24") == color_str )
    {
        imageInfo.color_space = AV_PIX_FMT_RGB24;
        m_data_len = width*height*3;
    }
    else if( std::string("bgr24") == color_str )
    {
        imageInfo.color_space = AV_PIX_FMT_BGR24;
        m_data_len = width*height*3;
    }
    else
    {
        printf("Not support color space:%s\n", color_str);
        return false;
    }

    imageInfo.width = width;
    imageInfo.height = height;
    m_rgb_data_len = width*height*3;
    m_cur_color = imageInfo.color_space;

    return true;
}

bool CRawFile::GetImageInfo(ImageInfo &image_info)
{
    image_info = m_image_info;

    return true;
}

void* CRawFile::GetFrame()
{
    return m_rgb_data;
}

int CRawFile::GetFrameLen()
{
    return m_rgb_data_len;
}
