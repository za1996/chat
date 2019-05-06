#ifndef GROUPCHATWIN_H
#define GROUPCHATWIN_H

#include "titlebar.h"
#include "message.h"
#include <QWidget>
#include <QTableWidget>

namespace Ui {
class GroupChatWin;
}

class GroupChatWin : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChatWin(uint32_t GroupId, const QString UserName, QWidget *parent = 0);
    ~GroupChatWin();
    void SetTitleName(const QString& Name);
    void SetDelHandle(std::function<void(uint32_t)>);
    void HandleCacheMsg(const std::list<MessagePtr>&);

private:
    Ui::GroupChatWin *ui;
    TitleBar *m_TitleBar;
    QTableWidget *m_EmotionWidget;
    QString m_UserName;
    uint32_t m_GroupId;
    std::function<void(uint32_t)> m_DelHandle;

private:
    void AddMsgItem(uint64_t Time, const QString& UserName, const QString& Msg, bool Me);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void paintEvent(QPaintEvent *event);

private slots:
    void ShowEmotionWidget();
    void SendChatWordMessage();
    void AddEmotion(int, int);

public slots:
    void HandleChatMsg(uint32_t, const QString& UserName, const QString& Msg, uint64_t Time);
};

#endif // GROUPCHATWIN_H
