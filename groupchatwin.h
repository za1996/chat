#ifndef GROUPCHATWIN_H
#define GROUPCHATWIN_H

#include "titlebar.h"
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

private:
    Ui::GroupChatWin *ui;
    TitleBar *m_TitleBar;
    QTableWidget *m_EmotionWidget;
    QString m_UserName;

protected:
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void ShowEmotionWidget();
    void SendChatWordMessage();
    void AddEmotion(int, int);
};

#endif // GROUPCHATWIN_H
