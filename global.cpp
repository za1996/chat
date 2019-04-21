#include "message.h"

#include <list>
#include <vector>
#include <QtNetwork>
#include <QAbstractSocket>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

QTcpSocket s;

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