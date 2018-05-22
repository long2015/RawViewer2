#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ImageWindow.h"
#include <QMimeData>
#include <QDebug>
#include <QtWidgets/QMdiSubWindow>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/logo.png"));
    setWindowTitle("RawViewer2");
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
