#include "message.h"
#include "concurrentqueue.h"
#include "global.h"

#include <list>
#include <vector>
#include <QtNetwork>
#include <QAbstractSocket>



QTcpSocket s;
QHash<uint32_t, std::map<uint64_t, VideoPacketPtr>> m_FriendVideoUdpPacketMap;
QHash<uint32_t, std::map<uint64_t, AudioPacketPtr>> m_FriendAudioUdpPacketMap;
std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendVideoDataQueueMap;
std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendAudioDataQueueMap;
moodycamel::ConcurrentQueue<SendFileItem> m_FileQueue;
std::atomic_int32_t m_ClientFileNum = 0;
QHash<uint32_t, std::string> m_ClientFileNumMap;
QHash<uint32_t, std::string> m_RemoteFileNumMap;
QHash<uint32_t, std::shared_ptr<QFile>> m_FileNumStoreMap;
QHash<uint32_t, DowloadFileItem> m_DowloadFileMap;
QHash<uint32_t, SendFileItem> m_SendFileMap;
QHash<uint32_t, std::shared_ptr<QFile>> m_FileNumOpenSendMap;
uint32_t m_ThisIsId;
uint32_t FileNum = 0xf0000000;

int getMessage(QTcpSocket &s, int n, std::list<std::shared_ptr<Message>> &mlist)
{
    static auto m = Message::CreateObject();
    int count = 0;
    bool all = (n <= 0);
    if(n == 0) goto end;
    qDebug() << "size start : " << s.size();
    while((!m->hasHead() && s.size() >= sizeof(MsgHead)) || (m->hasHead() && m->dataSize() <= s.size()))
    {
        if(m->hasHead())
        {
            QByteArray data = s.read(m->dataSize());
            m->setData(data.data(), data.size());
            mlist.push_back(m);
            m = Message::CreateObject();
            if(all) continue;
            else
            {
                count++;
                if(n <= count)break;
            }
        }
        else
        {
            MsgHead temp;
            s.read((char *)&temp, sizeof(MsgHead));
            m->setHead(temp);
        }
    }
    qDebug() << "size end : " << s.size();

end:
    return count;
}

bool SendtoRemote(QTcpSocket &s, MessagePtr m)
{
    if(s.state() != QAbstractSocket::ConnectedState) return false;
    return s.write((char *)m->tobuf(), m->size()) == m->size();
}

bool WaitForRead(QTcpSocket &s, std::list<MessagePtr> &mlist, const int count)
{
    while(s.size() > 0 || s.waitForReadyRead(10000))
    {
        if(getMessage(s, count, mlist) <= 0)
        {
            if(!s.waitForReadyRead(10000))
            {
                break;
            }
            continue;
        }
        else
        {
            return true;
        }
    }
    return false;
}

MessagePtr CreateReqUserInfoMsg(uint32_t reqID, uint32_t srcID, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["UserID"] = reqID;
    m->setHead(srcID, SERVERID, REQINFOGROUP, REQUSERINFOACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateChangeUserNameMsg(uint32_t changeID, const QString &Name, uint32_t srcID, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["ChangeId"] = changeID;
    info["ChangeName"] = Name.toStdString();
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, CHANGEFRIENDOTHERNAMEACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateAddUserGroupMsg(const QString &UserGroupName, uint32_t srcID, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["AddGroupName"] = UserGroupName.toStdString();
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, ADDFRIENDSGROUPACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateDelFriendsMsg(const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["FriendsId"] = json::array();
    for(int i = 0; i < IDList.size(); ++i)
    {
        info["FriendsId"].push_back(IDList[i]);
    }
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, DELFRIENDSACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateChangeFriendsGroupMsg(uint32_t GroupId, const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupId"] = GroupId;
    info["FriendsId"] = json::array();
    for(int i = 0; i < IDList.size(); ++i)
    {
        info["FriendsId"].push_back(IDList[i]);
    }
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, CHANGEFRIENDSGROUPACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateReqUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag)
{
    auto m = Message::CreateObject();
    json info;
    info["FriendId"] = FriendId;
    m->setHead(srcID, FriendId, REQUDPEVENTGROUP, action, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const char *data, uint16_t totalSize, uint16_t packetStart, uint16_t packetSize, uint32_t packetNum, uint64_t time)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, FriendId, UDPEVENTGROUP, action, flag);
    m->setUdpInfo(totalSize, packetStart, packetNum, time);
    m->setData(data, packetSize);
    return m;
}

MessagePtr CreateUdpChatMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const std::string &str)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, FriendId, UDPEVENTGROUP, action, flag);
    m->setData(str);
    return m;
}

MessagePtr CreateChatWordMsg(uint32_t FriendId, uint32_t srcID, uint32_t action, uint32_t flag, const std::string &str, uint64_t time)
{
    auto m = Message::CreateObject();
    json info;
    info["Msg"] = str;
    info["time"] = time;
    m->setHead(srcID, FriendId, TRANSFERDATAGROUP, action, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateDeleteUsersGroups(uint32_t srcID, uint32_t flag, const std::vector<uint32_t> &UsersGroupsVector)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupsId"] = json::array();
    for(int i = 0; i < UsersGroupsVector.size(); ++i)
    {
        info["GroupsId"].push_back(UsersGroupsVector[i]);
    }
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, DELUSERSGROUPSACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateReqUsersGroupInfo(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupId"] = UsersGroupId;
    m->setHead(srcID, SERVERID, REQINFOGROUP, REQUSERSGROUPINFOACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateReqUsersGroupMember(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupId"] = UsersGroupId;
    m->setHead(srcID, SERVERID, REQINFOGROUP, REQUSERSGROUPMEMBERACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateDelUsersGroupMembers(uint32_t srcID, uint32_t flag, uint32_t GroupId, const std::vector<uint32_t> &MemberList)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupId"] = GroupId;
    info["MembersId"] = json::array();
    for(int i = 0; i < MemberList.size(); ++i)
    {
        info["MembersId"].push_back(MemberList[i]);
    }
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, DELUSERSGROUPMEMBERSACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateSocketMergeMsg(uint32_t flag, uint32_t ChangeId, uint16_t SocketCode)
{
    auto m = Message::CreateObject();
    json info;
    info["ChangeId"] = ChangeId;
    info["SocketCode"] = SocketCode;
    m->setHead(REQID, SERVERID, LOGINEVENTGROUP, SOCKETMERGEACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateFileDataUploadMsg(uint32_t srcID, uint32_t flag, const char* buf, int size, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, UPLOADFILEDATAACTION, flag);
    m->setTcpFileNum(FileNum);
    m->setData(buf, size);
    return m;
}

MessagePtr CreateReadySendProfileMsg(uint32_t srcID, uint32_t flag, uint32_t ClientFileNum, std::string FileName, int FileCode, uint32_t id)
{
    auto m = Message::CreateObject();
    json info;
    info["FileName"] = FileName;
    info["ClientFileNum"] = ClientFileNum;
    info["FileCode"] = FileCode;
    info["Id"] = id;
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, READYSENDPROFILEACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateSendProfileEndMsg(uint32_t srcID, uint32_t flag, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    json info;
    info["FileNum"] = FileNum;
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, WRITEPROFILEENDACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateReqDownloadProfile(uint32_t srcID, uint32_t flag, const json& info)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, REQREADFILESOPENACTION, flag);
    m->setData(info.dump());
    return m;
}


MessagePtr CreateDownloadFileDataMsg(uint32_t srcID, uint32_t flag, uint32_t FileId)
{
    auto m = Message::CreateObject();
    json info;
    info["FileNum"] = FileId;
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, REQREADFILEDATAACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateReadySendFileToFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t Size, const std::string &FileName, uint32_t LocalFileNum)
{
    auto m = Message::CreateObject();
    json info;
    info["SenderFileNum"] = LocalFileNum;
    info["Name"] = FileName;
    info["FileSize"] = Size;
    m->setHead(srcID, destId, TRANSFERDATAGROUP, TRANSFERREQFILESENDACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateResSendFileByFriend(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t LocalFileNum, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    json info;
    info["SenderFileNum"] = LocalFileNum;
    info["FileNum"] = FileNum;
    m->setHead(srcID, destId, TRANSFERDATAGROUP, RESTRANSFERFILEACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateFileDataTransferMsg(uint32_t srcID, uint32_t destId, uint32_t flag, const char* buf, int size, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, destId, RESFILESENDETHREADRGROUP, TRANSFERFILEDATAACTION, flag);
    m->setTcpFileNum(FileNum);
    m->setData(buf, size);
    return m;
}

MessagePtr CreateFileTransferContinueMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    json info;
    info["FileNum"] = FileNum;
    m->setHead(srcID, destId, TRANSFERDATAGROUP, TRANSFERFILECONTINUE, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateFileTransferEndMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t FileNum)
{
    auto m = Message::CreateObject();
    json info;
    info["FileNum"] = FileNum;
    m->setHead(srcID, destId, TRANSFERDATAGROUP, TRANSFERFILEENDACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateAddRemoteFriendMsg(uint32_t srcID, uint32_t destId, uint32_t flag, uint32_t UserId)
{
    auto m = Message::CreateObject();
    json info;
    info["UserId"] = UserId;
    m->setHead(srcID, destId, TRANSFERDATAGROUP, ADDREMOTEFRIENDACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateChangeUsersGroupInfoMsg(uint32_t srcID, uint32_t flag, uint32_t UsersGroupId, const QString &Name, const QString &Desc)
{
    auto m = Message::CreateObject();
    json info;
    info["UsersGroupId"] = UsersGroupId;
    info["UsersGroupName"] = Name.toStdString();
    info["UsersGroupDesc"] = Desc.toStdString();
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, CHANGEUSERSGROUPINFO, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateChangeMyselfInfoMsg(uint32_t srcID, uint32_t flag, const QString &Name, const QString &Desc, const QString &Birthday, int Sex)
{
    auto m = Message::CreateObject();
    json info;
    info["Name"] = Name.toStdString();
    info["Desc"] = Desc.toStdString();
    info["Birthday"] = Birthday.toStdString();
    info["Sex"] = Sex;
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, CHANGEMYSELFINFOACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateNewUsersGroupMsg(uint32_t srcID, uint32_t flag, const QString &GroupName)
{
    auto m = Message::CreateObject();
    json info;
    info["GroupName"] = GroupName.toStdString();
    m->setHead(srcID, SERVERID, CHANGEINFOGROUP, CREATENEWUSERSGROUPACTION, flag);
    m->setData(info.dump());
    return m;
}

MessagePtr CreateTestMessage(uint32_t srcID, uint32_t flag, const std::string &msg)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, SERVERID, LOGINEVENTGROUP, SYSTESTLOGACTION, flag);
    m->setData(msg);
    return m;
}
