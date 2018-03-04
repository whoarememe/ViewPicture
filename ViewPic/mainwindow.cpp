#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMatrix>
#include <QDebug>
#include <QMovie>
#include <QString>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), curPath("./"), curDir("./"),
    i(0), model(new QFileSystemModel), circle(0),
    big(1.0)
{
    ui->setupUi(this);

    //获得屏幕尺寸
    QDesktopWidget* desktopWidget = QApplication::desktop();
    screenRect = desktopWidget->screenGeometry();

    //过滤器
    QStringList filterList;
    filterList << "*.gif" << "*.jpg" << "*.png" << "*.bmp";

    //过滤掉其他东西，除了文件
    curDir.setFilter(QDir::Files);
    curDir.setNameFilters(filterList);
    fileInfoList = curDir.entryInfoList();
    if ( !fileInfoList.isEmpty() ) {
        curFileName = fileInfoList.at(0).fileName();
    }


    model->setNameFilterDisables(false);
    model->setFilter(QDir::Dirs|QDir::Drives|QDir::Files|QDir::NoDotAndDotDot);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setGridSize(QSize(100,100));
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setModel(model);

    model->setNameFilters(filterList);
    ui->listView->setRootIndex(model->setRootPath(curPath));

    //居中显示setAlignment(Qt::AlignCenter)
    ui->label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    //将label装入scrollarea
    //ui->scrollArea->setWidget(ui->label);

    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(fileChanged(QModelIndex)));

    //状态栏添加标签
    tmp = new QLabel;
    perName = new QLabel;
    perSize = new QLabel;

    perName->setFrameStyle(QFrame::Box | QFrame::Sunken);
    perSize->setFrameStyle(QFrame::Box | QFrame::Sunken);
    tmp->setFrameStyle(QFrame::Box | QFrame::Sunken);
    tmp->setText("简易图片浏览器");
    if ( curFileName == NULL) {
        perName->setText("请打开图片");
        perSize->setText("0 X 0");
    } else {
        perName->setText(curFileName);
        perSize->setText(QString::number(QPixmap(curPath + "/" + curFileName).width()) + " X " +
                         QString::number(QPixmap(curPath + "/" + curFileName).height()));
    }
    ui->statusBar->addWidget(tmp);
    ui->statusBar->addPermanentWidget(perName);
    ui->statusBar->addPermanentWidget(perSize);

    showImg();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImg()
{
    QStringList style = curFileName.split(".");

    QPixmap fitpixmap;
    double bili;    //图片比例因子

    QMatrix matrix;
    matrix.rotate(circle);

    if (curFileName == "") {
        return;
    } else if (style.last().toLower() == "gif") {
        QMovie *movie = new QMovie(curPath + "/" + curFileName);
        ui->label->resize(big*300, big*300);
        ui->label->move((ui->widget->width() - ui->label->width()) / 2,
                        (ui->widget->height() - ui->label->height()) / 2);
        ui->label->setMovie(movie);
        movie->start();

    } else {
//        pix.load(curPath + "/" + curFileName);
//        pix.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//        ui->label->setPixmap(pix);
        pix.load(curPath + "/" + curFileName);

        bili = (double)pix.width()/pix.height();

        if (pix.width() <= ui->widget->width() &&
                pix.height() <= ui->widget->height()) {
            fitpixmap=pix.scaled(big*pix.width(), big*pix.height(),
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        } else if (pix.width() > ui->widget->width() &&
                   pix.height() <= ui->widget->height()) {
            //图片自适应widget宽度
            fitpixmap=pix.scaled(big*ui->widget->width(),
                                 big*ui->widget->width()/bili,
                                 Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        } else if (pix.width() <= ui->widget->width() &&
                   pix.height() > ui->widget->height()) {
            //图片自适应height高度
            fitpixmap=pix.scaled(big*ui->widget->height()*bili,
                                         big*ui->widget->height(),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        } else if (pix.width() < pix.height()) {
            fitpixmap=pix.scaled(big*ui->widget->height()*bili,
                                         big*ui->widget->height(),
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        } else if (pix.width() > pix.height()) {
            fitpixmap=pix.scaled(big*ui->widget->width(),
                                         big*ui->widget->width()/bili,
                                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        }
        //图片旋转
        //pix.transformed(matrix, Qt::SmoothTransformation);


        ui->label->resize(fitpixmap.width(), fitpixmap.height());
        ui->label->move((ui->widget->width() - ui->label->width()) / 2,
                        (ui->widget->height() - ui->label->height()) / 2);
        fitpixmap.transformed(matrix, Qt::SmoothTransformation);
        ui->label->setPixmap(fitpixmap);

        perName->setText(curFileName);
        perSize->setText(QString::number(QPixmap(curPath + "/" + curFileName).width()) + " X " +
                         QString::number(QPixmap(curPath + "/" + curFileName).height()));
    }
}

int MainWindow::getIndex(QString string)
{
    if (fileInfoList.size() <= 0) {
        return 0;
    }
    for (int k = 0; k < fileInfoList.size(); k++) {
        if (fileInfoList.at(k).fileName() == string) {
            return k;
        }
    }

    return 0;
}

//重写窗口改变事件
void MainWindow::resizeEvent(QResizeEvent *event)
{
    showImg();
}

void MainWindow::fileChanged(const QModelIndex &index)
{
    //获得文件名
    curFileName = index.data(Qt::DisplayRole).toString();
    i = getIndex(curFileName);

    showImg();
}

void MainWindow::on_closeWin_action_triggered()
{
    this->close();
}


/*
 * 通过自动关联，当点击打开文件的时候会自动打开filedialog，
 * 然后用户选择一张图片，同时我们会获取此图片所在的绝对路径curPath
 * 以及图片的名称fileName，然后我们会搜索当前路径下的所有图片文件
 *
 */
void MainWindow::on_open_action_triggered()
{
    //获取图片的路径
    curFileName = QFileDialog::getOpenFileName(this, tr("打开图片"), curPath,
                                             tr("图片文件(*.png *.jpg *.bmp *.gif)"));

    if (curFileName == NULL) {
        return;
    }

    //将图片的相关路径信息存储到fileInfo中
    QFileInfo fileInfo = QFileInfo(curFileName);
    //将图片名称赋给filename
    curFileName = fileInfo.fileName();
    //将图片路径赋给curpath
    curPath = fileInfo.absolutePath();
    //并重新设置路径
    curDir.setPath(curPath);
    //图片信息
    fileInfoList = curDir.entryInfoList();
    //获得图片位置
    i = getIndex(curFileName);

    ui->listView->setRootIndex(model->setRootPath(curPath));

    showImg();
}

void MainWindow::on_closePic_action_triggered()
{
    ui->listView->setRootIndex(model->setRootPath(NULL));
    pix.load(":/pic/background.png");
    //showImg();
    ui->label->resize(pix.width(), pix.height());
    ui->label->move((ui->widget->width() - ui->label->width()) / 2,
                    (ui->widget->height() - ui->label->height()) / 2);
    ui->label->setPixmap(pix);

    fileInfoList.clear();
    perName->setText("请打开图片");
    perSize->setText("0 X 0");
}

void MainWindow::on_left_action_triggered()
{
    if (fileInfoList.isEmpty()) {
        return;
    }

    if (i == 0) {
        i = fileInfoList.size()-1;
    } else {
        i--;
    }

    curFileName = fileInfoList.at(i).fileName();
    showImg();
}

void MainWindow::on_right_action_triggered()
{
    if (fileInfoList.isEmpty()) {
        return;
    }

    if (i == fileInfoList.size()-1) {
        i = 0;
    } else {
        i++;
    }

    curFileName = fileInfoList.at(i).fileName();
    showImg();
}

void MainWindow::on_turn_to_left_triggered()
{
    if ( circle == -270 ) {
        circle = 0;
    } else {
        circle -= 90;
    }

    showImg();
}

void MainWindow::on_turn_to_right_triggered()
{
    if ( circle == 270 ) {
        circle = 0;
    } else {
        circle += 90;
    }

    showImg();
}

void MainWindow::on_to_big_triggered()
{
    big += 0.05;

    showImg();
}

void MainWindow::on_to_small_triggered()
{
    big -= 0.05;

    showImg();
}

void MainWindow::on_reduction_action_triggered()
{
    big = 1.0;

    showImg();
}
