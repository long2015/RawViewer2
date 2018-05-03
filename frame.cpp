#include "frame.h"
#include "ui_frame.h"
#include <assert.h>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>

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

    resize(m_imageInfo.width, m_imageInfo.height);
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

    m_labelFrame->setText("0");
    m_labelFrameTotal->setText("1");
}

void frame::createStateBar()
{
    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 2px}"));

    m_labelWidth = new QLabel();
    m_labelHeight = new QLabel();
    m_labelColor = new QLabel();
    statusBar()->addWidget(m_labelWidth);
    statusBar()->addWidget(m_labelHeight);
    statusBar()->addWidget(m_labelColor);

    m_labelFrame = new QLabel();
    m_labelFrameTotal = new QLabel();
    statusBar()->addPermanentWidget(m_labelFrame);
    statusBar()->addPermanentWidget(m_labelFrameTotal);
}

void frame::createToolBar()
{
    QSlider* slider = new QSlider(Qt::Horizontal);
    ui->toolBar->addWidget(slider);
}

void frame::createRightMenu()
{
    QAction*     m_RGBAction;
    QAction*     m_BGRAction;
    QAction*     m_NV21Action;

    m_RGBAction = new QAction(tr("RGB"), this);  //创建新的菜单项
    m_RGBAction->setCheckable(true);//设置可选
    m_RGBAction->setChecked(true);//设置是否选中

    //connect(firstChannel, SIGNAL(triggered()), this, SLOT(firstChannelSlot()));       //该菜单项的连接信号和槽
    m_BGRAction = new QAction(tr("BGR"), this);  //创建新的菜单项

    m_NV21Action = new QAction(tr("NV21"), this);  //创建新的菜单项

    m_popMenu = new QMenu();
    m_popMenu->addAction(m_RGBAction);
    m_popMenu->addAction(m_BGRAction);
    m_popMenu->addAction(m_NV21Action);

    m_popMenu->addSeparator();
    m_popMenu->addMenu("&More");

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
        //int color = AV_PIX_FMT_RGB24；
        m_rawFile.Reload(0);

        m_image = QImage(m_imageInfo.width, m_imageInfo.height, QImage::Format_RGB888);
        memcpy(m_image.bits(), m_rawFile.GetFrame(), m_rawFile.GetFrameLen());

        scaledImage(this->size());
    }
}
