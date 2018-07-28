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
#include <map>

std::map<std::string, int> g_supportExtName;

void Init()
{
    g_supportExtName["bmp"] = RV_FILE_TYPE_BMP;
    g_supportExtName["png"] = RV_FILE_TYPE_PNG;
    g_supportExtName["jpg"] = RV_FILE_TYPE_JPG;
    g_supportExtName["gif"] = RV_FILE_TYPE_GIF;
    g_supportExtName["avi"] = RV_FILE_TYPE_AVI;

    g_supportExtName["rgb24"] = RV_FILE_TYPE_RAW;
    g_supportExtName["bgr24"] = RV_FILE_TYPE_RAW;
    g_supportExtName["rgb24p"] = RV_FILE_TYPE_RAW;
    g_supportExtName["rgb24_fp16"] = RV_FILE_TYPE_RAW;
    g_supportExtName["bgr24_fp16"] = RV_FILE_TYPE_RAW;
    g_supportExtName["rgb24p_fp16"] = RV_FILE_TYPE_RAW;
    g_supportExtName["rgb24_float"] = RV_FILE_TYPE_RAW;
    g_supportExtName["bgr24_float"] = RV_FILE_TYPE_RAW;
    g_supportExtName["rgb24p_float"] = RV_FILE_TYPE_RAW;
    g_supportExtName["yuv420"] = RV_FILE_TYPE_RAW;
    g_supportExtName["i420"] = RV_FILE_TYPE_RAW;
    g_supportExtName["nv12"] = RV_FILE_TYPE_RAW;
    g_supportExtName["yuv420sp"] = RV_FILE_TYPE_RAW;
    g_supportExtName["nv21"] = RV_FILE_TYPE_RAW;
    g_supportExtName["yuv420sp_vu"] = RV_FILE_TYPE_RAW;
    g_supportExtName["gray"] = RV_FILE_TYPE_RAW;
    g_supportExtName["y"] = RV_FILE_TYPE_RAW;
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


std::string toColorStr(int color)
{
    std::string colorStr = "RGB";

    return colorStr;
}

int toColor(std::string color_str)
{
    if( color_str == "rgb24" )
    {
        return RV_COLOR_SPACE_RGB24;
    }
    else if( color_str == "nv21" )
    {
        return RV_COLOR_SPACE_NV21;
    }
    else if( color_str == "i420" )
    {
        return RV_COLOR_SPACE_I420;
    }

    return RV_COLOR_SPACE_RGB24;
}

int getFrameLen(int width, int height, int color)
{
    switch (color)
    {
        case RV_COLOR_SPACE_RGB24:
        case RV_COLOR_SPACE_BGR24:
            return width*height*3;
        case RV_COLOR_SPACE_NV21:
        case RV_COLOR_SPACE_I420:
            return width*height*3/2;
        default:
            printf("not support color:%d\n", color);
            return 0;
    }
}

int cvMatToFrame(cv::Mat mat, CFrame& frame)
{
    int colorSpace = frame.info.color;
    if( colorSpace != RV_COLOR_SPACE_RGB24 && colorSpace != RV_COLOR_SPACE_BGR24 )
    {
        printf("[%s] not support color space:%d\n", __FUNCTION__, colorSpace);
    }

    unsigned char* rgbData = (unsigned char*)frame.data[0];
    for (int i = 0; i < frame.info.height; ++i)
    {
        for (int j = 0; j < frame.info.width; ++j)
        {
            int index = i*frame.stride[0] + 3*j;
            rgbData[index + 0] = mat.at<cv::Vec3b>(i, j)[0];
            rgbData[index + 1] = mat.at<cv::Vec3b>(i, j)[1];
            rgbData[index + 2] = mat.at<cv::Vec3b>(i, j)[2];
        }
    }
}

int frameToCvMat(CFrame frame, cv::Mat& mat)
{
    int width = frame.info.width;
    int height = frame.info.height;
    int color_space = frame.info.color;
    unsigned char* data = (unsigned char*)frame.data[0];

    mat.create(height*3/2, width, CV_8UC1);
    for (int i = 0; i < height*width*3/2; ++i)
    {
        mat.at<uchar>(i) = data[i];
    }
}


int toCvCode(int color)
{
    switch (color)
    {
        case RV_COLOR_SPACE_NV21:
            return CV_YUV2RGB_NV21;
        case RV_COLOR_SPACE_NV12:
            return CV_YUV2RGB_NV12;
        case RV_COLOR_SPACE_I420:
            return CV_YUV2RGB_I420;
        default:
            printf("Not support color:%d\n", color);
            assert(0);
    }
}
static int image_alloc(void *pointers[4], int linesizes[4],
    int w, int h, int color_space, int align)
{
    memset(pointers, 0, sizeof(pointers));
    if( color_space == RV_COLOR_SPACE_RGB24P )
    {
        pointers[0] = (uint8_t*)malloc(w*h*3);
        pointers[1] = pointers[0] + w*h;
        pointers[2] = pointers[1] + w*h;
    }
    else if( color_space == RV_COLOR_SPACE_RGB24 )
    {
        pointers[0] = (uint8_t*)malloc(w*h*3);
    }
    else
    {
        printf("[%s] Not support color:%d\n", color_space);
    }
}

static void image_copy(void *dst_data[4], int dst_linesizes[4],
    const uint8_t *src_data[4], const int src_linesizes[4],
    int color_space, int width, int height)
{
    if( color_space == RV_COLOR_SPACE_RGB24P )
    {
        memcpy(dst_data[0], src_data[0], width*height);
        memcpy(dst_data[1], src_data[1], width*height);
        memcpy(dst_data[2], src_data[2], width*height);
    }
    else if( color_space == RV_COLOR_SPACE_RGB24 )
    {
        memcpy(dst_data[0], src_data[0], width*height*3);
    }
    else
    {
        printf("[%s] Not support color:%d\n", color_space);
    }
}

CFrame::CFrame()
{

}

CFrame::CFrame(FrameInfo info)
{
    init(info.width, info.height, info.color);
}

CFrame::CFrame(int width, int height, int color)
{
    init(width, height, color);
}

void CFrame::init(int width, int height, int color)
{
    info.width = width;
    info.height = height;
    info.color = color;
    info.dataType = RV_DATA_CHAR;
    info.frameLen = getFrameLen(width, height, color);

    printf("CFrame::CFrame width:%d height:%d color:%d frameLen:%d\n", width, height, color, info.frameLen);

    data[0] = malloc(info.frameLen);
    stride[0] = width*3;

    printf("data %p %p %p %p\n", data[0], data[1], data[2], data[3]);
}

CFrame::CFrame(const CFrame &frame)
{
    printf("frame copy constructor\n");

    info = frame.info;
    init(info.width, info.height, info.color);

    memcpy(data[0], frame.data[0], info.frameLen);
}

void* CFrame::getData(int index)
{
    return data[index];
}


bool cvtColor(const CFrame srcFrame, CFrame& dstFrame)
{
    FrameInfo info = srcFrame.info;
    void** data = (void**)srcFrame.data;
    void** dataRGB = (void**)dstFrame.data;

    float min = 0;
    float max = 0;
    if( info.dataType == RV_DATA_FLOAT )
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

    if( info.color == RV_COLOR_SPACE_RGB24 )
    {
        unsigned char* _data = (unsigned char*)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.frameLen; ++i)
        {
            rgb_data[i] = _data[i];
        }
    }
    else if( info.color == RV_COLOR_SPACE_BGR24 )
    {
        unsigned char* _data = (unsigned char*)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3*i +0] = _data[3*i+2];
            rgb_data[3*i +1] = _data[3*i+1];
            rgb_data[3*i +2] = _data[3*i+0];
        }
    }
    else if( info.color == RV_COLOR_SPACE_RGB24P )
    {
        unsigned char* _data = (unsigned char*)data[0];
        unsigned char *rgb_data = (unsigned char *)dataRGB[0];
        for(int i = 0; i < info.width * info.height; ++i)
        {
            rgb_data[3 * i + 0] = _data[i + info.width * info.height * 0];
            rgb_data[3 * i + 1] = _data[i + info.width * info.height * 1];
            rgb_data[3 * i + 2] = _data[i + info.width * info.height * 1];
        }
    }
    else if( info.color == RV_COLOR_SPACE_RGB24_FLOAT )
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
    else if( info.color == RV_COLOR_SPACE_BGR24_FLOAT )
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
    else if( info.color == RV_COLOR_SPACE_RGB24P_FLOAT )
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
        cv::Mat srcMat;
        frameToCvMat(srcFrame, srcMat);
        cv::Mat dstMat;
        cv::cvtColor(srcMat, dstMat, toCvCode(info.color));
        printf("row:%d col:%d\n", dstMat.rows, dstMat.cols);
        cv::imwrite("dst.jpg", dstMat);

        cvMatToFrame(dstMat, dstFrame);
        
        printf("finished.\n");
    }
    return dataRGB[0];
}


IImageFile *IImageFile::createImage(std::string filename)
{
    std::string extName;
    QString name = filename.c_str();
    QStringList list = name.split(".");
    if( list.size() >= 2 )
    {
        extName = list.back().toStdString();

        if( CEncodingImageFile::isSupport(filename, extName) )
        {
            return new CEncodingImageFile(filename);
        }
        else if( CRawImageFile::isSupport(filename, extName) )
        {
            return new CRawImageFile(filename);
        }
    }

    printf("[%s] filename:%s error.\n", __FUNCTION__, filename.c_str());
    return NULL;
}

bool IImageFile::isSupport(std::string filename)
{
    IImageFile* pImageFile = IImageFile::createImage(filename);
    if( pImageFile )
    {
        delete pImageFile;
        return true;
    }
    else
    {
        return false;
    }
}
