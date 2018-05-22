#ifndef FRAME_H
#define FRAME_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QLabel>
#include <QAction>
#include <QtWidgets/QSlider>
#include "image.h"


namespace Ui {
class frame;
}

class CImageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CImageWindow(QString filename, QWidget *parent = 0);
    ~CImageWindow();

public slots:
    void sliderChanged(int value);

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void scaledImage(QSize size);
    void createToolBar();
    void createStateBar();
    void createRightMenu();

private:
    Ui::frame *ui;

    IImage*     m_image;
    FrameInfo   m_frameInfo;
    int         m_frameCnt;
    int         m_frameId;
    CFrame      m_frame;

    QPixmap      m_pixmap;
    QImage       m_QImage;

    QLabel*      m_labelWidth;
    QLabel*      m_labelHeight;
    QLabel*      m_labelColor;

    QSlider*     m_slider;
    QLabel*      m_labelFrameCnt;
    QLabel*      m_labelFrameId;

    QMenu*       m_popMenu;
};

#endif // FRAME_H
