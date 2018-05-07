extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}
#include <QDebug>

int YUVToRGB(unsigned char* pYUV, int width, int height, AVPixelFormat src_pix_fmt,
             unsigned char* pBGR24, AVPixelFormat dst_pix_fmt /*= AV_PIX_FMT_RGB24*/)
{
    uint8_t *src_data[4] = {0};
    uint8_t *dst_data[4] = {0};
    int src_stride[4] = {0, 0, 0, 0};
    int dst_stride[4] = {width*3, 0, 0, 0};
    int src_w = width, src_h = height;
    int dst_w = width, dst_h = height;

    int ret;
    SwsContext *sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt,
                             dst_w, dst_h, dst_pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx)
    {
        fprintf(stderr,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);
        ret = AVERROR(EINVAL);
        return ret;
    }
    av_image_fill_linesizes(src_stride, src_pix_fmt, src_w);
    av_image_fill_linesizes(dst_stride, dst_pix_fmt, src_w);

    av_image_fill_arrays(src_data, src_stride, pYUV, src_pix_fmt, src_w, src_h, 1);
    av_image_fill_arrays(dst_data, dst_stride, pBGR24, dst_pix_fmt, src_w, src_h, 1);
    //qDebug("src_data:%p %p %p %p src_stride:%d %d %d %d\n", src_data[0],src_data[1],src_data[2],src_data[3],
    //       src_stride[0],src_stride[1],src_stride[2],src_stride[3]);

    qDebug("%s -> %s %dx%d\n", av_get_pix_fmt_name(src_pix_fmt), av_get_pix_fmt_name(dst_pix_fmt), src_w, src_h);
    /* convert to destination format */
    ret = sws_scale(sws_ctx, (const uint8_t * const*)src_data,
              src_stride, 0, src_h, dst_data, dst_stride);

    sws_freeContext(sws_ctx);

    return ret;
}
