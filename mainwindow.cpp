#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "frame.h"
#include <QMimeData>
#include <QDebug>
#include <QtWidgets/QMdiSubWindow>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("RawViewer 2");
    resize(1100, 800);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString filename = event->mimeData()->urls()[0].toLocalFile();
    qDebug("filename:%s\n", filename.toStdString().c_str());

    frame* pFrame = new frame(filename, this);
    QMdiSubWindow* subWindow = ui->mdiArea->addSubWindow(pFrame);
//    if( pFrame->size() < QSize(320, 320) )
    subWindow->resize(pFrame->size());
    subWindow->show();
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
