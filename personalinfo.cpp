#include "personalinfo.h"
#include "ui_personalinfo.h"
#include "global.h"

#include <QDebug>
#include <QPainter>
#include <QBitmap>
#include <QPainterPath>
#include <QColor>
#include <QtCore/qmath.h>

PersonalInfo::PersonalInfo(QWidget *parent, const QString &Name, uint32_t id, int Sex, const QString &Date, const QString& Desc, const QString& Profile, bool Editable) :
    QWidget(parent),
    ui(new Ui::PersonalInfo)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#Board{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);

    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->Board->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(MIN_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");

    QStringList strList;
    strList << QString("man") << QString("girl");
    ui->SexComboBox->addItems(strList);

    ui->Profile->installEventFilter(this);
    QString FullPath = UserProfileCachePath(id, Profile);
    if(!QFileInfo(FullPath).isFile())
    {
        FullPath = "D:/imggggg.bmp";
    }
    ui->Profile->setPixmap(QPixmap::fromImage(QImage(FullPath)).scaled(ui->Profile->size()));
    ui->NameLineEdit->setText(Name);
    ui->NumLabel->setText(QString::number(id));
    ui->DescPlainTextEdit->setPlainText(Desc);
    ui->BirthdayDateEdit->setDate(QDate::fromString(Date, "yyyy-MM-dd"));
    if(!Sex)
    {
        ui->SexComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->SexComboBox->setCurrentIndex(1);
    }

    if(!Editable)
    {
        ui->BirthdayDateEdit->setReadOnly(true);
        ui->DescPlainTextEdit->setReadOnly(true);
        ui->NameLineEdit->setReadOnly(true);
        ui->SexComboBox->setDisabled(true);
        ui->SaveButton->hide();
    }

    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->SaveButton, SIGNAL(clicked(bool)), this, SLOT(ChangeMyselfInfo()));

}

PersonalInfo::~PersonalInfo()
{
    qDebug() << "~PersonalInfo";
    delete ui;
}

void PersonalInfo::paintEvent(QPaintEvent *event)
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

void PersonalInfo::ChangeMyselfInfo()
{
    QString Birthday = ui->BirthdayDateEdit->dateTime().toString("yyyy/MM/dd HH:mm:ss");
    QString Name = ui->NameLineEdit->text();
    QString Desc = ui->DescPlainTextEdit->toPlainText();
    int Sex = ui->SexComboBox->currentIndex() % 2;
    qDebug() << Birthday << " " << Sex;
    auto m = CreateChangeMyselfInfoMsg(m_ThisIsId, 0, Name, Desc, Birthday, Sex);
    SendtoRemote(s, m);
}

bool PersonalInfo::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->Profile)
    {
        if (event->type() == QEvent::MouseButtonPress)//mouse button pressed
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_ProfileWin = new ProfileUploadWin(false, m_ThisIsId);
                m_ProfileWin->show();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    this->QWidget::eventFilter(watched, event);
}

void PersonalInfo::ChangeProfile(uint32_t id, const QString &FileName)
{
    if(id == m_ThisIsId)
    {
        ui->Profile->setPixmap(QPixmap::fromImage(QImage(FileName)).scaled(ui->Profile->size()));
    }
}
