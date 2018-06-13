#include <string>

#include "image.h"
#include "RawImage.h"
#include "EncodingImage.h"
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

static bool is_float(int color)
{
    return color == AV_PIX_FMT_RGB24_FLOAT || color == AV_PIX_FMT_BGR24_FLOAT || color == AV_PIX_FMT_RGB24P_FLOAT;
}
static bool is_fp16(int color)
{
    return color == AV_PIX_FMT_RGB24_FP16 || color == AV_PIX_FMT_BGR24_FP16 || color == AV_PIX_FMT_RGB24P_FP16;
}

static unsigned char float_to_char(float value, float min, float max)
{
    int tmp = 0;
    if( min < -1 || max > 1 )
    {
        min = min > 0? 0 : min;
        tmp = value-min;
    }
    else
    {
        min = min > 0? 0 : min;
        tmp = (value-min)*255;
    }
    unsigned char c = (tmp > 255) ? (unsigned char)255 : (unsigned char)tmp;

    return c;
}
static bool parserFileName(std::string filename, FrameInfo &info)
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
    if( color_space == AV_PIX_FMT_NONE )
    {
        printf("parser error. PIX_FMT_NONE\n");
        return false;
    }

    info.width = width;
    info.height = height;
    info.color = color_space;
    info.dataType = 1;

    return true;
}
static bool tryOpen(std::string filename, FrameInfo &info)
{
    cv::Mat img;// = cv::imread(filename.c_str());
    if( img.empty() )
    {
        cv::VideoCapture capture;
        capture.open(filename.c_str());
        int frames = capture.get(CV_CAP_PROP_FRAME_COUNT);
        if( frames == 0 )
        {
            return false;
        }
        printf("frames:%f\n", frames);
        capture.read(img);
    }

    printf("img width:%d height:%d\n", img.cols, img.rows);
    return true;
}

std::string toColorStr(int color)
{
    std::string colorStr = "RGB";

    return colorStr;
}

int toColor(std::string color_str)
{
    AVPixelFormat color_space = av_get_pix_fmt(color_str.c_str());
    if( color_space == AV_PIX_FMT_NONE )
    {
        if (std::string("i420") == color_str)
        {
            color_space = AV_PIX_FMT_YUV420P;
        }
        else if( std::string("rgb24p") == color_str )
        {
            color_space = AVPixelFormat(AV_PIX_FMT_RGB24P);
        }
        else if( std::string("rgb24p_float") == color_str )
        {
            color_space = AVPixelFormat(AV_PIX_FMT_RGB24P_FLOAT);
        }
        else if( std::string("rgb24_float") == color_str )
        {
            color_space = AVPixelFormat(AV_PIX_FMT_RGB24_FLOAT);
        }
        else if( std::string("bgr24_float") == color_str )
        {
            color_space = AVPixelFormat(AV_PIX_FMT_BGR24_FLOAT);
        }
        else
        {
            printf("Not support color space:%s\n", color_str.c_str());
            return AV_PIX_FMT_NONE;
        }
    }

    return color_space;
}

static int image_get_buffer_size(int width, int height, int color, int align)
{
    if( color == AV_PIX_FMT_RGB24P )
    {
        return width*height*3;
    }
    else if( is_float(color) )
    {
        return width*height*3* sizeof(float);
    }
    else
    {
        return av_image_get_buffer_size((AVPixelFormat)color, width, height, align);
    }
}

static int image_alloc(uint8_t *pointers[4], int linesizes[4],
    int w, int h, enum AVPixelFormat pix_fmt, int align)
{
    memset(pointers, 0, sizeof(pointers));
    if( pix_fmt == AV_PIX_FMT_RGB24P )
    {
        pointers[0] = (uint8_t*)malloc(w*h*3);
        pointers[1] = pointers[0] + w*h;
        pointers[2] = pointers[1] + w*h;
    }
    else if( is_float(pix_fmt) )
    {
        if( pix_fmt == AV_PIX_FMT_RGB24_FLOAT || pix_fmt == AV_PIX_FMT_BGR24_FLOAT )
        {
            pointers[0] = (uint8_t*)malloc(w*h*3*sizeof(float));
        }
        else if( pix_fmt == AV_PIX_FMT_RGB24P_FLOAT )
        {
            pointers[0] = (uint8_t*)malloc(w*h*3*sizeof(float));
            pointers[1] = pointers[0] + w*h*sizeof(float);
            pointers[2] = pointers[1] + w*h*sizeof(float);
        }
    }
    else
    {
        return av_image_alloc(pointers, linesizes, w, h, (AVPixelFormat)pix_fmt, align);
    }
}

static void image_copy(uint8_t *dst_data[4], int dst_linesizes[4],
    const uint8_t *src_data[4], const int src_linesizes[4],
    enum AVPixelFormat pix_fmt, int width, int height)
{
    if( pix_fmt == AV_PIX_FMT_RGB24P )
    {
        memcpy(dst_data[0], src_data[0], width*height);
        memcpy(dst_data[1], src_data[1], width*height);
        memcpy(dst_data[2], src_data[2], width*height);
    }
    else if( is_float(pix_fmt) )
    {
        if( pix_fmt == AV_PIX_FMT_RGB24_FLOAT || pix_fmt == AV_PIX_FMT_BGR24_FLOAT )
        {
            memcpy(dst_data[0], src_data[0], width*height*3*sizeof(float));
        }
        else if( pix_fmt == AV_PIX_FMT_RGB24P_FLOAT )
        {
            memcpy(dst_data[0], src_data[0], width*height*sizeof(float));
            memcpy(dst_data[1], src_data[1], width*height*sizeof(float));
            memcpy(dst_data[2], src_data[2], width*height*sizeof(float));
        }
    }
    else
    {
        return av_image_copy(dst_data, dst_linesizes, src_data, src_linesizes, pix_fmt, width, height);
    }
}

CFrame::CFrame()
{

}

CFrame::CFrame(FrameInfo info)
{
    CFrame(info.width, info.height, info.color);
}

CFrame::CFrame(int width, int height, int color)
{
    info.width = width;
    info.height = height;
    info.color = color;
    info.dataType = is_float(color);
    info.frameLen = image_get_buffer_size(width, height, color, 1);

    printf("CFrame::CFrame width:%d height:%d color:%d frameLen:%d\n", width, height, color, info.frameLen);

    image_alloc(data, stride, width, height, (AVPixelFormat)color, 1);
    image_alloc(dataRGB, strideRGB, width, height, AV_PIX_FMT_RGB24, 1);

    printf("data %p %p %p %p\n", data[0], data[1], data[2], data[3]);
    printf("dataRGB %p %p %p %p\n", dataRGB[0], dataRGB[1], dataRGB[2], dataRGB[3]);
}

CFrame::CFrame(const CFrame &frame)
{
    printf("frame copy constructor\n");

    info = frame.info;
    image_alloc(data, stride, info.width, info.height, (AVPixelFormat)info.color, 1);

    image_copy(data, stride, (const uint8_t **)frame.data, frame.stride, (AVPixelFormat)info.color, info.width, info.height);

    image_alloc(dataRGB, strideRGB, info.width, info.height, AV_PIX_FMT_RGB24, 1);

    printf("data %p %p %p %p\n", data[0], data[1], data[2], data[3]);
    printf("dataRGB %p %p %p %p\n", dataRGB[0], dataRGB[1], dataRGB[2], dataRGB[3]);
}

void* CFrame::getData(int index)
{
    return data[index];
}


void* CFrame::getRGBData(bool shift)
{
    float min = 0;
    float max = 0;
    if( is_float(info.color))
    {
        float *m_data_float = (float *) data[0];
        for(int i = 0; i < info.width * info.height * 3; ++i)
        {
            if( min > m_data_float[i] )
                min = m_data_float[i];

            if( max < m_data_float[i] )
                max = m_data_float[i];
        }
        printf("min:%f max:%f\n", min, max);
    }

    int ret = -1;
    if( info.color == AV_PIX_FMT_RGB24P )
    {
        printf("RGB24P\n");
        unsigned char* _data = (unsigned char*)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3 * i + 0] = _data[i + info.width * info.height * 0];
            rgb_data[3 * i + 1] = _data[i + info.width * info.height * 1];
            rgb_data[3 * i + 2] = _data[i + info.width * info.height * 1];
        }
    }
    else if( info.color == AV_PIX_FMT_RGB24_FLOAT )
    {
        float *data_float = (float *)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3 * i + 0] = float_to_char(data_float[3 * i + 0], min, max);
            rgb_data[3 * i + 1] = float_to_char(data_float[3 * i + 1], min, max);
            rgb_data[3 * i + 2] = float_to_char(data_float[3 * i + 2], min, max);
        }
    }
    else if( info.color == AV_PIX_FMT_BGR24_FLOAT )
    {
        float *m_data_float = (float *)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3 * i + 0] = float_to_char(m_data_float[3 * i + 2], min, max);
            rgb_data[3 * i + 1] = float_to_char(m_data_float[3 * i + 1], min, max);
            rgb_data[3 * i + 2] = float_to_char(m_data_float[3 * i + 0], min, max);
        }
    }
    else if( info.color == AV_PIX_FMT_RGB24P_FLOAT )
    {
        printf("rgbpfffff %d %d\n", info.width, info.height);
        float *m_data_float = (float *)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];

        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3 * i + 0] = float_to_char(m_data_float[i], min, max);
            rgb_data[3 * i + 1] = float_to_char(m_data_float[i + info.width * info.height * 1], min,
                                                max);
            rgb_data[3 * i + 2] = float_to_char(m_data_float[i + info.width * info.height * 2], min,
                                                max);
        }
    }
    else
    {
        int src_w = info.width;
        int src_h = info.height;
        AVPixelFormat src_pix_fmt = (AVPixelFormat) info.color;
        int dst_w = info.width;
        int dst_h = info.height;
        AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;

        //convert
        SwsContext *sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL,
                                             NULL);
        if( !sws_ctx )
        {
            fprintf(stderr, "Impossible to create scale context for the conversion "
                            "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n", av_get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                    av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);

            return NULL;
        }
        ret = sws_scale(sws_ctx, (const uint8_t *const *) data, stride, 0, src_h, dataRGB, strideRGB);
        printf("ret:%d\n", ret);
        sws_freeContext(sws_ctx);
    }
    return dataRGB[0];
}


IImage *IImage::createImage(std::string filename)
{
    FrameInfo info;
    if( parserFileName(filename, info) )
    {
        return IImage::createImage(filename, info.width, info.height, info.color);
    }
    else if(tryOpen(filename, info) )
    {
        return new CEncodingImage(filename);
    }
    else
    {
        printf("[%s] parser filename:%s error.\n", __FUNCTION__, filename.c_str());
        return NULL;
    }
}

IImage *IImage::createImage(std::string filename, int width, int height, int color)
{
    return new CRawImage(filename, width, height, color);
}
