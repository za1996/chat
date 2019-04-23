#ifndef UDPRECVICER_H
#define UDPRECVICER_H

#include <QThread>
#include <map>
#include <QHash>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QLabel>
#include "message.h"
#include "global.h"

class UdpRecvicer : public QThread
{
    Q_OBJECT
public:

    UdpRecvicer();

    void run();

    void SetMeId(uint32_t Id) { m_MeId = Id; }
    void SetFriendId(uint32_t Id) {m_FriendId = Id; }

private:
//    void UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size);
    uint32_t m_MeId;
    uint32_t m_FriendId;
    QUdpSocket m_UdpSocket;
    QTimer *m_UdpTimer;
    cv::VideoCapture *cam;
    QLabel m_VideoLabel;

    void UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size);

private slots:
    void UdpSend();
    void UdpRecv();
};

#endif // UDPRECVICER_H
