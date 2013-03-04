#include "AutoCrop.h"

// C++ includes

#include <cmath>
#include <cfloat>

// Qt includes.

#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QPoint>
#include <QRect>

// OpenCV includes

#include <highgui.h>
#include <cvaux.h>
#include <opencv/cv.h>

class AutoCrop::Private
{
public:

    QString path;
    QString output;
};

AutoCrop::AutoCrop(QObject* const parent)
    : QObject(parent), d(new Private)
    {
    }

AutoCrop::~AutoCrop()
{
    delete d;
}

void AutoCrop::setImagePath(const QString& path)
{
    d->path=path;
}

QString AutoCrop::output() const
{
    return d->output;
}

void AutoCrop::postProgress(int p, const QString& t)
{
    qDebug() << "\n" << p << "% - " << t << "\n";
    emit signalProgress(p);
    qApp->processEvents();
}

QRect AutoCrop::autoOuterCrop()
{
    IplImage* img = cvLoadImage(d->path.toStdString().c_str());
    int i,j;
    int breakflag=0;
    float channel0,channel1, channel2;
    int topRow=-1, topColumn=-1;
    int bottomRow=-1, bottomColumn=-1;
    int leftRow=-1,leftColumn=-1;
    int rightRow=-1,rightColumn=-1;
    postProgress(10,"Data Init.");

    //----------------------Finding the outer boundaries of the image (i.e. with black portions)
    /*
      This would be done in 4 steps
      1. Search column wise:
      (a) From the left to the right, this is to get the left boundary
      (b) From the right to the left, this is to get the right boundary
      2. Search row wise :
      (a) From the top to the bottom, this is to get the top boundary
      (b) From the bottom to the top, this is to get the bottom boundary
      */



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
                leftRow=j;
                leftColumn=i;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug("Done Till step 1(a)");
    postProgress(30,"Left extracted");

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
                rightRow=j;
                rightColumn=i;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug()<<"Done Till step 1(b)";
    postProgress(50,"Right Extracted");

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
                topRow=i;
                topColumn=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }
    qDebug()<<"Done Till step 2(a)";
    postProgress(70,"Top Extracted");

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
                bottomRow=i;
                bottomColumn=j;
                breakflag=1;
            }
        }
        if(breakflag==1)
            break;
    }

    qDebug()<<"Done Till step 2(b)";
    postProgress(90,"Bottom Extracted");

    //------making the required output--------------------
    d->output.append("TopMost Pixel : ( ");
    d->output.append(QString::number(topRow));
    d->output.append(", ");
    d->output.append(QString::number(topColumn));
    d->output.append(")\nBottomMost Pixel : ( ");
    d->output.append(QString::number(bottomRow));
    d->output.append(", ");
    d->output.append(QString::number(bottomColumn));
    d->output.append(")\nLeftMost Pixel : ( ");
    d->output.append(QString::number(leftRow));
    d->output.append(", ");
    d->output.append(QString::number(leftColumn));
    d->output.append(")\nRightMost Pixel : ( ");
    d->output.append(QString::number(rightRow));
    d->output.append(", ");
    d->output.append(QString::number(rightColumn));
    d->output.append(")\nDONE");
    postProgress(91,"Verbose Output done");
    QPoint p1;
    p1.setX(leftColumn);
    p1.setY(topRow);
    QPoint p2;
    p2.setX(rightColumn);
    p2.setY(bottomRow);
    QRect cropArea;
    cropArea.setTopLeft(p1);
    cropArea.setBottomRight(p2);
    //----releasing Images---------------------------------
    cvReleaseImage(&img);

    return cropArea;
}

void AutoCrop::ShowOutput(QRect crop)
{
    IplImage* img = cvLoadImage(d->path.toStdString().c_str());
    IplImage* result = cvCreateImage(cvSize(crop.width(),crop.height()),img->depth,img->nChannels);
    int i,j;
    int ni,nj;
    qDebug() << "From "<<crop.top()<<" to "<<crop.bottom()<<" & "<<crop.left()<<" to "<<crop.right();
    for(i=crop.top(),ni=0;i<=crop.bottom();i++,ni++)
    {
        for(j=crop.left(),nj=0;j<=crop.right();j++,nj++)
        {
            cvSet2D(result,ni,nj,cvGet2D(img,i,j));

        }
    }
    cvShowImage("Result", result);
    cvReleaseImage(&img);
    cvReleaseImage(&result);
}
