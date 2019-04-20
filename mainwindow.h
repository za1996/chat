#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPaintEvent>
#include <QHash>
#include <stdint.h>
#include <stdint.h>

#include "titlebar.h"
#include "groupiteminfo.h"
#include "message.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(uint32_t UserId, QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum {SPACESIZE = 10, PROFILESIZE = 80};
    QTreeWidget *m_GroupTree;
    TitleBar *m_TitleBar;
    QLabel *m_Profile;
    QLabel *m_UserName;
    QLabel *m_UserDesc;
    QLineEdit *m_SerachLineEdit;
    QWidget *m_MainBoard;
    QAction *m_addGroup;
    QHash<QString, QTreeWidgetItem *> *m_GroupMap;
    QHash<uint32_t, GroupItemInfoPtr> *m_GroupIdMap;
    QHash<uint32_t, GroupItemInfoPtr> *m_FriendsMap;
    QHash<uint32_t, QTreeWidgetItem *> m_FriendsItemMap;
    QHash<uint32_t, std::function<void(MessagePtr)>> m_HandleMap;
    const uint32_t m_UserId;
    GroupItemInfoPtr m_Me;

    enum {CHECKLEN = 12};

    enum Driection
    {
        NONE = 0,
        UP,
        LEFT,
        RIGHT

    };

    bool m_bIsAutoHide;
    Driection m_enDriection;
    int m_nDesktopWidth;



    void AddGroupItem(uint32_t GroupId, uint32_t Id, const QString &Name, const QString &OtherName, const QString &Desc, const QString &Profile, int Sex);
    void AddGroupItem(uint32_t GroupId, const GroupItemInfoPtr& info);
    void DelGroupItem(uint32_t Id, bool clear = true);
    void hideWidget();
    void showWidget();
    void isAutoHide();
    void GetRemoteInfo();
    bool InitMySelf();
    bool InitFriendsGroup();
    bool InitFriends();
    void AddUserGroup(QString &Name, uint32_t GroupID);
    void InitHandle();


    void ShowUserInfo(MessagePtr);
    void ChangeUserName(MessagePtr);
    void AddFriendsGroup(MessagePtr);
    void DeleteFriends(MessagePtr);
    void ChangeFriendsGroup(MessagePtr);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);

signals:
    void HasMessage(uint32_t, std::shared_ptr<Message>);

private slots:
    void onGroupItemClick(QTreeWidgetItem *pitem, int col);
    void onGroupItemDoubleClick(QTreeWidgetItem *pitem, int col);
    void UpdatePos(void);
    void AddUserGroup();
    void DelGroupItem(void *item);
    void ChangeGroupItem(void *item);
    void ChangeGroupItemName(void *item);
    void ShowGroupItemInfo(void *item);
    void MessageRead();
    void HandleMessage(uint32_t, std::shared_ptr<Message>);
};

#endif // MAINWINDOW_H
