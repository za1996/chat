#include "personalinfo.h"
#include "ui_personalinfo.h"

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

    ui->NameLineEdit->setText(Name);
    ui->NumLabel->setText(QString::number(id));
    ui->DescPlainTextEdit->setPlainText(Desc);
    ui->BirthdayDateEdit->setDate(QDate::fromString(Date, "yyyy-MM-dd"));

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
