#include "usersgroupinfo.h"
#include "ui_usersgroupinfo.h"
#include "profileuploadwin.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QMouseEvent>

UsersGroupInfo::UsersGroupInfo(QWidget *parent, const QString& Name, uint32_t id, const QString& Desc, const QString& Profile, bool Editable) :
    QWidget(parent),
    ui(new Ui::UsersGroupInfo),
    m_GroupId(id)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#MainBoard{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);

    ui->NameLineEdit->setText(Name);
    ui->DescTextEdit->insertHtml(Desc);
    ui->ID->setText(QString("%1").arg(id));

    ui->ProfileLabel->installEventFilter(this);


    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->MainBoard->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");
    if(!Editable)
    {
        ui->SaveButton->hide();
        ui->DescTextEdit->setReadOnly(true);
        ui->NameLineEdit->setReadOnly(true);
    }
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
}

UsersGroupInfo::~UsersGroupInfo()
{
    delete ui;
}

void UsersGroupInfo::paintEvent(QPaintEvent *event)
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

bool UsersGroupInfo::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->ProfileLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)//mouse button pressed
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                ProfileUploadWin *w = new ProfileUploadWin(m_GroupId);
                w->show();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
