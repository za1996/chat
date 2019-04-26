#ifndef GLOBAL_H
#define GLOBAL_H

#include <list>
#include <vector>
#include <memory>
#include <QtNetwork>
#include "message.h"
#include "concurrentqueue.h"

int getMessage(QTcpSocket &s, int n, std::list<std::shared_ptr<Message>> &mlist);
bool SendtoRemote(QTcpSocket &s, MessagePtr m);
bool WaitForRead(QTcpSocket &s, std::list<MessagePtr> &mlist, const int count);
MessagePtr CreateReqUserInfoMsg(uint32_t reqID, uint32_t srcID, uint32_t flag);
MessagePtr CreateChangeUserNameMsg(uint32_t changeID, const QString &Name, uint32_t srcID, uint32_t flag);
MessagePtr CreateAddUserGroupMsg(const QString &UserGroupName, uint32_t srcID, uint32_t flag);
MessagePtr CreateDelFriendsMsg(const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag);
MessagePtr CreateChangeFriendsGroupMsg(uint32_t GroupId, const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag);
MessagePtr CreateReqUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag);
MessagePtr CreateUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const char *data, uint16_t totalSize, uint16_t packetStart, uint16_t packetSize, uint32_t packetNum, uint64_t time);
MessagePtr CreateUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const std::string &str);
MessagePtr CreateChatWordMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const std::string &str, uint64_t time);
typedef std::shared_ptr<UdpPacket> UdpPacketPtr;
typedef UdpPacketPtr VideoPacketPtr;
typedef UdpPacketPtr AudioPacketPtr;
extern QTcpSocket s;
extern QHash<uint32_t, std::map<uint64_t, VideoPacketPtr>> m_FriendVideoUdpPacketMap;
extern QHash<uint32_t, std::map<uint64_t, AudioPacketPtr>> m_FriendAudioUdpPacketMap;
extern std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendVideoDataQueueMap;
extern std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendAudioDataQueueMap;
//#define UDP_MAX_SIZE 14336
#define UDP_MAX_SIZE 1436
#define UDP_MAX_DELAY 200
#define TIMEMAGICNUMER 60000
#define AUDIO_FREQ 8000

#endif // GLOBAL_H
