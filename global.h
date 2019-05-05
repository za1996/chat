#ifndef GLOBAL_H
#define GLOBAL_H

#include <list>
#include <vector>
#include <memory>
#include <QtNetwork>
#include "message.h"
#include "concurrentqueue.h"
#include <nlohmann/json.hpp>
#include <QSqlDatabase>
using json = nlohmann::json;


int getMessage(QTcpSocket &s, int n, std::list<std::shared_ptr<Message>> &mlist);
bool SendtoRemote(QTcpSocket &s, MessagePtr m);
bool WaitForRead(QTcpSocket &s, std::list<MessagePtr> &mlist, const int count);
QString UserProfileCachePath(uint32_t id, const QString &FileName);
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
MessagePtr CreateDownloadFileDataMsg(uint32_t srcID, uint32_t flag, uint64_t FileId);
MessagePtr CreateReadySendFileToFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t Size, const std::string &FileName, uint64_t LocalFileNum);
MessagePtr CreateResSendFileByFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint64_t FileNum);
MessagePtr CreateFileDataTransferMsg(uint32_t srcID, uint32_t destId, uint32_t flag, const char* buf, int size, uint64_t FileNum);
MessagePtr CreateFileTransferContinueMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint64_t FileNum);
MessagePtr CreateFileTransferEndMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint64_t FileNum);
MessagePtr CreateAddRemoteFriendMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t UserId, const QString& Name);
MessagePtr CreateChangeUsersGroupInfoMsg(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId, const QString &Name, const QString &Desc);
MessagePtr CreateChangeMyselfInfoMsg(uint32_t srcID, uint32_t flag, const QString &Name, const QString &Desc, const QString &Birthday, int Sex);
MessagePtr CreateNewUsersGroupMsg(uint32_t srcID, uint32_t flag, const QString &GroupName);
MessagePtr CreateRealAddFriendMsg(uint32_t srcID, uint32_t flag, uint32_t UserId, uint32_t FriendId);
MessagePtr CreateReqJoinInOtherGroupMsg(uint32_t srcID, uint32_t flag, uint32_t UserId, uint32_t GroupId, const QString &UserName);
MessagePtr CreateAddMemberToGroupMsg(uint32_t srcID, uint32_t flag, uint32_t UserId, uint32_t GroupId);
MessagePtr CreateRegisterAccountMsg(uint32_t flag, const QString &Name, const QString &Password, const QString &Answer);
MessagePtr CreateReqForceCloseFileMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint64_t FileNum);
MessagePtr CreateReadySendOrDownloadUserFileMsg(uint32_t srcID, uint32_t flag, uint64_t ClientFileNum, std::string FileName, int FileCode, uint32_t id);
MessagePtr CreateSendUserFileToServerEndMsg(uint32_t srcID, uint32_t flag, uint64_t FileNum);
MessagePtr CreateReqFriendsStateMsg(uint32_t srcID, uint32_t flag);
MessagePtr CreateReqFilesInfoMsg(uint32_t srcID, uint32_t flag);
MessagePtr CreateFindPasswordMsg(uint32_t flag, uint32_t UserId, const std::string& Answer);
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
extern QHash<uint64_t, std::string> m_ClientFileNumMap;
extern QHash<uint64_t, std::string> m_RemoteFileNumMap;
extern QHash<uint64_t, std::shared_ptr<QFile>> m_FileNumStoreMap;
extern QHash<uint64_t, DowloadFileItem> m_DowloadFileMap;
extern QHash<uint64_t, SendFileItem> m_SendFileMap;
extern QHash<uint64_t, std::shared_ptr<QFile>> m_FileNumOpenSendMap;
extern uint32_t m_ThisIsId;
extern uint64_t GlobalFileNum;
extern QVector<SysMsgCacheItem> m_SysMsgCache;
extern QSqlDatabase db;
//#define UDP_MAX_SIZE 14336
#define UDP_MAX_SIZE 1436
#define UDP_MAX_DELAY 200
#define TIMEMAGICNUMER 60000
#define AUDIO_FREQ 8000
#define CACHEPATH "E:/University_Final_Text_Qt_Project/cache"

#endif // GLOBAL_H
