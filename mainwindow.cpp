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
