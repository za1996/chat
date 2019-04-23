#include "udprecvicer.h"

#include <assert.h>
#include <QLabel>
#include <QPixmap>

using namespace cv;
UdpRecvicer::UdpRecvicer()
{

}

void UdpRecvicer::run()
{
    m_UdpTimer = new QTimer;
    connect(m_UdpTimer, SIGNAL(timeout()), this, SLOT(UdpSend()));
    connect(&m_UdpSocket, SIGNAL(readyRead()), this, SLOT(UdpRecv()));
    m_UdpSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 10);
    cam = new VideoCapture();
    cam->open(0);
    cam->set(CAP_PROP_FRAME_WIDTH, 320);
    cam->set(CAP_PROP_FRAME_HEIGHT, 240);
//    m_VideoLabel.move(20, 20);
//    m_VideoLabel.resize(320, 240);
//    m_VideoLabel.show();
    m_UdpTimer->start(150);
    this->exec();
}

void UdpRecvicer::UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size)
{
//    const static int MAX_SIZE = 1472 - 36;
    uint64_t time = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int start = 0; start < size; start += UDP_MAX_SIZE)
    {
        int PacketSize = (start + UDP_MAX_SIZE) <= size ? UDP_MAX_SIZE : (size - start);
        auto m = CreateUdpChatMsg(FriendId, m_MeId, UDPTRANSFERMSG, 0, (data + start), size, start, PacketSize, PacketNum, time);
        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), 9000);
    }
}

//slot:
void UdpRecvicer::UdpSend()
{
    static int count = 0;
    Mat frame;
    std::vector<uchar> buff;
    std::vector<int> param = std::vector<int>(2);
    cam->read(frame);
    param[0] = IMWRITE_JPEG_QUALITY;
    param[1] = 50;
    imencode(".jpg", frame, buff, param);
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
    UdpSendToRemote(0, count++, (char *)buff.data(), buff.size());
}

void UdpRecvicer::UdpRecv()
{
    qDebug() << "start ready read";
    static uint64_t maxTimeOk = 0;
    static int count = 0;
    uint64_t Currenttime = QDateTime::currentDateTime().toMSecsSinceEpoch();
//        for(auto packetMapIt = m_FriendUdpPacketMap.begin(); packetMapIt != m_FriendUdpPacketMap.end(); ++packetMapIt)
//        {
//            auto UdpTimeMap = packetMapIt->first;
//            auto UdpPacketMap = packetMapIt->second;
//            auto end = UdpTimeMap.upper_bound(Currenttime - UDP_MAX_DELAY);
//            for(auto it = UdpTimeMap.begin(); it != end; ++it)
//            {
//                auto packetItemIt = UdpPacketMap.find(it->second);
//                if(packetItemIt != UdpPacketMap.end())
//                {
//                    delete packetItemIt->second;
//                    UdpPacketMap.erase(packetItemIt);

//                }
//            }
//            UdpTimeMap.erase(UdpTimeMap.begin(), end);
//        }

    while(m_UdpSocket.hasPendingDatagrams())
    {
        int size = m_UdpSocket.pendingDatagramSize();
        uint64_t time;
        uint32_t packetNum;
        uint16_t totalSize, packetStart, packetSize;
        UdpPacket *packet;
        auto m = Message::CreateObject();
        m->setRawSize(size);
        m_UdpSocket.readDatagram(m->rawBuf(), size);
        m->refresh();
        auto packetIt = m_FriendUdpPacketMap.find(m->srcId());
        assert(packetIt != m_FriendUdpPacketMap.end());
        std::map<uint64_t, UdpPacket*> &UdpPacketMap = *packetIt;
        m->getUdpInfo(packetNum, totalSize, packetStart, time);
        packetSize = size - sizeof(MsgHead);
        qDebug() << count++ << " " << size << " " << totalSize <<  " " << time;
        if(time > Currenttime - UDP_MAX_DELAY)
        {
            auto it = UdpPacketMap.find(time);
            if(it != UdpPacketMap.end())
                packet = it->second;
            else
            {
                packet = new UdpPacket;
                packet->packetNum = packetNum;
                packet->bitlist = 0;
                packet->size = 0;
                packet->time = time;
                packet->totalSize = totalSize;
                packet->data.resize(totalSize);
                UdpPacketMap.insert(std::pair<uint64_t, UdpPacket*>(time, packet));
            }
            memcpy(packet->data.data() + packetStart, m->data(), packetSize);
            packet->size += packetSize;
            if(packet->size == totalSize && time > maxTimeOk)
            {
//                Mat frame;
                maxTimeOk = time;
//                frame = imdecode(Mat(packet->data), IMREAD_COLOR);
//                m_VideoLabel.setPixmap(QPixmap::fromImage(MatToQImage(frame)).scaled(m_VideoLabel.width(), m_VideoLabel.height()));
                qDebug() << "isok" << " PackNum : " << packetNum << " time:" << time << "current time : " << Currenttime << " vector size : " << packet->data.size();
                auto queueIt = m_FriendDataQueueMap.find(m->srcId());
                if(queueIt != m_FriendDataQueueMap.end())
                {
                    queueIt->second.enqueue(packet);
                }
            }

        }
    }


//        if(maxTimeOk > Currenttime - UDP_MAX_DELAY)
//        {
//            for(auto packetMapIt = m_FriendUdpPacketMap.begin(); packetMapIt != m_FriendUdpPacketMap.end(); ++packetMapIt)
//            {
//                std::list<std::pair<uint64_t, UdpPacket*>> dataList;
//                auto UdpTimeMap = packetMapIt->first;
//                auto UdpPacketMap = packetMapIt->second;
//                auto end = UdpTimeMap.upper_bound(maxTimeOk);
//                for(auto it = UdpTimeMap.begin(); it != end; ++it)
//                {
//                    auto packetItemIt = UdpPacketMap.find(it->second);
//                    if(packetItemIt != UdpPacketMap.end() && packetItemIt->second->totalSize == packetItemIt->second->size)
//                    {
//                        qDebug() << "totalsize : " << packetItemIt->second->totalSize << " time : " << packetItemIt->second->time;
//                        dataList.push_back(std::pair<uint64_t, UdpPacket*>(packetItemIt->second->time, packetItemIt->second));
//                        UdpPacketMap.erase(packetItemIt);
//                    }
//                }
//                UdpTimeMap.erase(UdpTimeMap.begin(), end);
//            }
//        }

}


