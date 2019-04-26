#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <memory>
#include <functional>
#include <QHBoxLayout>
#include <atomic>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QTableWidget>
#include "videowindow.h"
#include "message.h"
#include "titlebar.h"
#include "audioplayer.h"
#include "messagewidget.h"

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
    QTableWidget *m_EmotionWidget;
    uint32_t m_FriendId;
    uint32_t m_MeId;
    std::function<void(uint32_t)> m_DelHandle;
    VideoWindow *m_VideoWindow;
    QHBoxLayout *m_VideoWindowLayout;
    std::atomic_bool m_IsUdpChatNow;
    QTimer m_ShowVideoTimer;
    AudioPlayer *m_AudioPlayer;
    bool m_EmotionWidgetShow;

    QHash<uint32_t, std::function<void(MessagePtr)>> m_HandleMap;

private:
    void Inithandle();
    void RemoveVideoChat();
    QImage MatToQImage(cv::Mat &mtx);

    void ReqUdpChatMsgHandle(MessagePtr);
    void RecvChatData(MessagePtr);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
    void moveEvent(QMoveEvent *event);

private slots:
    void ReqVideoChat();
    void CloseVideoWindow();
    void ShowVideoInfo();
    void SendChatWordMessage();
    void ShowEmotionWidget();
    void ChangeWordStyle();
    void AddEmotion(int, int);

signals:
    void ChatWindowUdpChatEnd(uint32_t);



public slots:

    void HandleMessage(uint32_t, std::shared_ptr<Message>);
};

#endif // CHATWINDOW_H
