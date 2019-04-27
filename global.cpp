#include "message.h"
#include "concurrentqueue.h"
#include "global.h"

#include <list>
#include <vector>
#include <QtNetwork>
#include <QAbstractSocket>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


QTcpSocket s;
QHash<uint32_t, std::map<uint64_t, VideoPacketPtr>> m_FriendVideoUdpPacketMap;
QHash<uint32_t, std::map<uint64_t, AudioPacketPtr>> m_FriendAudioUdpPacketMap;
std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendVideoDataQueueMap;
std::map<uint32_t, moodycamel::ConcurrentQueue<UdpPacketPtr>> m_FriendAudioDataQueueMap;
moodycamel::ConcurrentQueue<std::string> m_FileQueue;

int getMessage(QTcpSocket &s, int n, std::list<std::shared_ptr<Message>> &mlist)
{
    static auto m = Message::CreateObject();
    int count = 0;
    bool all = (n <= 0);
    if(n == 0) goto end;
    while(s.size() >= sizeof(MsgHead) || (m->hasHead() && m->dataSize() >= s.size()))
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
    while(s.waitForReadyRead())
    {
        if(getMessage(s, count, mlist) <= 0) continue;
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

MessagePtr CreateTestMessage(uint32_t srcID, uint32_t flag, const std::string &msg)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, SERVERID, LOGINEVENTGROUP, SYSTESTLOGACTION, flag);
    m->setData(msg);
    return m;
}

MessagePtr CreateFileDataUploadMsg(uint32_t srcID, uint32_t flag, const char* buf, int size)
{
    auto m = Message::CreateObject();
    m->setHead(srcID, SERVERID, FILETRANSFERGROUP, UPLOADFILEDATAACTION, flag);
    m->setData(buf, size);
    return m;
}
