#include "udprecvicer.h"

#include <assert.h>
#include <QLabel>
#include <QPixmap>


#define CAPTURE_SIZE (UDP_MAX_SIZE * 2)

using namespace cv;
UdpRecvicer::UdpRecvicer() :
    cam(nullptr),
    m_UdpVideoDataTimer(nullptr),
    m_UdpAudioDataTimer(nullptr),
    m_UdpSendAddrTimer(nullptr),
    m_StartSendData(false),
    micDevice(nullptr)
{
}

UdpRecvicer::~UdpRecvicer()
{
    qDebug() << "~UdpRecvicer";
    if(micDevice)
    {
        alcCaptureStop(micDevice);
        alcCaptureCloseDevice(micDevice);
    }
    if(cam) delete cam;
    if(m_UdpVideoDataTimer) delete m_UdpVideoDataTimer;
    if(m_UdpAudioDataTimer) delete m_UdpAudioDataTimer;
    if(m_UdpSendAddrTimer) delete m_UdpSendAddrTimer;
}

void UdpRecvicer::run()
{
    qDebug() << "thread run";
    m_UdpVideoDataTimer = new QTimer;
    m_UdpSendAddrTimer = new QTimer;
    m_UdpAudioDataTimer = new QTimer;
    bool VideoOk = true;
    bool AudioOk = true;
    connect(m_UdpVideoDataTimer, SIGNAL(timeout()), this, SLOT(UdpVideoSend()));
    connect(&m_UdpSocket, SIGNAL(readyRead()), this, SLOT(UdpRecv()));
    connect(m_UdpSendAddrTimer, SIGNAL(timeout()), this, SLOT(SendAddr()));
    connect(m_UdpAudioDataTimer, SIGNAL(timeout()), this, SLOT(UdpAudioSend()));
    m_UdpSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 10);
    cam = new VideoCapture();
    if(!cam->open(0 + CAP_DSHOW))
    {
        qDebug() << "Start Video failed";
        VideoOk = false;
    }
    cam->set(CAP_PROP_FRAME_WIDTH, 320);
    cam->set(CAP_PROP_FRAME_HEIGHT, 240);

    micDevice = alcCaptureOpenDevice(nullptr, AUDIO_FREQ, AL_FORMAT_MONO8, AUDIO_FREQ / 10 * 4);
    if (micDevice == nullptr || alcGetError(micDevice))
    {
        //未检测到输入设备， 你的电脑可能没有插麦克风
        qDebug() << "Start microphone failed! Please check if you have an input device!";
        AudioOk = false;
    }
//    m_VideoLabel.move(20, 20);
//    m_VideoLabel.resize(320, 240);
//    m_VideoLabel.show();
    m_UdpSendAddrTimer->start(500);
    if(VideoOk)
    {
        m_UdpVideoDataTimer->start(130);
    }
    if(AudioOk)
    {
        m_UdpAudioDataTimer->start(110);
    }
    this->exec();
}

void UdpRecvicer::UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size, bool isAudio)
{
//    const static int MAX_SIZE = 1472 - 36;
    uint64_t time = QDateTime::currentDateTime().toMSecsSinceEpoch();
    uint32_t action = isAudio ? UDPAUDIODATA : UDPVIDEODATA;
    for(int start = 0; start < size; start += UDP_MAX_SIZE)
    {
        int PacketSize = (start + UDP_MAX_SIZE) <= size ? UDP_MAX_SIZE : (size - start);
        auto m = CreateUdpChatMsg(FriendId, m_MeId, UDPTRANSFERMSG | action, 0, (data + start), size, start, PacketSize, PacketNum, time);
        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), m_Port);
    }


}

//slot:

void UdpRecvicer::SendAddr()
{
    qDebug() << __FUNCTION__;
    for(int i = 0; i < 10; i++)
    {
        auto m = CreateUdpChatMsg(m_FriendId, m_MeId, UDPADDENDPOINT, 0, "asdasdasdasdasdasdasdasd");
        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), m_Port);
    }
    if(!m_StartSendData)
    {
        m_UdpSendAddrTimer->stop();
        alcCaptureStart(micDevice);
    }
}

void UdpRecvicer::UdpVideoSend()
{
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << " test2";
    if(m_StartSendData)
    {
        qDebug() << __FUNCTION__ << " test";
        static int count = 0;
        Mat frame;
        std::vector<uchar> buff;
        std::vector<int> param = std::vector<int>(2);

        if(!cam->read(frame))
        {
            qDebug() << "read error";
            return;
        }
        qDebug() << "read success";
//        cam->grab();
//        if(!cam->retrieve(frame, 0))
//        {
//            qDebug() << "grab error";
//        }
        param[0] = IMWRITE_JPEG_QUALITY;
        param[1] = 10;
        imencode(".jpg", frame, buff, param);

        qDebug() << "Video Size : " << buff.size();

    //    static std::string str(10000, 'a');

    //    if(!m_UdpChatList.size())
    //    {
    //        m_UdpTimer.stop();
    //        return;
    //    }
    //    else
    //    {
    //        int FriendId = m_UdpChatList.front();
    //        auto m = CreateUdpChatMsg(FriendId, m_UserId, UDPTRANSFERMSG, 0, str);
    //        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), m_UdpPort);
    //    }
        UdpSendToRemote(m_FriendId, count, (char *)buff.data(), buff.size());
        count++;
    }


}

void UdpRecvicer::UdpAudioSend()
{
    qDebug() << __FUNCTION__;
    if(m_StartSendData)
    {
        static int count = 0;
        static char audioBuffer[CAPTURE_SIZE];
        ALint samples;
        alcGetIntegerv(micDevice, ALC_CAPTURE_SAMPLES, 1, &samples);
        qDebug() << "CAPTURE_SIZE : " << samples;
        int size = samples < CAPTURE_SIZE ? samples : CAPTURE_SIZE;
        alcCaptureSamples(micDevice, audioBuffer, size);
        UdpSendToRemote(m_FriendId, count++, audioBuffer, size, true);
    }
}

void UdpRecvicer::UdpRecv()
{
    qDebug() << "start ready read";
    static uint64_t maxVideoTimeOk = 0;
    static uint64_t maxAudioTimeOk = 0;
    static int count = 0;
    uint64_t Currenttime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    while(m_UdpSocket.hasPendingDatagrams())
    {
        int size = m_UdpSocket.pendingDatagramSize();
        uint64_t time;
        uint32_t packetNum;
        uint16_t totalSize, packetStart, packetSize;
        UdpPacketPtr packet;
        auto m = Message::CreateObject();
        m->setRawSize(size);
        m_UdpSocket.readDatagram(m->rawBuf(), size);
        m->refresh();

        QHash<uint32_t, std::map<uint64_t, UdpPacketPtr>> *FriendUdpPacketMap = nullptr;
        std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> *FriendDataQueueMap = nullptr;
        uint64_t *maxTimeOk = nullptr;
        if(m->TypeCode() == UDPVIDEOTRANSFERDATA)
        {
            FriendUdpPacketMap = &m_FriendVideoUdpPacketMap;
            FriendDataQueueMap = &m_FriendVideoDataQueueMap;
            maxTimeOk = &maxVideoTimeOk;
        }
        else if(m->TypeCode() == UDPAUDIOTRANSFERDATA)
        {
            FriendUdpPacketMap = &m_FriendAudioUdpPacketMap;
            FriendDataQueueMap = &m_FriendAudioDataQueueMap;
            maxTimeOk = &maxAudioTimeOk;
        }
        assert(FriendUdpPacketMap != nullptr && FriendDataQueueMap != nullptr);
        auto packetIt = FriendUdpPacketMap->find(m->srcId());
        assert(packetIt != FriendUdpPacketMap->end());
        std::map<uint64_t, UdpPacketPtr> &UdpPacketMap = *packetIt;
        m->getUdpInfo(packetNum, totalSize, packetStart, time);
        packetSize = size - sizeof(MsgHead);
        qDebug() << count++ << " " << size << " " << totalSize <<  " " << time << " " << Currenttime;
        if(time > Currenttime - UDP_MAX_DELAY)
        {
            auto it = UdpPacketMap.find(time);
            if(it != UdpPacketMap.end())
                packet = it->second;
            else
            {
                packet = UdpPacketPtr(new UdpPacket);
                packet->packetNum = packetNum;
                packet->time = time;
                packet->totalSize = totalSize;
                packet->data.resize(totalSize);
                UdpPacketMap.insert(std::pair<uint64_t, UdpPacketPtr>(time, packet));
            }
            memcpy(packet->data.data() + packetStart, m->data(), packetSize);
            packet->size += packetSize;
            if(packet->size == totalSize && time > *maxTimeOk)
            {
//                Mat frame;
                *maxTimeOk = time;
//                frame = imdecode(Mat(packet->data), IMREAD_COLOR);
//                m_VideoLabel.setPixmap(QPixmap::fromImage(MatToQImage(frame)).scaled(m_VideoLabel.width(), m_VideoLabel.height()));
                qDebug() << "isok" << " PackNum : " << packetNum << " time:" << time << "current time : " << Currenttime << " data size : " << packet->data.size();
                auto queueIt = FriendDataQueueMap->find(m->srcId());
                if(queueIt != FriendDataQueueMap->end())
                {
                    queueIt->second.enqueue(packet);
                }
            }

        }
    }

    for(auto packetMapIt = m_FriendVideoUdpPacketMap.begin(); packetMapIt != m_FriendVideoUdpPacketMap.end(); ++packetMapIt)
    {
        auto end = packetMapIt->upper_bound(Currenttime - UDP_MAX_DELAY);
        packetMapIt->erase(packetMapIt->begin(), end);
    }

    for(auto packetMapIt = m_FriendAudioUdpPacketMap.begin(); packetMapIt != m_FriendAudioUdpPacketMap.end(); ++packetMapIt)
    {
        auto end = packetMapIt->upper_bound(Currenttime - UDP_MAX_DELAY);
        packetMapIt->erase(packetMapIt->begin(), end);
    }



}


