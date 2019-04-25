#ifndef CHATWINDOWMESSAGEITEM_H
#define CHATWINDOWMESSAGEITEM_H

#include <QWidget>

namespace Ui {
class ChatWindowMessageItem;
}

class ChatWindowMessageItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindowMessageItem(QWidget *parent = 0);
    ~ChatWindowMessageItem();

private:
    Ui::ChatWindowMessageItem *ui;
};

#endif // CHATWINDOWMESSAGEITEM_H
