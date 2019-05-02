#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPaintEvent>
#include <QHash>
#include <QUdpSocket>
#include <QListWidget>
#include <stdint.h>
#include <list>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "titlebar.h"
#include "groupiteminfo.h"
#include "message.h"
#include "chatwindow.h"
#include "udprecvicer.h"
#include "usersgroupitem.h"
#include "concurrentqueue.h"
#include "deleteusersgroupmemberwin.h"
#include "tcpfilethread.h"
#include "tcpfilerecvicer.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(uint32_t UserId, QWidget *parent = nullptr);
    ~MainWindow();

    void SignalTest();
    void EmitRecvFileData(uint32_t, uint32_t, int, int);

    void AddSendFile(uint32_t FileNum, uint32_t Id, int FileCode, std::string FileName);
    void AddDownloadFile(uint32_t FileNum, const DowloadFileItem &info);
    void SignalSendFile(uint32_t);
    void CloseFileNum(uint32_t);

private:
    enum {SPACESIZE = 10, PROFILESIZE = 80};
    QTreeWidget *m_GroupTree;
    QListWidget *m_UsersGroupList;
    TitleBar *m_TitleBar;
    QLabel *m_Profile;
    QLabel *m_UserName;
    QLabel *m_UserDesc;
    QLabel m_VideoLabel;
    QLineEdit *m_SerachLineEdit;
    QWidget *m_MainBoard;
    QAction *m_addGroup;
    QAction *m_addUsersGroup;
    QAction *m_addRemoteFriend;
    QAction *m_joinInOtherUsersGroup;
    QPushButton *m_ShowFriendsGroupTreeButton;
    QPushButton *m_ShowUsersGroupListButton;
    DeleteUsersGroupMemberWin *m_DEUGMWindow;


    QHash<QString, QTreeWidgetItem *> *m_GroupMap;
    QHash<uint32_t, GroupItemInfoPtr> *m_GroupIdMap;
    QHash<uint32_t, GroupItemInfoPtr> *m_FriendsMap;
    QHash<uint32_t, QTreeWidgetItem *> m_FriendsItemMap;
    QHash<uint32_t, std::function<void(MessagePtr)>> m_HandleMap;
    QHash<uint32_t, ChatWindow *> m_FriendsChatMap;
    std::list<uint32_t> m_UdpChatList;

    QHash<uint32_t, QListWidgetItem *> m_UsersGroupMap;
    QHash<uint32_t, DeleteUsersGroupMemberWin*> m_DEUGMWindowMap;
    QHash<uint32_t, std::list<MessagePtr>> m_ChatInfoCache;



    const uint32_t m_UserId;
    GroupItemInfoPtr m_Me;
    QTimer m_ShowVideoTimer;
    UdpRecvicer *m_UdpRecvicer;
    TcpFileThread *m_TcpFile;
    TcpFileRecvicer *m_TcpFileRecvicer;



    int m_UdpPort;

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
    bool m_IsDowloadNow;
    QString m_Birthday;


    void GetFriendsProfile();
    void AddGroupItem(uint32_t GroupId, uint32_t Id, const QString &Name, const QString &OtherName, const QString &Desc, const QString &Profile, int Sex);
    void AddGroupItem(uint32_t GroupId, const GroupItemInfoPtr& info);
    void AddUsersGroupItem(uint32_t UsersGroupId, const QString& GroupName, const QString GroupDesc, const QString& GroupProfile, bool);
    void DelGroupItem(uint32_t Id, bool clear = true);
    void hideWidget();
    void showWidget();
    void isAutoHide();
    void GetRemoteInfo();
    bool InitMySelf();
    bool InitFriendsGroup();
    bool InitFriends();
    bool InitUserGroups();
    void InitDir();
    bool CreateDir(QString fullPath);
    void AddUserGroup(const QString &Name, uint32_t GroupID);
    void InitHandle();
    void CreateChatWindow(uint32_t);
    void DelChatWindow(uint32_t);
    void UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size);
    QImage MatToQImage(cv::Mat &mtx);
    void DownloadFile();


    void ShowUserInfo(MessagePtr);
    void ChangeUserName(MessagePtr);
    void AddFriendsGroup(MessagePtr);
    void DeleteFriends(MessagePtr);
    void ChangeFriendsGroup(MessagePtr);
    void ReadyReadUdpData(MessagePtr);
    void AddUdpAddr(MessagePtr);
    void RecvChatData(MessagePtr);
    void DelUsersGroups(MessagePtr);
    void ShowUsersGroupInfo(MessagePtr);
    void ShowUsersGroupMemberList(MessagePtr);
    void DelGroupMemberSuccess(MessagePtr);
    void ReadySendProfile(MessagePtr m);
    void SendFileNumDataContinue(MessagePtr);
    void DownloadFileData(MessagePtr);
    void DownloadFileEnd(MessagePtr);
    void AddNewUsersGroup(MessagePtr);
    void ReqAddFriendMsg(MessagePtr);
    void AddTheNewFriend(MessagePtr);
    void RemoteUserReqJoinInGroup(MessagePtr);
    void AddRemoteNewUsersGroup(MessagePtr);

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
    void Test();
    void FileDataBlockRecv(uint32_t, uint32_t, int, int);
    void ReadyChangeProfile(uint32_t, QString);

private slots:
    void onGroupItemClick(QTreeWidgetItem *pitem, int col);
    void onGroupItemDoubleClick(QTreeWidgetItem *pitem, int col);
    void UpdatePos(void);
    void AddUserGroup();
    void DelGroupItem(void *item);
    void ChangeGroupItem(void *item);
    void ChangeGroupItemName(void *item);
    void ShowGroupItemInfo(void *item);
    void ChangeUsersGroupInfo();
    void RemoveUsersGroupMember();
    void RemoveUsersGroupItem();
    void MessageRead();
    void HandleMessage(uint32_t, std::shared_ptr<Message>);
//    void UdpSend();
//    void UdpRead();
    void RemoveUdpChatFriend(uint32_t);

    void ShowFriendsGroupTree();
    void ShowUserGroupList();
    void HandleRecvFileData(uint32_t, uint32_t, int, int);
    void ChangeProfile(uint32_t, QString);
    void AddRemoteFriend();
    void NewUsersGroup();
    void WantToJoinInOtherGroup();
};

extern MainWindow *m_MainWin;

#endif // MAINWINDOW_H
