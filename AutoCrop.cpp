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

QRect AutoCrop::autoInnerCrop()
{
    CvMat* img = cvLoadImageM(d->path.toStdString().c_str());
    //CvMat* img = cvLoadImage(d->path.toStdString().c_str());
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
    for(i=0;i<img->cols;i++)
    {
        for(j=0;j<img->rows;j++)
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
    for(i=(img->cols-1);i>=0;i--)
    {
        for(j=0;j<img->rows;j++)
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
    for(i=0;i<img->rows;i++)
    {
        for(j=0;j<img->cols;j++)
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
    for(i=(img->rows-1);i>=0;i--)
    {
        for(j=0;j<img->cols;j++)
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
    QRect crop;
    crop.setTopLeft(p1);
    crop.setBottomRight(p2);

    //crop Image to outerCrop
    IplImage* result = cvCreateImage(cvSize(crop.width(),crop.height()),IPL_DEPTH_32F,3);
    int ni,nj;
    qDebug() << "From "<<crop.top()<<" to "<<crop.bottom()<<" & "<<crop.left()<<" to "<<crop.right();
    for(i=crop.top(),ni=0;i<=crop.bottom();i++,ni++)
    {
        for(j=crop.left(),nj=0;j<=crop.right();j++,nj++)
        {
            cvSet2D(result,ni,nj,cvGet2D(img,i,j));

        }
    }
    cvReleaseMat(&img);

    //work for innerCrop


    //---------------------threshold the image
    IplImage* threshold = cvCreateImage(cvSize(crop.width(),crop.height()),IPL_DEPTH_8U,1);
    float sum=0.0;
    for(i=0;i<result->height;i++)
    {
        for(j=0;j<result->width;j++)
        {
            sum=cvGet2D(result,i,j).val[0] + cvGet2D(result,i,j).val[1] + cvGet2D(result,i,j).val[2];
            if(sum==0)
                cvSet2D(threshold,i,j,cvScalar(0));
            else
                cvSet2D(threshold,i,j,cvScalar(255));
        }
    }


    int limitcolumn = threshold->width;
    int limitrow    = threshold->height;
    int centeri=((limitrow/2)-1);
    int centerj=((limitcolumn/2)-1);
    int startrighti=0,    startrightj=0,    endrighti=0,      endrightj=0;
    int startlefti=0,     startleftj=0,     endlefti=0,       endleftj=0;
    int startupi=0,       startupj=0,       endupi=0,         endupj=0;
    int startdowni=0,     startdownj=0,     enddowni=0,       enddownj=0;
    int travelright=0,    travelleft=0,     travelup=0,       traveldown=0;
    int rightmargin=0,    leftmargin=0,     bottommargin=0,   topmargin=0;
    int counter=0;
    bool fixtopmargin    =       false;
    bool fixrightmargin  =       false;
    bool fixleftmargin   =       false;
    bool fixbottommargin =       false;
    int count=0;
    endupi = centeri;
    endupj = centerj;
    travelright = traveldown = -1;
    travelleft  = travelup   = 0;

    while(true)
    {
        switch((counter%4))
        {
        case 0 :    //travelling right
            travelright += 2;
            if(fixrightmargin==true)
                travelright--;
            if(fixleftmargin==true)
                travelright--;
            if(fixtopmargin==true)
            {
                if(fixrightmargin==false)
                    endrightj++;
                break;
            }
            startrighti=endupi;
            startrightj=endupj;
            endrightj=startrightj+travelright;
            if(endrightj >= limitcolumn)
            {
                endrightj = limitcolumn-1;
                fixrightmargin=true;
                rightmargin=limitcolumn-1;
                counter++;
                travelright--;
            }
            i=startrighti;
            j=startrightj;
            for(j=startrightj+1;j<=endrightj;j++)
            {
                if(cvGet2D(threshold,i,j).val[0]==0)
                {
                    //we have found an empty space
                    fixtopmargin=true;
                    topmargin=i;
                    endupi++;
                    travelup--;
                    break;
                }
            }
            endrighti=startrighti;

            break;
        case 1 :    //travelling down
            traveldown += 2;
            if(fixbottommargin==true)
                traveldown--;
            if(fixtopmargin==true)
                traveldown--;
            if(fixrightmargin==true)
            {
                if(fixbottommargin==false)
                    enddowni++;
                //endrightj--;
                // qDebug() << "Travelling down : Case Skipped\n";
                break;
            }
            startdowni=endrighti;
            startdownj=endrightj;
            enddowni = startdowni + traveldown;
            if(enddowni >= limitrow)
            {
                enddowni = limitrow-1;
                counter++;
                bottommargin=limitrow-1;
                fixbottommargin=true;
                traveldown--;
            }
            i=startdowni;
            j=startdownj;
            for(i=startdowni+1; i<=enddowni; i++)
            {
                if(cvGet2D(threshold,i,j).val[0]==0)
                {
                    //we have found an empty space
                    fixrightmargin=true;
                    rightmargin=j;
                    endrightj--;
                    travelright--;
                    break;
                }
            }
            enddownj=startdownj;

            break;
        case 2 :    //travelling left
            travelleft += 2;
            if(fixleftmargin==true)
                travelleft--;
            if(fixrightmargin==true)
                travelleft--;
            if(fixbottommargin==true)
            {
                if(fixleftmargin==false)
                    endleftj--;
                break;
            }
            startlefti=enddowni;
            startleftj=enddownj;
            endleftj = startleftj - travelleft;
            if(endleftj < 0)
            {
                endleftj = 0;
                counter++;
                leftmargin=0;
                fixleftmargin=true;
                travelleft--;
            }
            i=startlefti;
            j=startleftj;
            for(j=startleftj-1;j>=endleftj;j--)
            {
                if(cvGet2D(threshold,i,j).val[0]==0)
                {
                    //we have found an empty space
                    fixbottommargin=true;
                    bottommargin=i;
                    enddowni--;
                    traveldown--;
                    break;
                }
            }
            endlefti=startlefti;

            break;
        case 3 :    //travelling up
            travelup += 2;
            if(fixbottommargin==true)
                travelup--;
            if(fixtopmargin==true)
                travelup--;
            if(fixleftmargin==true)
            {
                if(fixtopmargin==false)
                    endupi--;
                break;
            }
            startupi=endlefti;
            startupj=endleftj;
            endupi = startupi - travelup;
            //                            // qDebug() << "Travel : "<<travelup<<"\n";
            if(endupi < 0)
            {
                endupi = 0;
                counter++;
                fixtopmargin=true;
                topmargin=0;
                travelup--;
            }
            i=startupi;
            j=startupj;
            for(i=startupi-1; i>=endupi; i--)
            {
                if(cvGet2D(threshold,i,j).val[0]==0)
                {
                    //we have found an empty space
                    fixleftmargin=true;
                    leftmargin=j;
                    endleftj++;
                    travelleft--;
                    break;
                }
            }
            endupj=startupj;

            break;
        }
        counter++;
        if ( fixbottommargin==true && fixtopmargin==true && fixleftmargin==true && fixrightmargin==true)
            break;
    }
    qDebug() << "Endupi    : "<<endupi<<"\n";
    qDebug() << "Endupj    : "<<endupj<<"\n";
    qDebug() << "Endrighti : "<<endrighti<<"\n";
    qDebug() << "Endrightj : "<<endrightj<<"\n";
    qDebug() << "Enddowni  : "<<enddowni<<"\n";
    qDebug() << "Enddownj  : "<<enddownj<<"\n";
    qDebug() << "Endlefti  : "<<endlefti<<"\n";
    qDebug() << "Endleftj  : "<<endleftj<<"\n";
    qDebug() << "Done\n";

    qDebug() << "\n";
    qDebug() << "Left   Margin   : "<<leftmargin<<"\n";
    qDebug() << "Right  Margin   : "<<rightmargin<<"\n";
    qDebug() << "Top    Margin   : "<<topmargin<<"\n";
    qDebug() << "Bottom Margin   : "<<bottommargin<<"\n";

    //----------------------releasing images
    cvReleaseImage(&threshold);
    cvReleaseImage(&result);
    QPoint icp1;
    icp1.setX(leftColumn+leftmargin+1);
    icp1.setY(topRow+topmargin+1);
    QPoint icp2;
    icp2.setX(leftColumn+rightmargin-1);
    icp2.setY(topRow+bottommargin-1);
    QRect cropArea;
    cropArea.setTopLeft(icp1);
    cropArea.setBottomRight(icp2);

    return(cropArea);
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
