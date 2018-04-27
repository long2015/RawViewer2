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
    ui->label->setGeometry(0, 0, 0, 0);

    //create state bar
    createStateBar();
    createToolBar();
    createRightMenu();

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
    //ui->label->resize(this->size());
    //ui->label->setPixmap(QPixmap::fromImage(m_image));
}

frame::~frame()
{
    delete ui;
}

void frame::scaledImage(QSize size)
{
    QImage image = m_image.scaled(size);
    ui->label->resize(size);

    ui->label->setPixmap(QPixmap::fromImage(image));
    char state[128];
    snprintf(state, sizeof(state), "Info: %dx%d, %s, char", m_imageInfo.width, m_imageInfo.height, m_imageInfo.colorstr.c_str());
    m_stateLabel->setText(state);
}

void frame::createStateBar()
{
    m_stateLabel = new QLabel();

//    m_stateLabel->setMaximumSize(m_stateLabel->sizeHint());
//    m_stateLabel->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(m_stateLabel);
    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
}

void frame::createToolBar()
{
    QAction* origAction= new QAction("原始大小");
    QAction* winAction= new QAction("适合窗口");

    QToolBar* toolBar = addToolBar(tr("&File"));
    toolBar->addAction(origAction);
    toolBar->addAction(winAction);

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
