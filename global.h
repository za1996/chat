#ifndef GLOBAL_H
#define GLOBAL_H

#include <list>
#include <vector>
#include <memory>
#include <QtNetwork>
#include "message.h"
#include "concurrentqueue.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;


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
MessagePtr CreateDeleteUsersGroups(uint32_t srcID, uint32_t flag, const std::vector<uint32_t> &UsersGroupsVector);
MessagePtr CreateReqUsersGroupInfo(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId);
MessagePtr CreateReqUsersGroupMember(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId);
MessagePtr CreateDelUsersGroupMembers(uint32_t srcID, uint32_t flag, uint32_t GroupId, const std::vector<uint32_t> &MemberList);
MessagePtr CreateFileDataUploadMsg(uint32_t srcID, uint32_t flag, const char* buf, int size, uint32_t FileNum);
MessagePtr CreateSocketMergeMsg(uint32_t flag, uint32_t ChangeId, uint16_t SocketCode);
MessagePtr CreateReadySendProfileMsg(uint32_t srcID, uint32_t flag, uint32_t ClientFileNum, std::string FileName, int FileCode, uint32_t id);
MessagePtr CreateSendProfileEndMsg(uint32_t srcID, uint32_t flag, uint32_t FileNum);
MessagePtr CreateReqDownloadProfile(uint32_t srcID, uint32_t flag, const json& info);
MessagePtr CreateDownloadFileDataMsg(uint32_t srcID, uint32_t flag, uint32_t FileId);
MessagePtr CreateReadySendFileToFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t Size, const std::string &FileName, uint32_t LocalFileNum);
MessagePtr CreateResSendFileByFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t LocalFileNum, uint32_t FileNum);
MessagePtr CreateFileDataTransferMsg(uint32_t srcID, uint32_t destId, uint32_t flag, const char* buf, int size, uint32_t FileNum);
MessagePtr CreateFileTransferContinueMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t FileNum);
MessagePtr CreateFileTransferEndMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t FileNum);
MessagePtr CreateTestMessage(uint32_t srcID, uint32_t flag, const std::string &msg);

typedef std::shared_ptr<UdpPacket> UdpPacketPtr;
typedef UdpPacketPtr VideoPacketPtr;
typedef UdpPacketPtr AudioPacketPtr;
extern QTcpSocket s;
extern QHash<uint32_t, std::map<uint64_t, VideoPacketPtr>> m_FriendVideoUdpPacketMap;
extern QHash<uint32_t, std::map<uint64_t, AudioPacketPtr>> m_FriendAudioUdpPacketMap;
extern std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendVideoDataQueueMap;
extern std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendAudioDataQueueMap;
extern moodycamel::ConcurrentQueue<SendFileItem> m_FileQueue;
extern std::atomic_int32_t m_ClientFileNum;
extern QHash<uint32_t, std::string> m_ClientFileNumMap;
extern QHash<uint32_t, std::string> m_RemoteFileNumMap;
extern QHash<uint32_t, std::shared_ptr<QFile>> m_FileNumStoreMap;
extern QHash<uint32_t, DowloadFileItem> m_DowloadFileMap;
extern QHash<uint32_t, SendFileItem> m_SendFileMap;
extern QHash<uint32_t, std::shared_ptr<QFile>> m_FileNumOpenSendMap;
extern uint32_t m_ThisIsId;
extern uint32_t FileNum;
//#define UDP_MAX_SIZE 14336
#define UDP_MAX_SIZE 1436
#define UDP_MAX_DELAY 200
#define TIMEMAGICNUMER 60000
#define AUDIO_FREQ 8000
#define CACHEPATH "E:/University_Final_Text_Qt_Project/cache"

#endif // GLOBAL_H
