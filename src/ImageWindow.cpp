#include "ImageWindow.h"
#include "ui_frame.h"
#include <assert.h>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>
#include <QtWidgets/QSlider>
#include <QLayout>
#include <QtWidgets/QStyle>
#include <QPainter>
#include <QIcon>
#include <QLineEdit>
#include <QtGui/QIntValidator>
#include <QApplication>
#include <QClipboard>
#include "RawImage.h"

CImageWindow::CImageWindow(QString filename, QWidget *parent) :
    QMainWindow(parent), m_mode(VIEW_MODE),
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

    m_image = IImageFile::createImage(filename.toStdString());
    if( m_image == NULL )
    {
        qDebug("createImage failed. filename:%s\n",filename.toStdString().c_str());
        return;
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
    m_RGBFrame = CFrame(m_frameInfo.width, m_frameInfo.height, RV_COLOR_SPACE_RGB24);
    cvtColor(m_frame, m_RGBFrame);

    QImage image = QImage((uchar*)m_RGBFrame.getData(), m_frameInfo.width, m_frameInfo.height, QImage::Format_RGB888);
    m_origPixmap = QPixmap::fromImage(image);

    m_slider->setRange(0, m_frameCnt-1);
    m_slider->setValue(m_frameId);

    int width = m_frameInfo.width;
    int height = TOOLBAR_HEIGHT + STATEBAR_HEIGHT + m_frameInfo.height;
    resize(width, height);
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
    size.setHeight(size.height() - TOOLBAR_HEIGHT - STATEBAR_HEIGHT );
//    qDebug("toolbar height:%d, height:%d\n", ui->toolBar->height(), statusBar()->height());

    m_scaledPixmap = m_origPixmap.scaled(size);
    m_endPoint = m_startPoint;

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
    m_frameId = value;

    m_image->getFrame(m_frameId, m_frame);
    cvtColor(m_frame, m_RGBFrame);
    QImage image = QImage((uchar*)m_RGBFrame.getData(), m_frameInfo.width, m_frameInfo.height, QImage::Format_RGB888);
    m_origPixmap = QPixmap::fromImage(image);
    scaledImage(size());

    update();
}

void CImageWindow::createToolBar()
{
    QAction* playInAction = new QAction(tr("play"), this);
    playInAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->toolBar->addAction(playInAction);

    m_slider = new QSlider(Qt::Horizontal);
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

    ui->toolBar->addWidget(m_slider);

    ui->toolBar->setIconSize(QSize(18, 18));
    ui->toolBar->setMovable(false);
}

void CImageWindow::createRightMenu()
{
    QAction* picInfoAction = new QAction(tr("Pic Info"), this);
    QAction* cropAction = new QAction(tr("Crop"), this);
    m_popMenu = new QMenu();
    m_popMenu->addAction(picInfoAction);
    m_popMenu->addAction(cropAction);
    connect(picInfoAction, SIGNAL(triggered()), this, SLOT(showPicInfo()));
    connect(cropAction, SIGNAL(triggered()), this, SLOT(enterCrop()));

    QAction* copyAreaInfoAction = new QAction(tr("Copy Area Info"), this);
    QAction* exportPictureAction = new QAction(tr("Export to Picture"), this);
    QAction* exportVideoAction = new QAction(tr("Export to Video"), this);
    QAction* exitCropAction = new QAction(tr("Exit Crop"), this);

    m_cutAreaMenu = new QMenu();
    m_cutAreaMenu->addAction(copyAreaInfoAction);
    m_cutAreaMenu->addAction(exportPictureAction);
    m_cutAreaMenu->addAction(exportVideoAction);
    m_cutAreaMenu->addAction(exitCropAction);

    connect(copyAreaInfoAction, SIGNAL(triggered()), this, SLOT(copyArea()));
    connect(exitCropAction, SIGNAL(triggered()), this, SLOT(exitCrop()));
}
void CImageWindow::enterCrop()
{
    m_mode = CROP_MODE;
    update();
}
void CImageWindow::exitCrop()
{
    m_mode = VIEW_MODE;

    m_startPoint = {-1, -1};
    m_endPoint = m_startPoint;
    update();
}
void CImageWindow::copyArea()
{
    qDebug("[%s]\n", __FUNCTION__);
    char area[32] = {0};
    QPoint leftTop = m_startPoint - QPoint(0, TOOLBAR_HEIGHT);
    QPoint rightBottom = m_endPoint - QPoint(0, TOOLBAR_HEIGHT);
    //
    float scaledX = (float)m_origPixmap.width()/m_scaledPixmap.width();
    float scaledY = (float)m_origPixmap.height()/m_scaledPixmap.height();
    leftTop.setX( leftTop.x()*scaledX );
    leftTop.setY( leftTop.y()*scaledY );
    rightBottom.setX( rightBottom.x()*scaledX );
    rightBottom.setY( rightBottom.y()*scaledY );

    sprintf(area, "(%d,%d)(%d,%d)", leftTop.x(), leftTop.y(), rightBottom.x(), rightBottom.y());
    QApplication::clipboard()->setText(area);
}

void CImageWindow::contextMenuEvent(QContextMenuEvent *event)
{
    int toolHeight = ui->toolBar->height();
    int x = event->x();
    int y = event->y();

    if( y > toolHeight && y < height() - toolHeight &&
        x > 0 && x < width() )
        if( m_mode == VIEW_MODE )
        {
            m_popMenu->exec(event->globalPos());
        }
        else
        {
            m_cutAreaMenu->exec(event->globalPos());
        }
}

void CImageWindow::resizeEvent(QResizeEvent *event)
{
    scaledImage(event->size());
}

void CImageWindow::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Space )
    {

    }
}

void CImageWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::green, 1, Qt::SolidLine));

    painter.drawPixmap(0,TOOLBAR_HEIGHT, m_scaledPixmap);  //»­Ä£ºý±³¾°
    if( m_startPoint != m_endPoint )
    {
        painter.drawRect(QRect(m_startPoint, m_endPoint));
    }
}

void CImageWindow::mousePressEvent(QMouseEvent *event)
{
    if( m_mode != CROP_MODE )
        return;

    if( event->buttons() & Qt::LeftButton )
    {
        m_startPoint = event->pos();
        m_endPoint = m_startPoint;
        update();
    }
}

void CImageWindow::mouseMoveEvent(QMouseEvent *event)
{
    if( m_mode != CROP_MODE )
        return;

    if( event->buttons() == Qt::LeftButton )
    {
        m_endPoint = event->pos();
//        qDebug("<%d,%d> width:%d height:%d\n", event->x(), event->y(), m_endPoint.x()-m_startPoint.x(), m_endPoint.y()-m_startPoint.y());

        update();
    }
}

void CImageWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if( m_mode != CROP_MODE )
        return;
}
