#ifndef GROUPITEMINFO_H
#define GROUPITEMINFO_H

#include <QString>
#include <QHash>
#include <stdint.h>
#include <list>
#include <memory>

class GroupItemInfo;
typedef std::shared_ptr<GroupItemInfo> GroupItemInfoPtr;

class GroupItemInfo : public std::enable_shared_from_this<GroupItemInfo>
{
public:
    ~GroupItemInfo();

    static GroupItemInfoPtr CreateGroupItem(const QString &Name, const uint32_t &Id)
    {
        GroupItemInfoPtr obj(new GroupItemInfo(Name, Id, "", "", "", 0, 0, true));
        return obj->shared_from_this();
    }

    static GroupItemInfoPtr CreateItem(const QString &Name, const uint32_t &Id, const QString &Desc, const QString &OtherName, const QString &Profile, const int &Sex, const uint32_t &GroupId)
    {
        GroupItemInfoPtr obj(new GroupItemInfo(Name, Id, Desc, OtherName, Profile, Sex, GroupId, false));
        return obj->shared_from_this();
    }

    static GroupItemInfoPtr CreateObject(const QString &Name, const uint32_t &Id, const QString &Desc, const QString &OtherName, const QString &Profile, const int &Sex, const uint32_t GroupId, const bool &IsFriendsGroup = false)
    {
        GroupItemInfoPtr obj(new GroupItemInfo(Name, Id, Desc, OtherName, Profile, Sex, GroupId, IsFriendsGroup));
        return obj->shared_from_this();
    }
    static GroupItemInfoPtr CreateObject()
    {
        GroupItemInfoPtr obj(new GroupItemInfo());
        return obj->shared_from_this();
    }

    QString getName() { return m_Name; }
    uint32_t getId() { return m_Id; }
    QString getOtherName() { return m_OtherName; }
    QString getDesc() { return m_Desc; }
    QString getProfile() { return m_Profile; }
    uint32_t getGroupId() { return m_GroupId; }
    bool IsFriendsGroup() { return m_IsFriendsGroup; }
    int getSex() { return m_Sex; }

    void setName(QString Name) { m_Name = Name; }
    void setId(uint32_t Id) { m_Id = Id; }
    void setOtherName(QString OtherName) { m_OtherName = OtherName; }
    void setDesc(QString Desc) { m_Desc = Desc; }
    void setProfile(QString Profile) { m_Profile = Profile; }
    void setSex(int Sex) { m_Sex = Sex; }

    void addFriend(uint32_t Id);
    void delFriend(uint32_t Id);



private:
    GroupItemInfo();
    GroupItemInfo(const QString &Name, const uint32_t &Id, const QString &Desc, const QString &OtherName, const QString &Profile, const int &Sex, const uint32_t GroupId, const bool &IsFriendsGroup = false);
    QString m_Name;
    uint32_t m_Id;
    QString m_OtherName;
    QString m_Desc;
    QString m_Profile;
    bool m_IsFriendsGroup;
    int m_Sex;
    uint32_t m_GroupId;
    std::list<uint32_t> *m_FriendsList;
};

#endif // GROUPITEMINFO_H
