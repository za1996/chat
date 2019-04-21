#include "videowindow.h"
#include "ui_videowindow.h"

VideoWindow::VideoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget#VideoWindow{background-color:transparent;}");
    ui->Video->setStyleSheet("QLabel{background-color:rgb(255,255, 255);}");
}

VideoWindow::~VideoWindow()
{
    delete ui;
}
