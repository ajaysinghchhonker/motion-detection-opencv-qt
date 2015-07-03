#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
//#include <vector>
#include <QTimer>
#include <QLabel>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    VideoCapture *cap;
    Mat matFrame,matBack,matFore,matFramePre,matOriginal,matMask;
    BackgroundSubtractorMOG2 mog2;
    vector<vector<Point> > contours;
    QTimer *videoTimer;

public slots:
    void processData();

private slots:
    void on_btnStartStop_pressed();

private:
    Ui::MainWindow *ui;
    void initSettings();
    void showImage(Mat img, QLabel *lbl);
    string status;
    unsigned int minArea;
    Size sizeImg;
};

#endif // MAINWINDOW_H
