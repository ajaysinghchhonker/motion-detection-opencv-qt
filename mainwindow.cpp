#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::initSettings() {
    //mog2.nmixtures=3;
    //mog2.bShadowDetection=false;
    minArea=400;
    status="No Motion";
    sizeImg = Size(ui->lblOriginal->width(),ui->lblOriginal->height());
    //string videoPath="/home/ajay/image_processing_c/NumberPlate/MOV_0647.mp4";
    cap=new VideoCapture(1);
    videoTimer=new QTimer(this);
    connect(videoTimer,SIGNAL(timeout()),this,SLOT(processData()));
    if(cap->read(matFramePre)) {
        cv::resize(matFramePre,matFramePre,sizeImg);
        cvtColor(matFramePre,matFramePre,CV_RGB2GRAY);
        blur(matFrame,matFrame,Size(5,5));
        cv::flip(matFrame,matFrame,1);
    }
    //matMask=Mat(sizeImg,CV_8UC3);
    //cv::rectangle(matMask,Rect(0,0,matMask.cols,matMask.rows),Scalar(100,100,0),-1);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initSettings();

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::processData() {
    if(!cap->read(matFrame)) {
        cout<<"Unable to capture frame!!!"<<endl;
        return;
    }
    if(matFrame.empty()) {
        cout<<"Empty frame!!!"<<endl;
        return;
    }
    cv::resize(matFrame,matFrame,sizeImg);
    //cv::resize(matMask,matMask,sizeImg);
    cv::flip(matFrame,matFrame,1);
    matOriginal=matFrame.clone();

    //MOG2 bkg subtractor algo
    mog2.operator ()(matFrame,matFore);
    mog2.getBackgroundImage(matBack);
    cv::erode(matFore,matFore,cv::Mat());
    cv::dilate(matFore,matFore,cv::Mat());

    /*
    //ABS difference algo (basic)
    cvtColor(matFrame,matFrame,CV_RGB2GRAY);
    blur(matFrame,matFrame,Size(5,5));

    cv::absdiff(matFramePre,matFrame,matFore);
    cv::threshold(matFore,matFore,40,255,0);

    Mat element=getStructuringElement(MORPH_ELLIPSE,Size(10,10));
    morphologyEx(matFore,matFore,MORPH_DILATE,element);
    morphologyEx(matFore,matFore,MORPH_CLOSE,element);
    matBack=matFore.clone();
    */
    findContours(matFore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    vector<vector<Point> > ::iterator itc = contours.begin();

    //approxPolyDP(contours,contours,20,true);

    cout<<"No of contours : "<<contours.size()<<endl;
    //Vector<RotatedRect> rects;
    status="No Motion";
    while(itc!=contours.end()) {
        //RotatedRectRot tempRect=minAreaRect(Mat(*itc));
        if(contourArea(*itc)>minArea) {
            status="ALERT!!!";
            Rect tempRect = boundingRect((Mat(*itc)));
            rectangle(matOriginal,tempRect,Scalar(0,0,255));
        }
        ++itc;
    }
    //cv::addWeighted(matMask,0.6,matOriginal,0.4,0.0,matOriginal);
    putText(matOriginal,"Status : " + status,Point(10,30),1,2,Scalar(0,0,255),2);
    showImage(matOriginal,ui->lblOriginal);
    showImage(matFore,ui->lblProcessed);
    matFramePre=matFrame.clone();
}

void MainWindow::on_btnStartStop_pressed()
{
    if(videoTimer->isActive()) {
        videoTimer->stop();
        ui->btnStartStop->setText("START CAMERA");
    }
    else {
        videoTimer->start(40);
        ui->btnStartStop->setText("STOP CAMERA");
    }
}

void MainWindow::showImage(Mat img, QLabel *lbl) {
    if(img.empty()) {
        cout<<"Empty image to show!!!"<<endl;
        return;
    }
    QImage Qimgbkg;
    if(img.type()==CV_8UC3) {
        cvtColor(img,img,CV_BGR2RGB);
        QImage Qimgbkg1((uchar*)img.data,img.cols,img.rows,img.step,QImage::Format_RGB888);
        Qimgbkg = Qimgbkg1;
    }
    else if(img.type()==CV_8UC4) {
        cvtColor(img,img,CV_BGRA2RGBA);
        QImage Qimgbkg1((uchar*)img.data,img.cols,img.rows,img.step,QImage::Format_RGBA8888);
        Qimgbkg = Qimgbkg1;
    }
    else {
        QImage Qimgbkg1((uchar*)img.data,img.cols,img.rows,img.step,QImage::Format_Indexed8);
        Qimgbkg=Qimgbkg1;
    }

    lbl->setPixmap(QPixmap::fromImage(Qimgbkg));
}
