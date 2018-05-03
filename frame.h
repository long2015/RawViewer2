#ifndef FRAME_H
#define FRAME_H

#include <QMainWindow>
#include <QResizeEvent>
#include "rawfile.h"
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QLabel>
#include <QAction>

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
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void scaledImage(QSize size);
    void createToolBar();
    void createStateBar();
    void createRightMenu();

private:
    Ui::frame *ui;

    CRawFile    m_rawFile;
    ImageInfo   m_imageInfo;

    QPixmap      m_pixmap;
    QImage       m_image;

    QLabel*      m_labelWidth;
    QLabel*      m_labelHeight;
    QLabel*      m_labelColor;

    QLabel*      m_labelFrame;
    QLabel*      m_labelFrameTotal;

    QMenu*       m_popMenu;
};

#endif // FRAME_H
