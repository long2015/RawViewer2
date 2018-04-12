#include "frame.h"
#include "ui_frame.h"
#include <assert.h>

frame::frame(QString filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frame)
{
    ui->setupUi(this);
    ui->label->setGeometry(0, 0, 0, 0);

    if( !m_rawFile.Open(filename.toStdString()) )
    {
        qDebug("open failed. filename:%s\n",filename.toStdString().c_str());
        return;
    }
    m_rawFile.GetImageInfo(m_imageInfo);

    m_image = QImage(m_imageInfo.width, m_imageInfo.height, QImage::Format_RGB888);
    memcpy(m_image.bits(), m_rawFile.GetFrame(), m_rawFile.GetFrameLen());

    resize(m_imageInfo.width, m_imageInfo.height);
    ui->label->resize(this->size());
    ui->label->setPixmap(QPixmap::fromImage(m_image));
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
        m_rawFile.Reload(0);

        m_image = QImage(m_imageInfo.width, m_imageInfo.height, QImage::Format_RGB888);
        memcpy(m_image.bits(), m_rawFile.GetFrame(), m_rawFile.GetFrameLen());

        scaledImage(this->size());
    }
}