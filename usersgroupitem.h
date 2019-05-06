#ifndef USERSGROUPITEM_H
#define USERSGROUPITEM_H

#include <QWidget>
#include <QLabel>
#include <QBitmap>
#include <QAction>

struct UserGroupInfo
{
    uint32_t Id;
    bool isAdmin;
    QString Name;
    QString Desc;
    QString Profile;
    UserGroupInfo() : Id(0), isAdmin(false) {}
    UserGroupInfo(uint32_t id, bool admin, const QString& name, const QString& desc, const QString& profile) :
        Id(id), isAdmin(admin), Name(name), Desc(desc), Profile(profile)
    {}
};

class UsersGroupItem : public QWidget
{
    Q_OBJECT
public:
    explicit UsersGroupItem(uint32_t GroupId, bool isAdmin, QWidget *parent = nullptr);
    void setProfile(const QString &profile);
    void setName(const QString &name);
    void setDesc(const QString &desc);
    void setDate(const QString &date);
    void setMessageCount(const int count);
    void hideDateLabel();
    void hideMessageCountLabel();
    bool isAdmin() { return m_isAdmin; }

    QAction *m_changeGroupInfo;
    QAction *m_deleteGroup;
    QAction *m_changeGroupMember;

private:
    enum{HEIGHT = 60};
    QLabel *m_Profile;
    QLabel *m_Name;
    QLabel *m_Desc;
    QLabel *m_Date;
    QLabel *m_MsgCount;

    bool m_isAdmin;
    uint32_t m_GroupId;


    QBitmap createEllipseMask(const QSize &size, const QPoint &point, const int rx, const int ry);


protected:
    void resizeEvent(QResizeEvent *event);


signals:

public slots:
    void ChangeProfile(uint32_t, const QString& Profile);
};

typedef std::shared_ptr<UsersGroupItem> UsersGroupItemPtr;

#endif // USERSGROUPITEM_H
