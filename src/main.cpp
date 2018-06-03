#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

#if defined(WIN32) && defined(NDEBUG)
#define main WinMain
#endif

int main(int argc, char *argv[])
{
#if 1
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load("lang/lang_zh.qm");
    qApp->installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
#endif
    FILE* fp = fopen("animal_256x256.rgb24", "rb");

    int len = 256*256*3;
    unsigned char* rgb = (unsigned char*)malloc(len);
    float* rgb_float = (float*)malloc(len* sizeof(float));
    fread(rgb, len, 1, fp);
    fclose(fp);

    for (int i = 0; i < len; ++i)
    {
        rgb_float[i] = rgb[i];
    }
    fp = fopen("animal_256x256.rgb24_float", "wb");
    fwrite(rgb_float, len*sizeof(float), 1, fp);
    fclose(fp);
}
