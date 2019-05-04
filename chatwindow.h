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
#include <QFileInfo>
#include <QListWidget>
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
    explicit ChatWindow(uint32_t Id, uint32_t FriendId, const QString &Name, QWidget *parent = 0);
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
    QHBoxLayout *m_FileTransferLayout;
    QListWidget *m_FileTransferInfo;
    std::atomic_bool m_IsUdpChatNow;
    QTimer m_ShowVideoTimer;
    AudioPlayer *m_AudioPlayer;
    bool m_EmotionWidgetShow;
    QHash<uint64_t, QFileInfo> m_ReadySendFile;

    QHash<uint32_t, std::function<void(MessagePtr)>> m_HandleMap;
    QHash<uint64_t, QListWidgetItem*> m_FileTransferItemMap;
    QHash<uint64_t, DowloadFileItem> m_ReadyDownloadFile;

private:
    void Inithandle();
    void RemoveVideoChat();
    QImage MatToQImage(cv::Mat &mtx);
    void AddFileTransferItem(uint64_t FileNum, bool isUpload, int FileSize, const QString& FileName);

    void ReqUdpChatMsgHandle(MessagePtr);
    void RecvChatData(MessagePtr);
    void ResSendFile(MessagePtr);
    void SendFileOrClose(MessagePtr);
    void SendFileCountinue(MessagePtr);
    void RecvFileEnd(MessagePtr);
    void RemoteForceCloseFile(MessagePtr);

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
    void ReadySendFile();
    void DownloadFile(uint64_t);
    void ForceCloseFile(uint64_t);

signals:
    void ChatWindowUdpChatEnd(uint32_t);



public slots:

    void HandleMessage(uint32_t, std::shared_ptr<Message>);
    void OnTransferInfoClick();
    void HandleSendOrRecvSignal(uint64_t, uint32_t, int, int);
    void HandleFileSendedEnd(uint64_t);
};

#endif // CHATWINDOW_H
