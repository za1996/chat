#include "systemmessageitem.h"
#include "ui_systemmessageitem.h"
#include "global.h"
#include <QDebug>

SystemMessageItem::SystemMessageItem(bool isOnlyMsg, int MsgNum, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemMessageItem),
    m_OnlyMsg(isOnlyMsg),
    m_MsgNum(MsgNum)
{
    ui->setupUi(this);
    if(m_OnlyMsg)
    {
        ui->BottomWidget->hide();
        this->resize(ui->MsgText->size());
    }
    else
    {
        ui->BottomWidget->show();
    }
    connect(ui->OKButton, SIGNAL(clicked(bool)), this, SLOT(OnOKClick()));
    connect(ui->RejectButton, SIGNAL(clicked(bool)), this, SLOT(OnRejectClick()));
}

SystemMessageItem::~SystemMessageItem()
{
    delete ui;
}

void SystemMessageItem::resizeEvent(QResizeEvent *event)
{
    if(m_OnlyMsg)
    {
        ui->MsgText->resize(this->size());
    }
    else
    {
        ui->MsgText->resize(this->width(), this->height() - ui->BottomWidget->height());
        ui->BottomWidget->move(ui->MsgText->x(), ui->MsgText->y() + ui->MsgText->height());
        ui->BottomWidget->resize(this->width(), ui->BottomWidget->height());
        ui->ButtonClickMsg->resize(ui->BottomWidget->size());
        ui->RejectButton->move(this->width() - 10 - ui->RejectButton->width(), ui->RejectButton->y());
        ui->OKButton->move(ui->RejectButton->x() - 10 - ui->OKButton->width(), ui->RejectButton->y());
    }
    qDebug() << __FUNCTION__;
    qDebug() << this->size();
    qDebug() << ui->MsgText->pos();
    qDebug() << ui->MsgText->size();
    qDebug() << ui->BottomWidget->pos();
    qDebug() << ui->BottomWidget->size();
}

void SystemMessageItem::SetMsgText(const QString &Text)
{
    ui->MsgText->setText(Text);
    ui->MsgText->adjustSize();
    if(m_OnlyMsg)
    {
        this->resize(this->width(), ui->MsgText->height());
    }
    else
    {
        this->resize(this->width(), ui->MsgText->height() + ui->BottomWidget->height());
    }
}

void SystemMessageItem::OnOKClick()
{
    ui->OKButton->hide();
    ui->RejectButton->hide();
    ui->ButtonClickMsg->setText("同意");
//    emit IsOK(m_MsgNum);
    MessagePtr m;
    if(m_SysMsgCache[m_MsgNum].m->Type() == MESSAGETYPE(TRANSFERDATAGROUP, ADDREMOTEFRIENDACTION))
    {
        m = CreateRealAddFriendMsg(m_ThisIsId, 0, m_ThisIsId, m_SysMsgCache[m_MsgNum].m->srcId());
    }
    else if(m_SysMsgCache[m_MsgNum].m->Type() == MESSAGETYPE(TRANSFERDATAGROUP, REQJOININGROUPACTION))
    {
        json info = json::parse((char *)m_SysMsgCache[m_MsgNum].m->data());
        uint32_t UserId = info["UserId"].get<json::number_unsigned_t>();
        uint32_t GroupId = info["GroupId"].get<json::number_unsigned_t>();
        m = CreateAddMemberToGroupMsg(m_ThisIsId, 0, UserId, GroupId);
    }
    else
    {
        return;
    }
    SendtoRemote(s, m);
}

void SystemMessageItem::OnRejectClick()
{
    ui->OKButton->hide();
    ui->RejectButton->hide();
    ui->ButtonClickMsg->setText("拒绝");
    emit IsReject(m_MsgNum);
}

void SystemMessageItem::SetButtonHide(bool isOKClick)
{
    ui->OKButton->hide();
    ui->RejectButton->hide();
    if(isOKClick)
    {
        ui->ButtonClickMsg->setText("同意");
    }
    else
    {
        ui->ButtonClickMsg->setText("拒绝");
    }
}
