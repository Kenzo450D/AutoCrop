//local includes
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AutoCrop.h"

//OpenCV includes
#include <highgui.h>
#include <cvaux.h>
#include <opencv/cv.h>

//Qt includes
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QRect>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //Starting off with reading the image
    ui->progressBar->setValue(0);
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //Starting off with reading the image
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("*.*"));

    if (fileName == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not open File"));
        return;
    }

    ui->lineEdit->setText(fileName);

    IplImage* img = cvLoadImage(fileName.toStdString().c_str());

    if (img==0)
    {
        QMessageBox::critical(this, tr("Error"),tr("Could not load image"));
    }

    int width   = img->width;
    int height  = img->height;
    int step    = img->widthStep;
    int channel = img->nChannels;
    QString info;
    info.append("Width: ");
    info.append(QString::number(width));
    info.append("\nHeight: ");
    info.append(QString::number(height));
    info.append("\nChannels: ");
    info.append(QString::number(channel));
    info.append("\nWidth Step: ");
    info.append(QString::number(step));
    ui->textEdit->setText(info);
    cvReleaseImage(&img);
}

void MainWindow::on_pushButton_2_clicked()
{
    AutoCrop act(this);
    connect(&act, SIGNAL(signalProgress(int)),
                this, SLOT(slotProgress(int)));
    act.setImagePath(ui->lineEdit->text());
    QRect crop = act.autoOuterCrop();
    ui->textEdit->setText(act.output());
    qDebug() << "\nTopLeft : ("<<crop.left()<<","<<crop.top()<<")";
    qDebug() << "\nBottomRight : ("<<crop.right()<<","<<crop.bottom()<<")";
    act.ShowOutput(crop);
    /*
    int height=crop.height();
    int width=crop.width();
    IplImage* img=cvLoadImage(ui->lineEdit->text().toStdString().c_str());
    IplImage* result = cvCreateImage(cvSize(width,height),img->depth,img->nChannels);
    int ni,nj,i,j,topY,bottomY,leftX,rightX;
    topY=crop.top();
    bottomY=crop.bottom();
    leftX=crop.left();
    rightX=crop.right();
    for(i=topY,ni=0;i<=bottomY;i++,ni++)
    {
        for (j=leftX,nj=0;j<=rightX;j++,nj++)
        {
            cvSet2D(result,ni,nj,cvGet2D(img,i,j));
        }
    }
    cvShowImage("Result", result);
    cvReleaseImage(&img);
    cvReleaseImage(&result);
    */
    /**
    IplImage* img=cvLoadImage(ui->lineEdit->text().toStdString().c_str());
    //this is to store the binary image (only for inner crop)
//    CvMat* binaryImg = cvCreateMat(img->height,img->width,CV_8UC1);
    int i,j;
    uint breakflag=0;
    float channel0,channel1, channel2;
    int topi=-1, topj=-1;
    int bottomi=-1, bottomj=-1;
    int lefti=-1,leftj=-1;
    int righti=-1,rightj=-1;
//    float* binaryImgPtr = (float*)binaryImg->data.ptr;
    //----------------------Finding the outer boundaries of the image (i.e. with black portions)

//      This would be done in 4 steps
//      1. Search column wise:
//      (a) From the left to the right, this is to get the left boundary
//      (b) From the right to the left, this is to get the right boundary
//      2. Search row wise :
//      (a) From the top to the bottom, this is to get the top boundary
//      (b) From the bottom to the top, this is to get the bottom boundary




    //1(a) Traversing the image from top to bottom, left to right, to get left boundary
    breakflag=0;
    for(i=0;i<img->width;i++)
    {
        for(j=0;j<img->height;j++)
        {
            channel0=cvGet2D(img,j,i).val[0];
            channel1=cvGet2D(img,j,i).val[1];
            channel2=cvGet2D(img,j,i).val[2];
            if(channel0!=0 || channel1!=0 || channel2!=0)
            {
                //we have found our pixel
                lefti=i;
                leftj=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug("Done Till step 1(a)");

    //1(b) Traversing the image from top to bottom, right to left, to get right boundary
    breakflag=0;
    for(i=(img->width-1);i>=0;i--)
    {
        for(j=0;j<img->height;j++)
        {
            channel0=cvGet2D(img,j,i).val[0];
            channel1=cvGet2D(img,j,i).val[1];
            channel2=cvGet2D(img,j,i).val[2];
            if(channel0!=0 || channel1!=0 || channel2!=0)
            {
                //we have found our pixel
                righti=i;
                rightj=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug()<<"Done Till step 1(b)";
    //2(a) Traversing the image left to right, top to down, to get top boundary
    breakflag=0;
    for(i=0;i<img->height;i++)
    {
        for(j=0;j<img->width;j++)
        {
            //qDebug()<<"Pixel : ("<<i<<","<<j<<")\n";
            channel0=cvGet2D(img,i,j).val[0];
            channel1=cvGet2D(img,i,j).val[1];
            channel2=cvGet2D(img,i,j).val[2];
            if(channel0!=0 || channel1!=0 || channel2!=0)
            {
                //we have found our pixel
                topi=i;
                topj=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug()<<"Done Till step 2(a)";

    //2(b) Traversing the image from left to right, bottom up, to get lower boundary
    breakflag=0;
    for(i=(img->height-1);i>=0;i--)
    {
        for(j=0;j<img->width;j++)
        {
            //qDebug()<<"BPixel : ("<<i<<","<<j<<")\n";
            channel0=cvGet2D(img,i,j).val[0];
            channel1=cvGet2D(img,i,j).val[1];
            channel2=cvGet2D(img,i,j).val[2];
            if(channel0!=0 || channel1!=0 || channel2!=0)
            {
                //we have found our pixel
                bottomi=i;
                bottomj=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }

    qDebug()<<"Done Till step 2(b)";
    QString info;
    info.append("TopMost Pixel : ( ");
    info.append(QString::number(topi));
    info.append(", ");
    info.append(QString::number(topj));
    info.append(")\nBottomMost Pixel : ( ");
    info.append(QString::number(bottomi));
    info.append(", ");
    info.append(QString::number(bottomj));
    info.append(")\nLeftMost Pixel : ( ");
    info.append(QString::number(lefti));
    info.append(", ");
    info.append(QString::number(leftj));
    info.append(")\nRightMost Pixel : ( ");
    info.append(QString::number(righti));
    info.append(", ");
    info.append(QString::number(rightj));
    info.append(")\nDONE");
    ui->textEdit->setText(info);
    //To Crop the Image to the Outer Periphery

    int height=bottomi-topi+1;
    int width=righti-lefti+1;
    IplImage* result = cvCreateImage(cvSize(width,height),img->depth,img->nChannels);
    int ni,nj;

    for(i=topi,ni=0;i<=bottomi;i++,ni++)
    {
        for (j=lefti,nj=0;j<=righti;j++,nj++)
        {
            cvSet2D(result,ni,nj,cvGet2D(img,i,j));
        }
    }
    cvShowImage("Result", result);



    //------------Releasing Images and Arrays

    cvReleaseImage(&img);
    cvReleaseImage(&result);
    cvSaveImage("AutoCropResult.jpg",result);
    //    cvReleaseMat(&BinaryImage);
    */
}

void MainWindow::slotProgress(int value)
{
    ui->progressBar->setValue(value);
}

void MainWindow::on_pushButton_3_clicked()
{
    AutoCrop act(this);
    connect(&act, SIGNAL(signalProgress(int)),
                this, SLOT(slotProgress(int)));
    act.setImagePath(ui->lineEdit->text());
    //QRect crop = act.autoOuterCrop();
    QRect innercrop=act.autoInnerCrop();
    ui->textEdit->setText(act.output());
    qDebug() << "\nTopLeft : ("<<innercrop.left()<<","<<innercrop.top()<<")";
    qDebug() << "\nBottomRight : ("<<innercrop.right()<<","<<innercrop.bottom()<<")";
    act.ShowOutput(innercrop);

}
