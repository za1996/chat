#ifndef SYSTEMMESSAGEITEM_H
#define SYSTEMMESSAGEITEM_H

#include <QWidget>

namespace Ui {
class SystemMessageItem;
}

class SystemMessageItem : public QWidget
{
    Q_OBJECT

public:
    explicit SystemMessageItem(bool isOnlyMsg, int MsgNum, QWidget *parent = 0);
    ~SystemMessageItem();
    void SetMsgText(const QString &Text);
    void SetButtonHide(bool isOKClick);

signals:
    void IsOK(int);
    void IsReject(int);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::SystemMessageItem *ui;

    bool m_OnlyMsg;
    int m_MsgNum;

private slots:
    void OnOKClick();
    void OnRejectClick();
};

#endif // SYSTEMMESSAGEITEM_H
