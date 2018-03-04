#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDir>
#include <QFileInfoList>
#include <QLabel>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QPixmap>
#include <QRect>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void showImg();

    int getIndex(QString string);

protected:
    void resizeEvent(QResizeEvent * event);

private slots:
    void on_closeWin_action_triggered();

    void on_open_action_triggered();

    void fileChanged(const QModelIndex &index);

    void on_closePic_action_triggered();

    void on_left_action_triggered();

    void on_right_action_triggered();

    void on_turn_to_left_triggered();

    void on_turn_to_right_triggered();

    void on_to_big_triggered();

    void on_to_small_triggered();

    void on_reduction_action_triggered();

private:
    Ui::MainWindow *ui;

    //用来判断第几张
    int i;

    //当前路径和文件名称
    QString curPath;
    QString curFileName;

    QDir curDir;
    QFileInfoList fileInfoList;
    //给listview使用的过滤器
    QFileSystemModel *model;
    QPixmap pix;

    //旋转角度
    int circle;
    //放大系数
    double big;
    //屏幕尺寸
    QRect screenRect;
    //状态栏标签
    QLabel *tmp;
    QLabel *perName;
    QLabel *perSize;

};

#endif // MAINWINDOW_H
