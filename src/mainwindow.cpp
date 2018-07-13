#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ImageWindow.h"
#include "image.h"
#include <QMimeData>
#include <QDebug>
#include <QtWidgets/QMdiSubWindow>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/logo.png"));
    setWindowTitle("RawViewer2");
    resize(1100, 800);
    setAcceptDrops(true);

#if 1
    QString filename = "/home/long2015/Code/Qt/RawViewer2/data/animal_256x256.bgr24";
//    QString filename = "/home/long2015/Code/Qt/RawViewer2/data/tool.png";
    if( !IImageFile::isSupport(filename.toStdString()) )
    {
        QMessageBox::information(this, tr(""), tr("Quit ?"), QMessageBox::Yes);
    }
    else
    {
        CImageWindow* pFrame = new CImageWindow(filename, this);

        QMdiSubWindow* subWindow = ui->mdiArea->addSubWindow(pFrame);
        //删除系统菜单
        QList<QAction*> actionList = subWindow->systemMenu()->actions();
        for (int i = 0; i < actionList.size(); ++i)
        {
            subWindow->systemMenu()->removeAction(actionList[i]);
        }
        //设置图标
        subWindow->setWindowIcon(QIcon(":/logo.png"));

        int width = pFrame->size().width() + 8;
        int height = pFrame->size().height() + 28;
        subWindow->resize(width, height);
        subWindow->show();
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString filename = event->mimeData()->urls()[0].toLocalFile();
    qDebug("filename:%s\n", filename.toStdString().c_str());

    if( !IImageFile::isSupport(filename.toStdString()) )
    {
        QMessageBox::information(this, tr(""), tr("Open failed. Maybe Not support it."), QMessageBox::Ok);
    }
    else
    {
        CImageWindow* pFrame = new CImageWindow(filename, this);

        QMdiSubWindow* subWindow = ui->mdiArea->addSubWindow(pFrame);
        //删除系统菜单
        QList<QAction*> actionList = subWindow->systemMenu()->actions();
        for (int i = 0; i < actionList.size(); ++i)
        {
            subWindow->systemMenu()->removeAction(actionList[i]);
        }
        //设置图标
        subWindow->setWindowIcon(QIcon(":/logo.png"));

        int width = pFrame->size().width() + 8;
        int height = pFrame->size().height() + 28;
        subWindow->resize(width, height);
        subWindow->show();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug("[%s]\n", __FUNCTION__);
    event->acceptProposedAction();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->mdiArea->resize(event->size());
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if( QMessageBox::question(this, tr(""), tr("Quit ?"), QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes )
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}