#ifndef CHATWINDOWMESSAGEITEM_H
#define CHATWINDOWMESSAGEITEM_H

#include <QWidget>
#include <QResizeEvent>
#include <QPoint>
#include <QLabel>

#include "messagewidget.h"

namespace Ui {
class ChatWindowMessageItem;
}

class ChatWindowMessageItem : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *event);

public:
    explicit ChatWindowMessageItem(QWidget *parent, bool isMe, uint64_t time, const QString &text, int width, QString UserName = "");
    ~ChatWindowMessageItem();

    void UpdateWidth(int width);

private:
    Ui::ChatWindowMessageItem *ui;
    QLabel m_TimerLabel;
    bool m_IsMe;
    MessageWidget m_MsgWidget;
    QPoint m_TimerLabelPoint;
    QPoint m_MsgWidgetPoint;
    enum { SPACE = 10, WHITE = 100 };
    void UpdatePoint();
};

#endif // CHATWINDOWMESSAGEITEM_H
