#ifndef RAWFILE_H
#define RAWFILE_H

#include <string>

struct ImageInfo
{
    int width;
    int height;
    int color_space;
};

class CRawFile
{
public:
    CRawFile();
    bool Open(std::string filename);
    bool Open(std::string filename, int width, int height, int color_space);
    bool Reload(int color_space);
    bool GetImageInfo(ImageInfo& image_info);

    void* GetFrame();
    int GetFrameLen();

    void Close();

private:
    bool ParserFileName(std::string filename, ImageInfo& imageInfo);

private:
    ImageInfo m_image_info;
    void*   m_data;       //原图像数据
    int     m_data_len;
    int     m_cur_color;

    void*   m_rgb_data;   //用于显示的rgb图像数据
    int     m_rgb_data_len;
};

#endif // RAWFILE_H
