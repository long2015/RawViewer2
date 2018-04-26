#include "rawfile.h"
#include <assert.h>
#include <string.h>
#include <color_space.h>
extern "C"
{
    #include <libavutil/pixdesc.h>
    #include "libavformat/avformat.h"
    #include <libavutil/imgutils.h>
}

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

    Reload(m_image_info.color_space);

    return true;
}

bool CRawFile::Reload(int color_space, int width, int height, int data_type)
{
    int ret =  YUVToRGB((unsigned char*)m_data, m_image_info.width, m_image_info.height, (AVPixelFormat)color_space,
             (unsigned char*)m_rgb_data);

    m_image_info.color_space = color_space;
    m_image_info.colorstr = av_get_pix_fmt_name((AVPixelFormat)color_space);

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

    AVPixelFormat color_space = av_get_pix_fmt(color_str);
    if( color_space == AV_PIX_FMT_NONE )
    {
        if (std::string("i420") == color_str)
        {
            color_space = AV_PIX_FMT_YUV420P;
        }
        else
        {
            printf("Not support color space:%s\n", color_str);
            return false;
        }
    }

    imageInfo.width = width;
    imageInfo.height = height;
    imageInfo.color_space = color_space;

    m_data_len = av_image_get_buffer_size(color_space, width, height, 1);
    m_rgb_data_len = width*height*3;

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
