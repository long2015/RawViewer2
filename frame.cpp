#include "frame.h"
#include "ui_frame.h"
#include <assert.h>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QtWidgets/QSlider>
#include <QLayout>


extern "C"
{
#include "libavutil/pixfmt.h"
}

frame::frame(QString filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frame)
{
    //UI
    ui->setupUi(this);
    QString name = filename.split("/").back();
    setWindowTitle(name);

    //create state bar
    createStateBar();
    createToolBar();
    createRightMenu();

    int height = ui->toolBar->height() + statusBar()->height();
    setMinimumSize(320, 320+height);

    // load file
    if( !m_rawFile.Open(filename.toStdString()) )
    {
        qDebug("open failed. filename:%s\n",filename.toStdString().c_str());
        return;
    }
    m_rawFile.GetImageInfo(m_imageInfo);

    m_image = QImage(m_imageInfo.width, m_imageInfo.height, QImage::Format_RGB888);
    memcpy(m_image.bits(), m_rawFile.GetFrame(), m_rawFile.GetFrameLen());

    resize(420, 420);
}

frame::~frame()
{
    delete ui;
}

void frame::scaledImage(QSize size)
{
    int height = ui->toolBar->height() + statusBar()->height();
    size.setHeight(size.height()- height);

    QImage image = m_image.scaled(size);
    ui->label->resize(size);

    ui->label->setPixmap(QPixmap::fromImage(image));
    char buf[128];
    sprintf(buf, "%d", m_imageInfo.width);
    m_labelWidth->setText(buf);
    sprintf(buf, "%d", m_imageInfo.height);
    m_labelHeight->setText(buf);
    m_labelColor->setText(m_imageInfo.colorstr.c_str());
}

void frame::createStateBar()
{
    m_labelWidth = new QLabel();
    m_labelHeight = new QLabel();
    m_labelColor = new QLabel();
    statusBar()->addWidget(m_labelWidth);
    statusBar()->addWidget(m_labelHeight);
    statusBar()->addWidget(m_labelColor);

    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 2px}"));
}

void frame::createToolBar()
{
    QSlider* slider = new QSlider(Qt::Horizontal);
    QLabel* frame = new QLabel("0/1");
    ui->toolBar->addWidget(slider);
    ui->toolBar->addWidget(frame);
    slider->setMaximumWidth(250);
    ui->toolBar->addSeparator();

    ui->toolBar->setIconSize(QSize(18, 18));
}

void frame::createRightMenu()
{
    QAction* zoomInAction = new QAction(tr("Zoom In"), this);
    QAction* zoomOutAction = new QAction(tr("Zoom Out"), this);
    QAction* zoomMinAction = new QAction(tr("Min Size"), this);
    QAction* zoomOrigAction = new QAction(tr("Orig Size"), this);
    QAction* zoomFullAction = new QAction(tr("Full"), this);

    QAction* infoAction = new QAction(tr("Info"), this);
    QAction* moreAction = new QAction(tr("More"), this);
    m_popMenu = new QMenu();
    QMenu* zoomMenu = m_popMenu->addMenu("Zoom");
    zoomMenu->addAction(zoomInAction);
    zoomMenu->addAction(zoomOutAction);
    zoomMenu->addAction(zoomMinAction);
    zoomMenu->addAction(zoomOrigAction);
    zoomMenu->addAction(zoomFullAction);

    m_popMenu->addAction(infoAction);
    m_popMenu->addSeparator();
    m_popMenu->addAction(moreAction);
}

void frame::contextMenuEvent(QContextMenuEvent *event)
{
    m_popMenu->exec(event->globalPos());
}

void frame::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    scaledImage(size);
}

void frame::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Space )
    {
        //int color = AV_PIX_FMT_RGB24£»
        m_rawFile.Reload(0);

        m_image = QImage(m_imageInfo.width, m_imageInfo.height, QImage::Format_RGB888);
        memcpy(m_image.bits(), m_rawFile.GetFrame(), m_rawFile.GetFrameLen());

        scaledImage(this->size());
    }
}
