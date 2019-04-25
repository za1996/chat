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
extern "C"
{
#include <al.h>
#include <alc.h>
}

class UdpRecvicer : public QThread
{
    Q_OBJECT
public:

    UdpRecvicer();
    ~UdpRecvicer();

    void run();

    void SetMeId(uint32_t Id) { m_MeId = Id; }
    void SetFriendId(uint32_t Id) {m_FriendId = Id; }
    void SetPort(int port) { m_Port = port; }
    void ReadyStart() {m_StartSendData = true;}

private:
//    void UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size);
    uint32_t m_MeId;
    uint32_t m_FriendId;
    int m_Port;
    QUdpSocket m_UdpSocket;
    QTimer *m_UdpVideoDataTimer;
    QTimer *m_UdpSendAddrTimer;
    QTimer *m_UdpAudioDataTimer;
    cv::VideoCapture *cam;
    ALCdevice* micDevice;
    bool m_StartSendData;

    void UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size, bool isAudio = false);

private slots:
    void UdpVideoSend();
    void UdpAudioSend();
    void UdpRecv();
    void SendAddr();
};

#endif // UDPRECVICER_H
