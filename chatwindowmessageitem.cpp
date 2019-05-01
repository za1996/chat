#include "chatwindowmessageitem.h"
#include "ui_chatwindowmessageitem.h"

#include <QDateTime>


ChatWindowMessageItem::ChatWindowMessageItem(QWidget *parent, bool isMe, uint64_t time, const QString &text, int width) :
    QWidget(parent),
    m_MsgWidget(width, this),
    m_TimerLabel(this),
    m_IsMe(isMe),
    ui(new Ui::ChatWindowMessageItem)
{
    ui->setupUi(this);
    m_TimerLabel.setText(QDateTime::fromMSecsSinceEpoch(time).toString("yyyy-MM-dd hh:mm:ss:zzz"));
    m_TimerLabel.adjustSize();
    m_TimerLabel.show();
    m_MsgWidget.show();
    m_MsgWidget.SetText(text);
    UpdateWidth(width);
//    m_MsgWidget.hide();
}

ChatWindowMessageItem::~ChatWindowMessageItem()
{
    delete ui;
}

void ChatWindowMessageItem::paintEvent(QPaintEvent *event)
{

}

void ChatWindowMessageItem::UpdatePoint()
{
    if(m_IsMe)
    {
        m_TimerLabelPoint.setX(this->width() - m_TimerLabel.width() - SPACE);
        m_TimerLabelPoint.setY(SPACE);
        m_MsgWidgetPoint.setX(this->width() - m_MsgWidget.width() - SPACE);
        m_MsgWidgetPoint.setY(m_TimerLabelPoint.y() + m_TimerLabel.height() + SPACE);
    }
    else
    {
         m_TimerLabelPoint.setX(SPACE);
         m_TimerLabelPoint.setY(SPACE);
         m_MsgWidgetPoint.setX(SPACE);
         m_MsgWidgetPoint.setY(m_TimerLabelPoint.y() + m_TimerLabel.height() + SPACE);
    }
    m_TimerLabel.move(m_TimerLabelPoint);
    m_MsgWidget.move(m_MsgWidgetPoint);
}

void ChatWindowMessageItem::UpdateWidth(int width)
{
    m_MsgWidget.SetWidth(width - SPACE - WHITE);
    this->resize(width, SPACE + m_TimerLabel.height() + SPACE + m_MsgWidget.height());
    UpdatePoint();
}
