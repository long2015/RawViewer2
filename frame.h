#ifndef FRAME_H
#define FRAME_H

#include <QMainWindow>
#include <QResizeEvent>
#include "rawfile.h"
#include <QKeyEvent>

namespace Ui {
class frame;
}

class frame : public QMainWindow
{
    Q_OBJECT

public:
    explicit frame(QString filename, QWidget *parent = 0);
    ~frame();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void scaledImage(QSize size);

private:
    Ui::frame *ui;

    CRawFile    m_rawFile;
    ImageInfo   m_imageInfo;

    QPixmap      m_pixmap;
    QImage       m_image;
};

#endif // FRAME_H
