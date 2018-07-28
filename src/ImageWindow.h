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
    enum
    {
        TOOLBAR_HEIGHT = 33,
        STATEBAR_HEIGHT = 23,
    };
    enum
    {
        VIEW_MODE,
        CROP_MODE
    };
public slots:
    void sliderChanged(int value);
    void enterCrop();
    void exitCrop();
    void copyArea();

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *event);
    void paintEvent(QPaintEvent* event);

private:
    void scaledImage(QSize size);
    void createToolBar();
    void createStateBar();
    void createRightMenu();

private:
    Ui::frame *ui;

    QLabel*      m_labelWidth;
    QLabel*      m_labelHeight;
    QLabel*      m_labelColor;

    QSlider*     m_slider;
    QLabel*      m_labelFrameCnt;
    QLabel*      m_labelFrameId;

    QMenu*       m_popMenu;
    QMenu*       m_cutAreaMenu;

    IImageFile*     m_image;
    FrameInfo   m_frameInfo;
    int         m_frameCnt;
    int         m_frameId;
    CFrame      m_frame;
    CFrame      m_RGBFrame;

    QPixmap     m_origPixmap;
    QPixmap     m_scaledPixmap;

    QPoint      m_startPoint;
    QPoint      m_endPoint;

    //
    int         m_mode;
};

#endif // FRAME_H
