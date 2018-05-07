#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

extern "C"
{
#include "libavformat/avformat.h"
};

int YUVToRGB(unsigned char* pYUV, int width, int height, AVPixelFormat src_pix_fmt, unsigned char* pBGR24, AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24);

#endif // COLOR_SPACE_H
