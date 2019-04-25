#include "videowindow.h"
#include "ui_videowindow.h"

VideoWindow::VideoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget#VideoWindow{background-color:transparent;}");
    ui->Video->setStyleSheet("QLabel{background-color:rgb(255,255, 255);}");
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(OnClosePress()));
}

VideoWindow::~VideoWindow()
{
    delete ui;
}

void VideoWindow::OnClosePress()
{
    emit VideoWindowCloseButtonClick();
}

void VideoWindow::SetVideoPic(const QPixmap &pixmap)
{
    ui->Video->setPixmap(pixmap.scaled(ui->Video->width(), ui->Video->height()));
}
