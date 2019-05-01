#include "profileuploadwin.h"
#include "ui_profileuploadwin.h"
#include <QFileDialog>
#include <QScrollBar>
#include <QPainter>
#include <QPalette>
#include <QtCore/qmath.h>
#include <QDebug>
#include "global.h"


ProfileUploadWin::ProfileUploadWin(bool isGroup, uint32_t id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileUploadWin),
    m_isGroup(isGroup),
    m_Img(nullptr),
    m_UsersGroupId(id)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#MainBoard{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);
    QPalette Palette;
    Palette.setColor(QPalette::Background, Qt::white);
    ui->Size50Img->setPalette(Palette);
    ui->Size100Img->setPalette(Palette);
    ui->Size150Img->setPalette(Palette);
    ui->ProfileArea->setPalette(Palette);
    ui->ProfileArea->resize(ui->ProfileArea->width() + ui->ProfileArea->verticalScrollBar()->height(), ui->ProfileArea->height() + ui->ProfileArea->horizontalScrollBar()->height());

    m_Label = new QLabel;

    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->MainBoard->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");
    connect(ui->ChooseFile, SIGNAL(clicked(bool)), this, SLOT(SelectFile()));
    connect(ui->ReadImg, SIGNAL(clicked(bool)), this, SLOT(ReadImg()));
    connect(ui->Upload, SIGNAL(clicked(bool)), this, SLOT(UploadImg()));
    connect(ui->Close, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
}

ProfileUploadWin::~ProfileUploadWin()
{
    delete ui;
}

void ProfileUploadWin::SelectFile()
{
    m_File = QFileDialog::getOpenFileName(this, tr("选择文件"), "C:", tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    qDebug() << m_File;
    if(m_File != "")
    {
        if(m_Img) delete m_Img;
        m_Img = new QImage(m_File);
        m_Label->setPixmap(QPixmap::fromImage(*m_Img));
        m_Label->resize(m_Img->size());
        ui->ProfileArea->setWidget(m_Label);

    }
}

void ProfileUploadWin::ReadImg()
{
    if(!m_File.isEmpty())
    {
        QImage m = m_Img->copy(ui->ProfileArea->horizontalScrollBar()->value(), ui->ProfileArea->verticalScrollBar()->value(), 150, 150);
        ui->Size150Img->setPixmap(QPixmap::fromImage(m));
        ui->Size100Img->setPixmap(QPixmap::fromImage(m).scaled(100, 100));
        ui->Size50Img->setPixmap(QPixmap::fromImage(m).scaled(50, 50));
    }
}

void ProfileUploadWin::UploadImg()
{
    if(!m_File.isEmpty())
    {
//        info["FileName"] = m_File.split('/').last()->toStdString();
//        uint32_t ClientFileNum = m_ClientFileNum++;
//        info["ClientFileNum"] = ClientFileNum;
        QImage img = m_Img->copy(ui->ProfileArea->horizontalScrollBar()->value(), ui->ProfileArea->verticalScrollBar()->value(), 150, 150);
        uint32_t ClientFileNum = m_ClientFileNum++;
        QString Name = QString("%1%2").arg(ClientFileNum).arg(m_File.split('/').last());
        QString path = QString("%1/%2/%3/%4").arg("E:/University_Final_Text_Qt_Project/cache").arg(m_ThisIsId).arg("profile").arg(Name);
        qDebug() << __FUNCTION__;
        qDebug() << Name;
        qDebug() << path;
        img.save(path);
        uint32_t action = m_isGroup ? UPLOADUSERSGROUPPROFILE : UPLOADUSERPROFILE;
        auto m = CreateReadySendProfileMsg(m_ThisIsId, 0, ClientFileNum, Name.toStdString(), action, m_UsersGroupId);
        SendtoRemote(s, m);
        m_ClientFileNumMap.insert(ClientFileNum, path.toStdString());
        this->close();
    }
}

void ProfileUploadWin::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
    this->QWidget::paintEvent(event);
}
