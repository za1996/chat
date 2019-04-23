#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "global.h"

#include <QPainter>
#include <QPushButton>
#include <QtCore/qmath.h>
#include <QResizeEvent>
#include <QMessageBox>
#include <tchar.h>

ChatWindow::ChatWindow(uint32_t Id, uint32_t FriendId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow),
    m_MeId(Id),
    m_FriendId(FriendId),
    m_DelHandle(nullptr),
    m_VideoWindow(nullptr),
    m_VideoWindowLayout(nullptr),
    m_IsUdpChatNow(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#MainBoard{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);

    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->MainBoard->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(MIN_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");
    m_TitleBar->setTitleContent("我的好友", 15);




    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->VideoButton, SIGNAL(clicked(bool)), this, SLOT(ReqVideoChat()));

    Inithandle();
}

ChatWindow::~ChatWindow()
{
    delete ui;
    if(m_DelHandle)m_DelHandle(m_FriendId);
}

//init
void ChatWindow::Inithandle()
{
    m_HandleMap.insert(MESSAGETYPE(REQUDPEVENTGROUP, REQUDPCHATACTION), std::bind(&ChatWindow::ReqUdpChatMsgHandle, this, std::placeholders::_1));
}

// 虚函数
void ChatWindow::paintEvent(QPaintEvent *event)
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

void ChatWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

//public fun
void ChatWindow::SetDelHandle(std::function<void (uint32_t)> f)
{
    m_DelHandle = f;
}

//private fun
void ChatWindow::ReqVideoChat()
{
    if(!m_IsUdpChatNow)
    {
        m_IsUdpChatNow = true;
        if(!m_VideoWindowLayout && !m_VideoWindow)
        {
            m_VideoWindowLayout = new QHBoxLayout(ui->MainBoard);
            m_VideoWindowLayout->setAlignment(Qt::AlignHCenter);
            m_VideoWindow = new VideoWindow(ui->MainBoard);
            m_VideoWindowLayout->addWidget(m_VideoWindow);
            connect(m_VideoWindow, SIGNAL(VideoWindowCloseButtonClick()), this, SLOT(CloseVideoWindow()));
        }
        this->resize(this->width() + m_VideoWindow->width(), this->height());
        ui->MainLayout->addLayout(m_VideoWindowLayout);
        auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQUDPCHATACTION, 0);
        SendtoRemote(s, m);
    }

}


//slot
void ChatWindow::HandleMessage(uint32_t id, std::shared_ptr<Message> m)
{
    qDebug() << __FUNCTION__ << " messageid : " << id << "chat id : " << m_FriendId;
    if(id == m_FriendId)
    {
        qDebug() << __FUNCTION__ << "ready fun start";
        auto it = m_HandleMap.find(m->Type());
        if(it != m_HandleMap.end())
        {
            qDebug() << __FUNCTION__ << "fun start";
            (*it)(m);
            qDebug() << __FUNCTION__ << "fun end";
        }
        qDebug() << __FUNCTION__ << "ready fun end";
    }
}

void ChatWindow::CloseVideoWindow()
{
    m_IsUdpChatNow = false;
    ui->MainLayout->removeItem(m_VideoWindowLayout);
    this->resize(ui->MsgEdit->width(), this->height());
    auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQCLOSEUDPCHATACTION, 0);
    SendtoRemote(s, m);
}

//消息处理函数
void ChatWindow::ReqUdpChatMsgHandle(MessagePtr m)
{
    if(m->Type() == MESSAGETYPE(REQUDPEVENTGROUP, REQUDPCHATACTION) && !m_IsUdpChatNow)
    {
        ReqVideoChat();
        auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQUDPCHATSUCCESSACTION, 0);
        SendtoRemote(s, m);

    }
    else if(m->Type() == MESSAGETYPE(RESUDPREQGROUP, RESCLOSEUDPCHATCODE))
    {

    }
    else if(m->Type() == MESSAGETYPE(RESUDPREQGROUP, RESREADYUDPCHATSENDCODE))
    {

    }
}
