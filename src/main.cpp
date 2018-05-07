#include "mainwindow.h"
#include <QApplication>

#if defined(WIN32) && defined(NDEBUG)
//#define main WinMain
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
