#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <memory>
#include <functional>
#include <QHBoxLayout>
#include <atomic>
#include "videowindow.h"
#include "message.h"
#include "titlebar.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(uint32_t Id, uint32_t FriendId, QWidget *parent = 0);
    ~ChatWindow();

    void SetDelHandle(std::function<void(uint32_t)>);

    bool isUdpChatNow() { return m_IsUdpChatNow.load(); }

private:
    Ui::ChatWindow *ui;
    TitleBar *m_TitleBar;
    uint32_t m_FriendId;
    uint32_t m_MeId;
    std::function<void(uint32_t)> m_DelHandle;
    VideoWindow *m_VideoWindow;
    QHBoxLayout *m_VideoWindowLayout;
    std::atomic_bool m_IsUdpChatNow;

    QHash<uint32_t, std::function<void(MessagePtr)>> m_HandleMap;

private:
    void Inithandle();
    void ReqUdpChatMsgHandle(MessagePtr);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void ReqVideoChat();
    void CloseVideoWindow();


public slots:

    void HandleMessage(uint32_t, std::shared_ptr<Message>);
};

#endif // CHATWINDOW_H
