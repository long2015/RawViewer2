#include "ImageWindow.h"
#include "ui_frame.h"
#include <assert.h>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QtWidgets/QSlider>
#include <QLayout>
#include "RawImage.h"


extern "C"
{
#include "libavutil/pixfmt.h"
}

CImageWindow::CImageWindow(QString filename, QWidget *parent) :
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

    m_image = IImage::createImage(filename.toStdString());
    if( m_image == NULL )
    {
        //dialog to get width, height, color
        int width = 0;
        int height = 0;
        int color = 0;
        m_image = IImage::createImage(filename.toStdString(), width, height, color);
    }

    // load file
    if( !m_image->open() )
    {
        qDebug("open failed. filename:%s\n",filename.toStdString().c_str());
        return;
    }
    m_image->getFrameInfo(m_frameInfo);
    m_frameCnt = m_image->getFrameCount();
    m_frameId = 0;

    m_image->getFrame(m_frameId, m_frame);
    m_QImage = QImage((uchar*)m_frame.getRGBData(), m_frameInfo.width, m_frameInfo.height, QImage::Format_RGB888);

    m_slider->setRange(0, m_frameCnt-1);
    m_slider->setValue(m_frameId);

    resize(420, 420);
}

CImageWindow::~CImageWindow()
{
    if( m_image )
    {
        m_image->close();
        delete m_image;
        m_image = NULL;
    }
    delete ui;
}

void CImageWindow::scaledImage(QSize size)
{
    int height = ui->toolBar->height() + statusBar()->height();
    size.setHeight(size.height()- height);

    QImage image = m_QImage.scaled(size);
    ui->label->resize(size);

    ui->label->setPixmap(QPixmap::fromImage(image));
    char buf[128];
    sprintf(buf, "%d", m_frameInfo.width);
    m_labelWidth->setText(buf);
    sprintf(buf, "%d", m_frameInfo.height);
    m_labelHeight->setText(buf);
    m_labelColor->setText(toColorStr(m_frameInfo.color).c_str());

    sprintf(buf, "%d", m_frameId);
    m_labelFrameId->setText(buf);
    sprintf(buf, "%d", m_frameCnt);
    m_labelFrameCnt->setText(buf);
}

void CImageWindow::createStateBar()
{
    m_labelWidth = new QLabel();
    m_labelHeight = new QLabel();
    m_labelColor = new QLabel();

    m_labelFrameId = new QLabel();
    m_labelFrameCnt = new QLabel();

    statusBar()->addWidget(m_labelWidth);
    statusBar()->addWidget(m_labelHeight);
    statusBar()->addWidget(m_labelColor);

    statusBar()->addPermanentWidget(m_labelFrameId, 0);
    statusBar()->addPermanentWidget(m_labelFrameCnt, 0);

    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 2px}"));
}

void CImageWindow::sliderChanged(int value)
{
    printf("slider:%d %d\n", m_slider->value(), value);
    m_frameId = value;

    m_image->getFrame(m_frameId, m_frame);
    m_QImage = QImage((uchar*)m_frame.getRGBData(), m_frameInfo.width, m_frameInfo.height, QImage::Format_RGB888);
    scaledImage(size());
}
void CImageWindow::createToolBar()
{
    m_slider = new QSlider(Qt::Horizontal);
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

    ui->toolBar->addWidget(m_slider);

    ui->toolBar->setIconSize(QSize(18, 18));
    ui->toolBar->setMovable(false);
}

void CImageWindow::createRightMenu()
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

void CImageWindow::contextMenuEvent(QContextMenuEvent *event)
{
    int toolHeight = ui->toolBar->height();
    int x = event->x();
    int y = event->y();

    if( y > toolHeight && y < ui->label->height() + toolHeight &&
        x > 0 && x < ui->label->width() )
        m_popMenu->exec(event->globalPos());
}

void CImageWindow::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    scaledImage(size);
}

void CImageWindow::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Space )
    {

    }
}
