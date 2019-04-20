#include "groupiteminfo.h"

GroupItemInfo::GroupItemInfo()
{

}

GroupItemInfo::GroupItemInfo(const QString &Name, const uint32_t &Id, const QString &Desc, const QString &OtherName, const QString &Profile, const int &Sex, const uint32_t GroupId, const bool &IsFriendsGroup) :
    m_Name(Name),
    m_Id(Id),
    m_Desc(Desc),
    m_OtherName(OtherName),
    m_IsFriendsGroup(IsFriendsGroup),
    m_Profile(Profile),
    m_Sex(Sex),
    m_GroupId(GroupId),
    m_FriendsList(nullptr)
{
    if(m_IsFriendsGroup)
        m_FriendsList = new std::list<uint32_t>();
}

void GroupItemInfo::addFriend(uint32_t Id)
{
    if(m_FriendsList)
    {
        m_FriendsList->push_back(Id);
    }
}

void GroupItemInfo::delFriend(uint32_t Id)
{
    if(m_FriendsList)
    {
        m_FriendsList->remove(Id);
    }
}

GroupItemInfo::~GroupItemInfo()
{
    if(m_FriendsList) delete m_FriendsList;
}
