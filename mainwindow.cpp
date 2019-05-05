#include "mainwindow.h"
#include "groupitem.h"
#include "personalinfo.h"
#include "message.h"
#include "global.h"
#include "chatwindow.h"
#include "usersgroupitem.h"
#include <nlohmann/json.hpp>
#include "groupiteminfo.h"
#include "usersgroupinfo.h"
#include "systemmessagewin.h"
#include "networkspacewin.h"
#include <functional>
#include <iostream>

#include <QDebug>
#include <QPainter>
#include <QBitmap>
#include <QPainterPath>
#include <QColor>
#include <QtCore/qmath.h>
#include <QScrollBar>
#include <QInputDialog>
#include <QAction>
#include <QStringList>
#include <QList>
#include <QGraphicsDropShadowEffect>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QApplication>
#include <assert.h>
#include <QMessageBox>
#include <QTimer>
#include <map>
#include <QThread>

using nlohmann::json;
using namespace cv;
using moodycamel::ConcurrentQueue;

MainWindow *m_MainWin;

MainWindow::MainWindow(uint32_t UserId, QWidget *parent) :
    QWidget(parent)
  , m_bIsAutoHide(false)
  , m_enDriection(NONE)
  , m_UserId(UserId)
  , m_UdpPort(-1)
  , m_UdpRecvicer(nullptr)
  , m_TcpFileRecvicer(nullptr)
  , m_IsDowloadNow(false)
{
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    //init
    m_ThisIsId = UserId;
    m_MainBoard = new QWidget(this);
    m_TitleBar = new TitleBar(this);
    m_GroupTree = new QTreeWidget(m_MainBoard);
    m_UsersGroupList = new QListWidget(m_MainBoard);
    m_Profile = new QLabel(m_MainBoard);
    m_UserName = new QLabel(m_MainBoard);
    m_UserDesc = new QLabel(m_MainBoard);
    m_SerachLineEdit = new QLineEdit(m_MainBoard);
    m_ShowFriendsGroupTreeButton = new QPushButton(m_MainBoard);
    m_ShowUsersGroupListButton = new QPushButton(m_MainBoard);
    m_NetworkSpaceButton = new  QPushButton(m_MainBoard);
    m_GroupMap = new QHash<QString, QTreeWidgetItem *>();
    m_GroupIdMap = new QHash<uint32_t, GroupItemInfoPtr>();
    m_FriendsMap = new QHash<uint32_t, GroupItemInfoPtr>();

    m_MainBoard->setObjectName("mainwindow");
    m_MainBoard->resize(320, 680);
    m_MainBoard->setMaximumSize(320, 680);
    m_MainBoard->setMinimumSize(320, 680);
    qDebug() << "m_mainboard width : " << m_MainBoard->width();
    this->resize(320 + 2 * SPACESIZE, 680 + 2 * SPACESIZE);
    this->setMaximumSize(320 + 2 * SPACESIZE, 680 + 2 * SPACESIZE);
    this->setMinimumSize(320 + 2 * SPACESIZE, 680 + 2 * SPACESIZE);
    this->setStyleSheet("QWidget#mainwindow{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);

    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(m_MainBoard->width());
    m_TitleBar->setWindowBorderWidth(SPACESIZE * 2);
    m_TitleBar->setButtonType(MIN_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");

    m_GroupTree->setHeaderHidden(true);
    m_GroupTree->setExpandsOnDoubleClick(false);// 关闭双击自动展开.
    m_GroupTree->setRootIsDecorated(false);
    m_GroupTree->setIndentation(0);
    m_GroupTree->setContentsMargins(0, 0, 0, 0);
    m_GroupTree->resize(m_MainBoard->width(), m_MainBoard->height() * 0.65);
    m_GroupTree->setStyleSheet("QTreeWidget{border-style:none;   }QTreeWidget::item{background-color:rgba(255,255,255,0%);}QTreeWidget::branch{image:none;}");
    QFile file("D:/temp/scrollbar.css");
    file.open(QFile::ReadOnly);
    m_GroupTree->verticalScrollBar()->setStyleSheet(file.readAll());
    m_GroupTree->setVerticalScrollMode(QTreeWidget::ScrollPerPixel); //设置滚动条滚动的模式:按照像素滚动
    m_GroupTree->verticalScrollBar()->setSingleStep(10);//获取滚动条并且设置每一步骤滚动10像素
    m_GroupTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_addGroup = new QAction(tr("&添加分组"), m_GroupTree);
    m_addRemoteFriend = new QAction(tr("&添加好友"), m_GroupTree);
    m_GroupTree->addAction(m_addGroup);
    m_GroupTree->addAction(m_addRemoteFriend);
    m_GroupTree->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_UsersGroupList->setStyleSheet("QListWidget{border-style:none;   }QListWidget::item{background-color:rgba(255,255,255,0%);}");
    m_UsersGroupList->verticalScrollBar()->setStyleSheet(file.readAll());
    m_UsersGroupList->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    m_UsersGroupList->verticalScrollBar()->setSingleStep(10);
    m_UsersGroupList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_UsersGroupList->hide();
    m_addUsersGroup = new QAction(tr("&新建群组"), m_UsersGroupList);
    m_joinInOtherUsersGroup = new QAction(tr("&加入群组"), m_UsersGroupList);
    m_UsersGroupList->addAction(m_addUsersGroup);
    m_UsersGroupList->addAction(m_joinInOtherUsersGroup);
    m_UsersGroupList->setContextMenuPolicy(Qt::ActionsContextMenu);



    m_ShowFriendsGroupTreeButton->setText("好友");
    m_ShowUsersGroupListButton->setText("群组");
    m_NetworkSpaceButton->setText("网盘");


    m_SerachLineEdit->resize(m_MainBoard->width(), 30);
    m_SerachLineEdit->setStyleSheet("QLineEdit::focus{ background-color: white; border-bottom:1px solid rgb(200,200,200);}QLineEdit{  border-style:none;background-color: rgba(255, 255, 255, 25%); }");
    m_SerachLineEdit->setPlaceholderText("搜索 : 号码 / 名称 / 备注");
    m_SerachLineEdit->installEventFilter(this);

    m_Profile->resize(PROFILESIZE, PROFILESIZE);
    m_Profile->setObjectName("profile");
    m_Profile->installEventFilter(this);
    QBitmap mask(m_Profile->size());
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 255));
    painter.drawRoundedRect(mask.rect(), 15, 15);
    m_Profile->setMask(mask);
    m_Profile->setStyleSheet("QLabel#profile{border-image : url(\"D:/59888355_p0.png\");}");

    m_UserName->setText("UserName");

    m_UserDesc->setText("UserDesc");

    GlobalFileNum |= ((uint64_t)m_UserId << 32);


//    m_UdpSocket.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 4);

    UpdatePos();

    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), qApp, SLOT(quit()));
    connect(m_GroupTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemClick(QTreeWidgetItem*,int)));
    connect(m_GroupTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemDoubleClick(QTreeWidgetItem*,int)));
    connect(m_addGroup, SIGNAL(triggered(bool)), this, SLOT(AddUserGroup()));
    connect(m_addRemoteFriend, SIGNAL(triggered(bool)), this, SLOT(AddRemoteFriend()));
    connect(m_addUsersGroup, SIGNAL(triggered(bool)), this, SLOT(NewUsersGroup()));
    connect(m_joinInOtherUsersGroup, SIGNAL(triggered(bool)), this, SLOT(WantToJoinInOtherGroup()));
    connect(this, SIGNAL(FileDataBlockRecv(uint64_t, uint32_t, int, int)), this, SLOT(HandleRecvFileData(uint64_t, uint32_t, int, int)));
//    connect(&m_ShowVideoTimer, SIGNAL(timeout()), this, SLOT(ShowVideo()));


//    connect(&m_UdpTimer, SIGNAL(timeout()), this, SLOT(UdpSend()));
//    connect(&m_UdpSocket, SIGNAL(readyRead()), this, SLOT(UdpRead()));
    connect(m_ShowFriendsGroupTreeButton, SIGNAL(clicked(bool)), this, SLOT(ShowFriendsGroupTree()));
    connect(m_ShowUsersGroupListButton, SIGNAL(clicked(bool)), this, SLOT(ShowUserGroupList()));
    connect(m_NetworkSpaceButton, SIGNAL(clicked(bool)), this, SLOT(ShowNetworkSpaceWin()));
    connect(this, SIGNAL(ReadyChangeProfile(uint32_t,QString)), this, SLOT(ChangeProfile(uint32_t,QString)));
    qDebug() << "TitleBar Height" << m_TitleBar->height() << endl;

//    QTreeWidgetItem *pRootFriendItem = new QTreeWidgetItem();
//    QLabel *pItemName = new QLabel(m_GroupTree);
//    pItemName->setText("我的好友");
//    pRootFriendItem->setSizeHint(0, QSize(0, 13));
//    m_GroupTree->addTopLevelItem(pRootFriendItem);
//    m_GroupMap->insert(pItemName->text(), pRootFriendItem);



//    for(int i = 0; i < 10; i++)
//    {
//        AddGroupItem(pRootFriendItem);
//    }
    std::list<MessagePtr> Cache;
    qDebug() << "start thread";
    m_TcpFile = new TcpFileThread();
    m_TcpFile->SetUserId(m_UserId);
    m_TcpFile->start(QThread::HighPriority);
    qDebug() << "start m_TcpFile";
    QThread::sleep(1);
    WaitForRead(s, Cache, 1);
    m_TcpFileRecvicer = new TcpFileRecvicer();
    m_TcpFileRecvicer->SetUserId(m_UserId);
    m_TcpFileRecvicer->start(QThread::HighPriority);
    QThread::sleep(1);
    WaitForRead(s, Cache, 1);
    qDebug() << "start m_TcpFileRecvicer";
    InitHandle();
    GetRemoteInfo();
    InitDir();
    m_GroupTree->collapseAll();
}

MainWindow::~MainWindow()
{
//    delete m_GroupTree;
//    delete m_TitleBar;
//    delete m_Profile;
//    delete m_UserName;
//    delete m_UserDesc;
//    delete m_SerachLineEdit;
//    delete m_addGroup;
//    delete m_GroupMap;
//    delete m_GroupIdMap;
//    delete m_FriendsMap;

}

void MainWindow::EmitRecvFileData(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    qDebug() << __FUNCTION__;
    emit FileDataBlockRecv(FileNum, Id, FileCode, Size);
}

void MainWindow::EmitSendFileData(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    qDebug() << __FUNCTION__;
    emit FileDataBlockSend(FileNum, Id, FileCode, Size);
}

void MainWindow::AddSendFile(uint64_t FileNum, uint32_t Id, int FileCode, std::string FileName)
{
    m_TcpFile->AddFile(FileNum, Id, FileCode, FileName);
}

void MainWindow::AddDownloadFile(uint64_t FileNum, const DowloadFileItem &info)
{
    m_DowloadFileMap.insert(FileNum, info);
    m_IsDowloadNow = true;
    DownloadFile();
}

void MainWindow::SignalSendFile(uint64_t FileNum)
{
    m_TcpFile->send(FileNum);
}

void MainWindow::CloseSendFileNum(uint64_t FileNum, bool force)
{
    m_FileNumOpenSendMap.remove(FileNum);
    auto it = m_SendFileMap.find(FileNum);
//    assert(it != m_SendFileMap.end());
    if(it != m_SendFileMap.end())
    {
        if(!force)
        {
            if(it->FileCode == UPLOADUSERPROFILE)
            {
                emit ReadyChangeProfile(it->Id, QString::fromStdString(it->FileName));
            }
            else if(it->FileCode == FILEDATATRANSFER)
            {
                emit FileTransferEnd(FileNum);
            }
            else if(it->FileCode == UPLOADUSERFILE)
            {
                emit UserFileSendOrDownloadEnd(FileNum);
            }
            else
            {

            }
        }
        m_SendFileMap.erase(it);
    }
}

void MainWindow::CloseDownloadFileNum(uint64_t FileNum, bool force)
{
    m_FileNumStoreMap.remove(FileNum);
//    auto fit = m_FileNumStoreMap.find(FileNum);
//    assert(fit != m_FileNumStoreMap.end());
//    if(fit != m_FileNumStoreMap.end())
//    {
//        qDebug() << "close";
//        (*fit)->close();
//    }
//    m_FileNumStoreMap.erase(fit);
    auto it = m_DowloadFileMap.find(FileNum);
//    assert(it != m_DowloadFileMap.end());
    if(it != m_DowloadFileMap.end())
    {
        if(!force)
        {
            if(it->FileCode == DOWNLOADUSERPROFILE)
            {
                emit ReadyChangeProfile(it->Id, QString("%1%2").arg(QString::fromStdString(it->LocalPath)).arg(QString::fromStdString(it->Name)));
            }
            else if(it->FileCode == DOWNLOADUSERFILE)
            {
                emit UserFileSendOrDownloadEnd(FileNum);
            }
        }
        else
        {
            QFile::remove(QString("%1%2").arg(QString::fromStdString(it->LocalPath)).arg(QString::fromStdString(it->Name)));
        }
        m_DowloadFileMap.erase(it);
    }
    DownloadFile();
}


//virtual fun
bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == m_SerachLineEdit)
    {
        if(ev->type() == QEvent::FocusOut)
        {
            qDebug("lineEdit FocusOut");
            m_SerachLineEdit->clear();
        }
        else if(ev->type() == QEvent::FocusIn)
        {
            qDebug("lineEdit FocusIn");
        }
    }
    else if(obj == m_Profile)
    {
        if (ev->type() == QEvent::MouseButtonPress)//mouse button pressed
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                PersonalInfo *p = new PersonalInfo(nullptr, m_Me->getName(), m_UserId, m_Me->getSex(), m_Birthday.split(' ')[0], m_Me->getDesc(), m_Me->getProfile(), true);
                p->show();
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return this->QWidget::eventFilter(obj, ev);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
    this->QWidget::paintEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

}

//实现靠边隐藏 enterEvent leaveEvent isAutoHide hideWidget showWidget
void MainWindow::leaveEvent(QEvent *event)
{
    isAutoHide();
    if (m_bIsAutoHide)
    {
        hideWidget();
    }
}

void MainWindow::enterEvent(QEvent *event)
{
    if (m_bIsAutoHide)
    {
        showWidget();
    }
}

void MainWindow::hideWidget()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    QPoint pos = QPoint(this->geometry().x(), this->geometry().y());
    QSize size = this->size();
    animation->setDuration(100);
    animation->setStartValue(QRect(pos, size));

    QRect rcEnd;
    if (m_enDriection == UP)
    {
        rcEnd = QRect(pos.x(), -size.height() + 2, size.width(), size.height());
    }
    else if (m_enDriection == LEFT)
    {
        rcEnd = QRect(-size.width() + CHECKLEN, pos.y(), size.width(), size.height());
    }
    else if (m_enDriection == RIGHT)
    {
        rcEnd = QRect(m_nDesktopWidth - CHECKLEN, pos.y(), size.width(), size.height());
    }
    animation->setEndValue(rcEnd);
    animation->start();
}

void MainWindow::showWidget()
{
    QPoint pos = QPoint(this->geometry().x(), this->geometry().y());
    QSize size = this->size();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(100);
    animation->setStartValue(QRect(pos, size));

    QRect rcEnd;
    if (m_enDriection == UP)
    {
        rcEnd = QRect(pos.x(), -CHECKLEN, size.width(), size.height());
    }
    else if (m_enDriection == LEFT)
    {
        rcEnd = QRect(-CHECKLEN, pos.y(), size.width(), size.height());
    }
    else if (m_enDriection == RIGHT)
    {
        rcEnd = QRect(m_nDesktopWidth - size.width(), pos.y() + CHECKLEN, size.width(), size.height());
    }
    animation->setEndValue(rcEnd);
    animation->start();
}

void MainWindow::isAutoHide()
{
    QDesktopWidget * desktop = QApplication::desktop();
    QPoint pos = QPoint(this->geometry().x(), this->geometry().y());
    QSize size = this->size();

    qDebug() << pos.x() << " " << pos.y();

    m_bIsAutoHide = false;
    m_enDriection = NONE;
    m_nDesktopWidth = desktop->width();

    if(pos.y() <= CHECKLEN)
    {
        m_bIsAutoHide = true;
        m_enDriection = UP;
    }
    else if(pos.x() <= CHECKLEN)
    {
        m_bIsAutoHide = true;
        m_enDriection = LEFT;
    }
    else if(pos.x() + size.width() >= m_nDesktopWidth - CHECKLEN)
    {
        m_bIsAutoHide = true;
        m_enDriection = RIGHT;
    }
}

//消息处理函数
void MainWindow::ShowUserInfo(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    QString Name = QString::fromStdString(info["Name"].get<std::string>());
    QString Desc = QString::fromStdString(info["Desc"].get<std::string>());
    QString Profile = QString::fromStdString(info["Profile"].get<std::string>());
    QString Date = QString::fromStdString(info["Birthday"].get<std::string>());
    int Sex = info["Sex"].get<int>();
    uint32_t id = info["ID"].get<json::number_unsigned_t>();
    PersonalInfo *p = new PersonalInfo(nullptr, Name, id, Sex, Date.split(' ')[0], Desc, Profile, false);
    p->show();
}

void MainWindow::ChangeUserName(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    QString Name = QString::fromStdString(info["FriendOtherName"].get<std::string>());
    uint32_t id = info["ChangeId"].get<json::number_unsigned_t>();
    auto it = m_FriendsItemMap.find(id);
    if(it != m_FriendsItemMap.end())
    {
        GroupItem *item = dynamic_cast<GroupItem *>((*it)->treeWidget()->itemWidget(*it, 0));
        item->setName(Name);
    }
}

void MainWindow::AddFriendsGroup(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    QString Name = QString::fromStdString(info["FriendsGroupName"].get<std::string>());
    uint32_t id = info["ID"].get<json::number_unsigned_t>();
    AddUserGroup(Name, id);
}

void MainWindow::DeleteFriends(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    info = info["FriendsId"];
    for(int i = 0; i < info.size(); ++i)
    {
        uint32_t id = info[i].get<json::number_unsigned_t>();
        DelGroupItem(id);
    }
}

void MainWindow::ChangeFriendsGroup(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint32_t NewGroupId = info["GroupId"].get<json::number_unsigned_t>();
    info = info["FriendsId"];
    for(int i = 0; i < info.size(); ++i)
    {
        uint32_t id = info[i].get<json::number_unsigned_t>();
        auto it = m_FriendsMap->find(id);
        assert(it != m_FriendsMap->end());
        DelGroupItem(id, false);
        AddGroupItem(NewGroupId, *it);
    }
}

//开启udp发送和接收线程 开始先缓慢发包让服务器记住地址
void MainWindow::AddUdpAddr(MessagePtr m)
{
    QDebug q = qDebug();
    q << __FUNCTION__;
    m->operator <<(q);
    json info = json::parse((char *)m->data());
    uint32_t FriendId = info["FriendId"].get<json::number_unsigned_t>();
    m_UdpPort = info["Port"].get<int>();
    auto it = m_FriendsChatMap.find(FriendId);
    if(m_FriendsChatMap.end() != it)
    {
        q << "in Timer start ";
        q << m_UdpChatList.size();
        if(!m_UdpChatList.size() && !m_UdpRecvicer)
        {
            q << "recv start";
            m_FriendVideoUdpPacketMap.insert(FriendId, std::map<uint64_t, UdpPacketPtr>());
            m_FriendVideoDataQueueMap.insert(std::pair<uint32_t, ConcurrentQueue<UdpPacketPtr>>(FriendId, ConcurrentQueue<UdpPacketPtr>()));
            m_FriendAudioUdpPacketMap.insert(FriendId, std::map<uint64_t, UdpPacketPtr>());
            m_FriendAudioDataQueueMap.insert(std::pair<uint32_t, ConcurrentQueue<UdpPacketPtr>>(FriendId, ConcurrentQueue<UdpPacketPtr>()));
//            UdpRecvicer *recvicer = new UdpRecvicer;
            m_UdpRecvicer = new UdpRecvicer;
            m_UdpRecvicer->SetMeId(m_UserId);
            m_UdpRecvicer->SetFriendId(FriendId);
            m_UdpRecvicer->SetPort(m_UdpPort);
            m_UdpRecvicer->start(QThread::HighPriority);
        }


        if((*it)->isUdpChatNow())
            m_UdpChatList.push_back(FriendId);

//        emit HasMessage(FriendId, m);
    }
}

void MainWindow::ReadyReadUdpData(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint32_t FriendId = info["FriendId"].get<json::number_unsigned_t>();
    if(m_UdpRecvicer)
    {
        m_UdpRecvicer->ReadyStart();
        emit HasMessage(FriendId, m);
    }
}

void MainWindow::RecvChatData(MessagePtr m)
{
    uint32_t FriendId = m->srcId();
    auto it = m_ChatInfoCache.find(FriendId);
    if(it != m_ChatInfoCache.end())
    {
        (*it).push_back(m);
        auto item = m_FriendsItemMap.find(FriendId);
        assert(item != m_FriendsItemMap.end());
        if(item != m_FriendsItemMap.end())
        {
            auto FriendItem = dynamic_cast<GroupItem *>(m_GroupTree->itemWidget((*item), 0));
            assert(FriendItem != nullptr);
            FriendItem->setMessageCount((*it).size());
        }
    }
}

void MainWindow::DelUsersGroups(MessagePtr m)
{
    if(true)
    {
        //后期要判断聊天窗口是否打开
        //单对单聊天也一样
        json info = json::parse((char *)m->data());
        json Groups = info["Groups"];
        for(int i = 0; i < Groups.size(); ++i)
        {
            uint32_t id = Groups[i]["GroupId"].get<json::number_unsigned_t>();
            auto it = m_UsersGroupMap.find(id);
            if(it != m_UsersGroupMap.end())
            {
                m_UsersGroupList->removeItemWidget(it.value());
                delete it.value();
                m_UsersGroupMap.erase(it);
            }

        }

    }
}

void MainWindow::ShowUsersGroupInfo(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    QString Name = QString::fromStdString(info["GroupName"].get<std::string>());
    QString Desc = QString::fromStdString(info["GroupDesc"].get<std::string>());
    QString Profile = QString::fromStdString(info["GroupProfile"].get<std::string>());
    uint32_t id = info["GroupId"].get<json::number_unsigned_t>();
    uint32_t admin = info["AdminId"].get<json::number_unsigned_t>();
    UsersGroupInfo *w = new UsersGroupInfo(nullptr, Name, id, Desc, Profile, admin == m_UserId);
    w->show();
}

void MainWindow::ShowUsersGroupMemberList(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint32_t GroupId = info["GroupId"].get<json::number_unsigned_t>();
    std::map<uint32_t, QString> map;
    info = info["Members"];
    qDebug() << __FUNCTION__;
    for(int i = 0; i < info.size(); ++i)
    {
        QString Name = QString::fromStdString(info[i]["UserName"].get<std::string>());
        QString OtherNameInGroup = QString::fromStdString(info[i]["OtherNameInGroup"].get<std::string>());
        uint32_t id = info[i]["UserId"].get<json::number_unsigned_t>();
        if(id == m_UserId)continue;
        map.insert(std::pair<uint32_t, QString>(id, QString("%1(%2)").arg(Name).arg(OtherNameInGroup)));
    }
    qDebug() << __FUNCTION__;

    m_DEUGMWindow = new DeleteUsersGroupMemberWin(nullptr, map, GroupId, m_UserId);
    m_DEUGMWindow->show();
    connect(this, SIGNAL(HasMessage(uint32_t,std::shared_ptr<Message>)), m_DEUGMWindow, SLOT(HandMessage(uint32_t,std::shared_ptr<Message>)));
//    m_DEUGMWindowMap.insert(GroupId, m_DEUGMWindow);
}

void MainWindow::DelGroupMemberSuccess(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint32_t GroupId = info["GroupId"].get<json::number_unsigned_t>();
    if(info["MembersId"].size() == 1 && info["MembersId"][0].get<json::number_unsigned_t>() == m_UserId)
    {
        auto it = m_UsersGroupMap.find(GroupId);
        assert(it != m_UsersGroupMap.end());
        m_UsersGroupList->removeItemWidget(*it);
        m_UsersGroupMap.erase(it);

    }
}

void MainWindow::ReadySendProfile(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint64_t ClientFileNum = info["ClientFileNum"].get<json::number_unsigned_t>();
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    uint32_t Id = info["Id"].get<json::number_unsigned_t>();
    int code = info["FileCode"].get<json::number_unsigned_t>();
    auto it = m_ClientFileNumMap.find(ClientFileNum);
    assert(it != m_ClientFileNumMap.end());
    if(it != m_ClientFileNumMap.end())
    {
        m_RemoteFileNumMap.insert(FileNum, *it);
        m_TcpFile->AddFile(FileNum, Id, code, *it);
        m_ClientFileNumMap.erase(it);
    }

}

void MainWindow::SendFileNumDataContinue(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint64_t FileNum = info["FileNum"];
    m_TcpFile->send(FileNum);
}

void MainWindow::DownloadFileData(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    info = info["Files"];
    for(int i = 0; i < info.size(); ++i)
    {
        qDebug() << __FUNCTION__;
        int FileCode = info[i]["FileCode"].get<int>();
        uint32_t Id = info[i]["Id"].get<json::number_unsigned_t>();
        int Size = info[i]["Length"].get<int>();
        std::string Name = info[i]["FileName"].get<std::string>();
        uint64_t FileNum = info[i]["FileNum"].get<json::number_unsigned_t>();
        std::string Path;
        if(FileCode == DOWNLOADUSERPROFILE)
        {
            Path = QString("%1/%2/%3/").arg(CACHEPATH).arg(Id).arg("profile").toStdString();
        }
        else
        {
            continue;
        }
        m_DowloadFileMap.insert(FileNum, DowloadFileItem(FileNum, Id, Size, FileCode, Name, Path));
    }
    m_IsDowloadNow = true;
    DownloadFile();
}

void MainWindow::DownloadFileEnd(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    CloseDownloadFileNum(FileNum, false);
}

void MainWindow::AddNewUsersGroup(MessagePtr m)
{
    qDebug() << __FUNCTION__;
    json info = json::parse((char *)m->data());
    uint32_t GroupId = info["GroupID"].get<json::number_unsigned_t>();
    std::string GroupName = info["GroupName"].get<std::string>();
    AddUsersGroupItem(GroupId, QString::fromStdString(GroupName), "", "", true);
}

void MainWindow::ReqAddFriendMsg(MessagePtr m)
{
    SysMsgCacheItem sysitem(false, false, true, m);
    m_SysMsgCache.push_back(sysitem);
    auto item = m_FriendsItemMap.find(0);
    assert(item != m_FriendsItemMap.end());
    if(item != m_FriendsItemMap.end())
    {
        GroupItem *p = dynamic_cast<GroupItem *>(m_GroupTree->itemWidget(*item, 0));
        assert(p != nullptr);
        if(p != nullptr)
        {
            p->setMessageCount(1);
        }
    }


}

void MainWindow::AddTheNewFriend(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint32_t FriendId = info["ID"].get<json::number_unsigned_t>();
    uint32_t GroupId = info["GroupId"].get<json::number_unsigned_t>();
    QString FriendName = QString::fromStdString(info["Name"].get<std::string>());
    QString FriendProfile = QString::fromStdString(info["Name"].get<std::string>());
    QString FriendDesc = QString::fromStdString(info["Desc"].get<std::string>());
    int Sex = info["Sex"].get<int>();

    QString path = QString("%1/%2/%3").arg(CACHEPATH).arg(FriendId).arg("profile");
    CreateDir(path);
    path = QString("%1/%2/%3").arg(CACHEPATH).arg(FriendId).arg("recvfile");
    CreateDir(path);

    AddGroupItem(GroupId, FriendId, FriendName, "", FriendDesc, FriendProfile, Sex);

    //请求头像
}

void MainWindow::RemoteUserReqJoinInGroup(MessagePtr m)
{
    SysMsgCacheItem sysitem(false, false, true, m);
    m_SysMsgCache.push_back(sysitem);
    auto item = m_FriendsItemMap.find(0);
    assert(item != m_FriendsItemMap.end());
    if(item != m_FriendsItemMap.end())
    {
        GroupItem *p = dynamic_cast<GroupItem *>(m_GroupTree->itemWidget(*item, 0));
        assert(p != nullptr);
        if(p != nullptr)
        {
            p->setMessageCount(1);
        }
    }
}

void MainWindow::AddRemoteNewUsersGroup(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint32_t GroupId = info["GroupId"].get<json::number_unsigned_t>();
    QString GroupName = QString::fromStdString(info["GroupName"].get<std::string>());
    QString GroupDesc = QString::fromStdString(info["GroupDesc"].get<std::string>());
    QString GroupProfile = QString::fromStdString(info["GroupProfile"].get<std::string>());
    uint32_t AdminId = info["AdminId"].get<json::number_unsigned_t>();

    QString path = QString("%1/%2/%3/%4").arg(CACHEPATH).arg("groups").arg(GroupId).arg("profile");
    CreateDir(path);
    AddUsersGroupItem(GroupId, GroupName, GroupDesc, GroupProfile, AdminId == m_UserId);

    //更新头像
}

void MainWindow::HandleResSendToFileServer(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    int FileCode = info["FileCode"].get<int>();
    if(FileCode == UPLOADUSERFILE)
    {
        emit ResSendToServer(m);
    }
    else if(FileCode == DOWNLOADUSERFILE)
    {
        emit ResRecvFromServer(m);
    }
    else
    {

    }
}

void MainWindow::ResUserFilesInfo(MessagePtr m)
{
    emit NetworkSpaceRefresh(m);
}

void MainWindow::SignalTest()
{
    qDebug() << __FUNCTION__;
    emit Test();
}

//private

void MainWindow::DownloadFile()
{
    qDebug() << __FUNCTION__;
    if(m_IsDowloadNow)
    {
        if(!m_FileNumStoreMap.size())
        {
            qDebug() << " map size " << m_DowloadFileMap.size();
            if(m_DowloadFileMap.size())
            {
                auto it = m_DowloadFileMap.begin();
                QString FullName = QString("%1%2").arg(QString::fromStdString((*it).LocalPath)).arg(QString::fromStdString((*it).Name));
                qDebug() << "FullName : " << FullName;
                std::shared_ptr<QFile> file(new QFile(FullName));
                file->open(QIODevice::WriteOnly | QIODevice::Truncate);
                m_FileNumStoreMap.insert((*it).FileNum, file);
                int FileCode = (*it).FileCode;
                MessagePtr m;
                if(FileCode == FILEDATATRANSFER)
                {
                    m = CreateResSendFileByFriend(m_UserId, (*it).Id, 0, (*it).FileNum);
                }
                else
                {
                    m = CreateDownloadFileDataMsg(m_UserId, 0, (*it).FileNum);
                }
                SendtoRemote(s, m);
            }
            else
            {
                m_IsDowloadNow = false;
            }
        }
    }
}

void MainWindow::GetFriendsProfile()
{
    json info;
    info["Friends"] = json::array();
    info["UsersGroups"] = json::array();
    for(auto it = m_FriendsMap->begin(); it != m_FriendsMap->end(); ++it)
    {
        json item;
        uint32_t Id = (*it)->getId();
        QString Profile = (*it)->getProfile();
        int FileCode = DOWNLOADUSERPROFILE;
        if(!Profile.isEmpty())
        {
            item["Id"] = Id;
            item["FileName"] = Profile.toStdString();
            item["FileCode"] = FileCode;
            info["Friends"].push_back(item);
        }
    }
    json item;
    uint32_t Id = m_Me->getId();
    QString Profile = m_Me->getProfile();
    int FileCode = DOWNLOADUSERPROFILE;
    if(!Profile.isEmpty())
    {
        item["Id"] = Id;
        item["FileName"] = Profile.toStdString();
        item["FileCode"] = FileCode;
        info["Friends"].push_back(item);
    }
    auto m = CreateReqDownloadProfile(m_UserId, 0, info);
    SendtoRemote(s, m);
}


void MainWindow::InitHandle()
{
    m_HandleMap.insert(MESSAGETYPE(RESINFOGROUP, RESUSERINFOCODE), std::bind(&MainWindow::ShowUserInfo, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESINFOGROUP, RESUSERSGROUPINFOCODE), std::bind(&MainWindow::ShowUsersGroupInfo, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESINFOGROUP, RESUSERSGROUPMEMBERCODE), std::bind(&MainWindow::ShowUsersGroupMemberList, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESINFOGROUP, RESUSERFILESINFOCODE), std::bind(&MainWindow::ResUserFilesInfo, this, std::placeholders::_1));


    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESCHANGEFRIENDOTHERNAMECODE), std::bind(&MainWindow::ChangeUserName, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESADDFRIENDSGROUPCODE), std::bind(&MainWindow::AddFriendsGroup, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESDELFRIENDSCODE), std::bind(&MainWindow::DeleteFriends, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESCHANGEFRIENDSGROUPCODE), std::bind(&MainWindow::ChangeFriendsGroup, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESDELUSERSGROUPSOKCODE), std::bind(&MainWindow::DelUsersGroups, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESDELMEMBERSUCCEESCODE), std::bind(&MainWindow::DelGroupMemberSuccess, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESCREATENEWUSERSGROUPCODE), std::bind(&MainWindow::AddNewUsersGroup, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESREALADDFRIENDCODE), std::bind(&MainWindow::AddTheNewFriend, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESADDMEMBERTOGROUPCODE), std::bind(&MainWindow::AddRemoteNewUsersGroup, this, std::placeholders::_1));

    m_HandleMap.insert(MESSAGETYPE(RESUDPREQGROUP, RESSENDUDPENDPOINTCODE), std::bind(&MainWindow::AddUdpAddr, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESUDPREQGROUP, RESREADYUDPCHATSENDCODE), std::bind(&MainWindow::ReadyReadUdpData, this, std::placeholders::_1));

    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERCHATDATAACTION), std::bind(&MainWindow::RecvChatData, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERFILEENDACTION), std::bind(&MainWindow::DownloadFileEnd, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, ADDREMOTEFRIENDACTION), std::bind(&MainWindow::ReqAddFriendMsg, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, REQJOININGROUPACTION), std::bind(&MainWindow::RemoteUserReqJoinInGroup, this, std::placeholders::_1));

    m_HandleMap.insert(MESSAGETYPE(RESFILETRANSDERINFOGROUP, RESREADYPROFILETRANSER), std::bind(&MainWindow::ReadySendProfile, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESFILETRANSDERINFOGROUP, RESUPLOADDATACOUNTCODE), std::bind(&MainWindow::SendFileNumDataContinue, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESFILETRANSDERINFOGROUP, RESREADFILESOPENCODE), std::bind(&MainWindow::DownloadFileData, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESFILETRANSDERINFOGROUP, RESREADFILEENDCODE), std::bind(&MainWindow::DownloadFileEnd, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESFILETRANSDERINFOGROUP, RESREDAYSENDORDOWNLOADUSERFILECODE), std::bind(&MainWindow::HandleResSendToFileServer, this, std::placeholders::_1));
}


void MainWindow::UpdatePos()
{
    m_MainBoard->move(SPACESIZE, SPACESIZE);
    m_TitleBar->move(m_MainBoard->pos().x(), m_MainBoard->pos().y());
    m_Profile->move(SPACESIZE, m_TitleBar->height() + SPACESIZE);
    m_UserName->adjustSize();
    m_UserName->move(m_Profile->pos().x() + m_Profile->width() + SPACESIZE, m_Profile->pos().y());
    m_UserDesc->adjustSize();
    m_UserDesc->move(m_UserName->pos().x(), m_UserName->pos().y() + m_UserName->height() + SPACESIZE);
    m_SerachLineEdit->move(0, m_Profile->pos().y() + m_Profile->height() + SPACESIZE);
    m_GroupTree->move(0, m_SerachLineEdit->pos().y() + m_SerachLineEdit->height());

    m_UsersGroupList->move(m_GroupTree->pos());
    m_UsersGroupList->resize(m_GroupTree->size());

    m_ShowFriendsGroupTreeButton->move(m_GroupTree->x(), m_GroupTree->y() + m_GroupTree->height());
    m_ShowFriendsGroupTreeButton->resize(m_MainBoard->width() * 0.5, 20);

    m_ShowUsersGroupListButton->move(m_ShowFriendsGroupTreeButton->x() + m_ShowFriendsGroupTreeButton->width(), m_ShowFriendsGroupTreeButton->y());
    m_ShowUsersGroupListButton->resize(m_ShowFriendsGroupTreeButton->size());
    m_ShowFriendsGroupTreeButton->show();
    m_ShowUsersGroupListButton->show();

    m_NetworkSpaceButton->resize(40, 40);
    m_NetworkSpaceButton->move(0, m_MainBoard->height() - m_NetworkSpaceButton->height());
    m_NetworkSpaceButton->show();
}

QImage MainWindow::MatToQImage(cv::Mat &mtx)
{
//    switch (mtx.type())
//    {
//    case CV_8UC1:
//        {
//            qDebug() << "CV_8UC1";
//            QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols, QImage::Format_Grayscale8);
//            return img;
//        }
//        break;
//    case CV_8UC3:
//        {
//            qDebug() << "CV_8UC3";
//            QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols * 3, QImage::Format_RGB888);
//            return img.rgbSwapped();
//        }
//        break;
//    case CV_8UC4:
//        {
//            qDebug() << "CV_8UC4";
//            QImage img((const unsigned char *)(mtx.data), mtx.cols, mtx.rows, mtx.cols * 4, QImage::Format_ARGB32);
//            return img;
//        }
//        break;
//    default:
//        {
//            qDebug() << "DEFAULT";
//            QImage img;
//            return img;
//        }
//        break;
//    }
    cv::Mat temp; // make the same cv::Mat
    cvtColor(mtx, temp, COLOR_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

void MainWindow::DelGroupItem(uint32_t Id, bool clear)
{
    auto it = m_FriendsItemMap.find(Id);
    if(it != m_FriendsItemMap.end())
    {
        QTreeWidgetItem *p = (*it);
        assert(p && p->parent());
        auto info = m_FriendsMap->find(Id);
        if(info != m_FriendsMap->end())
        {
            (*m_GroupIdMap->find((*info)->getGroupId()))->delFriend(Id);
        }

        p->parent()->removeChild(p);
        m_FriendsItemMap.erase(it);
        if(clear)m_FriendsMap->remove(Id);
    }
}

void MainWindow::AddGroupItem(uint32_t GroupId, uint32_t Id, const QString &Name, const QString &OtherName, const QString &Desc, const QString &Profile, int Sex)
{
    QTreeWidgetItem *pitem = new QTreeWidgetItem();
    GroupItem *ptable = new GroupItem(nullptr, pitem, Id);
    auto it = m_GroupIdMap->find(GroupId);
    const QString &GroupName = (*it)->getName();
    auto parentIt = m_GroupMap->find(GroupName);
    pitem->setData(0, Qt::UserRole, Id);
    (*parentIt)->addChild(pitem);
    ptable->setProfile("D:/imggggg.bmp");
    if(OtherName != "")ptable->setName(OtherName);
    else ptable->setName(Name);
    ptable->setDesc(Desc);
    ptable->hideDateLabel();
    ptable->hideMessageCountLabel();
    pitem->setSizeHint(0, QSize(0, ptable->height()));
    qDebug() << "GroupItem height : " << ptable->height() << endl;
    m_GroupTree->setItemWidget(pitem, 0, ptable);
    m_FriendsItemMap.insert(Id, pitem);
    (*parentIt)->setExpanded(true);
    (*it)->addFriend(Id);
    if(m_FriendsMap->find(Id) == m_FriendsMap->end()) m_FriendsMap->insert(Id, GroupItemInfo::CreateItem(Name, Id, Desc, OtherName, Profile, Sex, GroupId));
    connect(ptable, SIGNAL(deleteFriendSign(void*)), this, SLOT(DelGroupItem(void*)));
    connect(ptable, SIGNAL(changeGroupSign(void*)), this, SLOT(ChangeGroupItem(void*)));
    connect(ptable, SIGNAL(changeNameSign(void*)), this, SLOT(ChangeGroupItemName(void*)));
    connect(ptable, SIGNAL(showInfoSign(void*)), this, SLOT(ShowGroupItemInfo(void*)));
}

void MainWindow::AddGroupItem(uint32_t GroupId, const GroupItemInfoPtr &info)
{
    AddGroupItem(GroupId, info->getId(), info->getName(), info->getOtherName(), info->getDesc(), info->getProfile(), info->getSex());
}


void MainWindow::AddUserGroup(const QString &Name, uint32_t GroupID)
{
    QTreeWidgetItem *pRootFriendItem = new QTreeWidgetItem();
    QLabel *pItemName = new QLabel(m_GroupTree);
    pItemName->setText(Name);
    pRootFriendItem->setData(0, Qt::UserRole, GroupID);
    pRootFriendItem->setSizeHint(0, QSize(0, 26));
    m_GroupTree->addTopLevelItem(pRootFriendItem);
    m_GroupTree->setItemWidget(pRootFriendItem, 0, pItemName);
    m_GroupMap->insert(Name, pRootFriendItem);
    m_GroupIdMap->insert(GroupID, GroupItemInfo::CreateGroupItem(Name, GroupID));
}

void MainWindow::AddUsersGroupItem(uint32_t UsersGroupId, const QString &GroupName, const QString GroupDesc, const QString &GroupProfile, bool isAdmin)
{
    QListWidgetItem *plitem = new QListWidgetItem();
    UsersGroupItem *item = new UsersGroupItem(isAdmin);
    item->setProfile("D:/imggggg.bmp");
    item->setDesc(GroupDesc);
    item->setName(GroupName);
    item->hideDateLabel();
    item->hideMessageCountLabel();
    plitem->setSizeHint(QSize(0, item->height()));
    plitem->setData(Qt::UserRole, UsersGroupId);
    m_UsersGroupList->addItem(plitem);
    m_UsersGroupList->setItemWidget(plitem, item);
    m_UsersGroupMap.insert(UsersGroupId, plitem);
    connect(item->m_changeGroupInfo, SIGNAL(triggered(bool)), this, SLOT(ChangeUsersGroupInfo()));
    connect(item->m_deleteGroup, SIGNAL(triggered(bool)), this, SLOT(RemoveUsersGroupItem()));
    if(isAdmin)
    {
        connect(item->m_changeGroupMember, SIGNAL(triggered(bool)) ,this, SLOT(RemoveUsersGroupMember()));
    }

}


void MainWindow::GetRemoteInfo()
{
    if(InitMySelf() && InitFriendsGroup() && InitFriends() && InitUserGroups())
    {
        qDebug() << "connect";
        connect(this, SIGNAL(HasMessage(uint32_t,std::shared_ptr<Message>)), this, SLOT(HandleMessage(uint32_t,std::shared_ptr<Message>)));
        connect(&s, SIGNAL(readyRead()), this, SLOT(MessageRead()));
        GetFriendsProfile();
        return;
    }
error:
    //网络异常则会运行到这里
    qDebug() << "error\n";
    this->close();
}

void MainWindow::CreateChatWindow(uint32_t FriendId)
{
    auto it = m_FriendsChatMap.find(FriendId);
    if(it == m_FriendsChatMap.end())
    {
        auto fit = m_FriendsMap->find(FriendId);
        assert(fit != m_FriendsMap->end());
        ChatWindow *w = new ChatWindow(m_UserId, FriendId, (*fit)->getShowName());
        connect(this, SIGNAL(HasMessage(uint32_t,std::shared_ptr<Message>)), w, SLOT(HandleMessage(uint32_t,std::shared_ptr<Message>)));
//        connect(this, SIGNAL(ChatWindowReadyReadUdp(uint32_t)), w, SLOT(StartReadyShowVideo(uint32_t)));
        connect(w, SIGNAL(ChatWindowUdpChatEnd(uint32_t)), this, SLOT(RemoveUdpChatFriend(uint32_t)));
        connect(this, SIGNAL(FileDataBlockSend(uint64_t,uint32_t,int,int)), w, SLOT(HandleSendOrRecvSignal(uint64_t,uint32_t,int,int)));
        connect(this, SIGNAL(FileDataBlockRecv(uint64_t,uint32_t,int,int)), w, SLOT(HandleSendOrRecvSignal(uint64_t,uint32_t,int,int)));
        connect(this, SIGNAL(FileTransferEnd(uint64_t)), w, SLOT(HandleFileSendedEnd(uint64_t)));
        m_FriendsChatMap.insert(FriendId, w);
        w->SetDelHandle(std::bind(&MainWindow::DelChatWindow, this, std::placeholders::_1));
        w->show();
    }
    else
    {
        (*it)->raise();
    }
    qDebug() << __FUNCTION__ << m_FriendsChatMap;
}

void MainWindow::DelChatWindow(uint32_t FriendId)
{
    m_FriendsChatMap.remove(FriendId);
    m_UdpChatList.remove(FriendId);
    qDebug() << __FUNCTION__ << m_FriendsChatMap;
}

bool MainWindow::InitMySelf()
{
    auto m = Message::CreateObject();
    json info;
    info["UserID"] = m_UserId;
    m->setHead(m_UserId, SERVERID, REQINFOGROUP, REQUSERINFOACTION, 0);
    m->setData(info.dump());
    SendtoRemote(s, m);
    std::list<MessagePtr> mlist;
    if(WaitForRead(s, mlist, 1))
    {
        m = mlist.front();
        QDebug q = qDebug();
        m->operator <<(q);
        json info = json::parse((char *)m->data());
        if(m->isError())
        {
            std::string str = info["Msg"].get<std::string>();
            QMessageBox::information(nullptr, "Error", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return false;
        }
        QString Name = QString::fromStdString(info["Name"].get<std::string>());
        QString Desc = QString::fromStdString(info["Desc"].get<std::string>());
        QString Profile = QString::fromStdString(info["Profile"].get<std::string>());
        m_Birthday = QString::fromStdString(info["Birthday"].get<std::string>());
        int Sex = info["Sex"].get<int>();
        m_Me = GroupItemInfo::CreateObject(Name, m_UserId, Desc, "", Profile, Sex, 0);
        m_UserName->setText(Name);
        m_UserDesc->setText(Desc);
        return true;

    }
    return false;
}

bool MainWindow::InitFriendsGroup()
{
    auto m = Message::CreateObject();
    m->setHead(m_UserId, SERVERID, REQINFOGROUP, REQFRIENDSGROUPACTION, 0);
    SendtoRemote(s, m);
    std::list<MessagePtr> mlist;
    if(WaitForRead(s, mlist, 1))
    {
        m = mlist.front();
        QDebug q = qDebug();
        m->operator <<(q);
        AddUserGroup(QString("系统消息"), 0);
        json info = json::parse((char *)m->data());
        if(m->isError())
        {
            std::string str = info["Msg"].get<std::string>();
            QMessageBox::information(nullptr, "Error", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return false;
        }
        json FriendArray = info["FriendGroups"];
        for(int i = 0; i < FriendArray.size(); ++i)
        {
            QString Name = QString::fromStdString(FriendArray[i]["GroupName"].get<std::string>());
            uint32_t id = FriendArray[i]["GroupId"].get<json::number_unsigned_t>();
            AddUserGroup(Name, id);
        }
        return true;
    }
    return false;
}

bool MainWindow::InitFriends()
{
    auto m = Message::CreateObject();
    m->setHead(m_UserId, SERVERID, REQINFOGROUP, REQFRIENDSACTION, 0);
    SendtoRemote(s, m);
    std::list<MessagePtr> mlist;
    if(WaitForRead(s, mlist, 1))
    {
        m = mlist.front();
        QDebug q = qDebug();
        m->operator <<(q);
        json info = json::parse((char *)m->data());
        if(m->isError())
        {
            std::string str = info["Msg"].get<std::string>();
            QMessageBox::information(nullptr, "Error", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return false;
        }
        json Friends = info["Friends"];
        for(int i = 0; i < Friends.size(); ++i)
        {
            QString Name = QString::fromStdString(Friends[i]["FriendName"].get<std::string>());
            QString OtherName = QString::fromStdString(Friends[i]["FriendOtherName"].get<std::string>());
            QString Desc = QString::fromStdString(Friends[i]["Desc"].get<std::string>());
            QString Profile = QString::fromStdString(Friends[i]["Profile"].get<std::string>());
            uint32_t Id = Friends[i]["FriendID"].get<json::number_unsigned_t>();
            uint32_t GroupId = Friends[i]["FriendGroupID"].get<json::number_unsigned_t>();
            int Sex = Friends[i]["Sex"].get<int>();
            qDebug() << GroupId << " " << Id << " " << Name << " " << OtherName << " " << Desc << " " << Profile << " " << Sex;
            AddGroupItem(GroupId, Id, Name, OtherName, Desc, Profile, Sex);
            m_ChatInfoCache.insert(Id, std::list<MessagePtr>());

        }
        AddGroupItem(0, 0, "系统消息", "", "", "", 0);
        return true;
    }
    return false;
}

bool MainWindow::InitUserGroups()
{
    auto m = Message::CreateObject();
    m->setHead(m_UserId, SERVERID, REQINFOGROUP, REQUSERSGROUPSACTION, 0);
    SendtoRemote(s, m);
    std::list<MessagePtr> mlist;
    if(WaitForRead(s, mlist, 1))
    {
        m = mlist.front();
        QDebug q = qDebug();
        qDebug() << "operator start";
        m->operator <<(q);
        json info = json::parse((char *)m->data());
        if(m->isError())
        {
            std::string str = info["Msg"].get<std::string>();
            QMessageBox::information(nullptr, "Error", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return false;
        }
        json Friends = info["UsersGroups"];
        for(int i = 0; i < Friends.size(); ++i)
        {
            QString Name = QString::fromStdString(Friends[i]["GroupName"].get<std::string>());
            QString Desc = QString::fromStdString(Friends[i]["Intro"].get<std::string>());
            QString Profile = QString::fromStdString(Friends[i]["GroupProfile"].get<std::string>());
            uint32_t Id = Friends[i]["GroupID"].get<json::number_unsigned_t>();
            uint32_t AdminId = Friends[i]["AdminID"].get<json::number_unsigned_t>();
            AddUsersGroupItem(Id, Name, Desc ,Profile, AdminId == m_UserId);
            qDebug() << "operator end";

        }
        return true;
    }
    return false;
}

void MainWindow::InitDir()
{
    auto ulist = m_FriendsMap->keys();
    for(auto it = ulist.begin(); it != ulist.end(); ++it)
    {
        QString path = QString("%1/%2/%3").arg(CACHEPATH).arg(*it).arg("profile");
        CreateDir(path);
        path = QString("%1/%2/%3").arg(CACHEPATH).arg(*it).arg("recvfile");
        CreateDir(path);
    }

    auto glist = m_UsersGroupMap.keys();
    for(auto it = glist.begin(); it != glist.end(); ++it)
    {
        QString path = QString("%1/%2/%3/%4").arg(CACHEPATH).arg("groups").arg(*it).arg("profile");
        CreateDir(path);
    }
}

bool MainWindow::CreateDir(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    else
    {
       bool ok = dir.mkpath(fullPath);//创建多级目录
       return ok;
    }
}



//slot
void MainWindow::HandleMessage(uint32_t id, std::shared_ptr<Message> m)
{
    qDebug() << "HandleMessage " << id;
    if(SERVERID == id || m_FriendsChatMap.find(id) == m_FriendsChatMap.end())
    {
        auto it = m_HandleMap.find(m->Type());
        if(it != m_HandleMap.end())
        {
            qDebug() << __FUNCTION__;
            (*it)(m);
        }
    }
    qDebug() << "HandleMessage end";
}

void MainWindow::MessageRead()
{
    QDebug q = qDebug();
    qDebug() << "Message Read Start";
    std::list<std::shared_ptr<Message>> mlist;
    int n = 16;
    while(getMessage(s, n, mlist) > 0)
    {
        for(auto it = mlist.begin(); it != mlist.end(); ++it)
        {
            (*it)->operator <<(q);
            emit HasMessage((*it)->srcId(), *it);
        }
        mlist.clear();
    }
    qDebug() << "Message Read end";
}

void MainWindow::AddUserGroup()
{
    bool isOK;
    QString text = QInputDialog::getText(this, "Input Dialog",
                                                           "Please input your comment",
                                                           QLineEdit::Normal,
                                                           "your comment",
                                                           &isOK);
    if(isOK && !m_GroupMap->count(text))
    {
        auto m = CreateAddUserGroupMsg(text, m_UserId, 0);
        SendtoRemote(s, m);
    }
}


void MainWindow::onGroupItemClick(QTreeWidgetItem *pitem, int col)
{
    pitem->setExpanded(!pitem->isExpanded());
}

void MainWindow::onGroupItemDoubleClick(QTreeWidgetItem *pitem, int col)
{
    GroupItem *item = dynamic_cast<GroupItem *>(pitem->treeWidget()->itemWidget(pitem, col));
    if(item)
    {
        item->hideDateLabel();
        item->hideMessageCountLabel();
        // open the chat
        // write code
        uint32_t Id = pitem->data(0, Qt::UserRole).value<uint32_t>();
        if(Id)
        {
            CreateChatWindow(Id);
        }
        else
        {
            SystemMessageWin *w = new SystemMessageWin();
            w->show();
        }
    }
}

void MainWindow::DelGroupItem(void *item)
{
//    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
//    assert(p && p->parent());

//    p->parent()->removeChild(p);
//    qDebug() << "delete end";
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    uint32_t id = p->data(0, Qt::UserRole).value<uint32_t>();
    std::vector<uint32_t> list = {id};
    auto m = CreateDelFriendsMsg(list, m_UserId, 0);
    SendtoRemote(s, m);

}

void MainWindow::ChangeGroupItem(void *item)
{
    bool isOK;
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    assert(p && p->parent());

    QStringList stritems(m_GroupMap->keys());

    QString Name = QInputDialog::getItem(this, "SelectGroup", "Group:", stritems, 0, false, &isOK);
    if(isOK && m_GroupMap->count(Name))
    {
        uint32_t id = p->data(0, Qt::UserRole).value<uint32_t>();
        auto it = m_GroupMap->find(Name);
        if(it != m_GroupMap->end())
        {
            uint32_t groupid = (*it)->data(0, Qt::UserRole).value<uint32_t>();
            std::vector<uint32_t> list = {id};
            auto m = CreateChangeFriendsGroupMsg(groupid, list, m_UserId, 0);
            SendtoRemote(s, m);
        }
    }

}

void MainWindow::ChangeGroupItemName(void *item)
{
    bool isOK;
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    assert(p);
    QString text = QInputDialog::getText(this, "Input Dialog",
                                                           "Please input your comment",
                                                           QLineEdit::Normal,
                                                           "your comment",
                                                           &isOK);
    if(isOK)
    {
        uint32_t id = p->data(0, Qt::UserRole).value<uint32_t>();
        auto m = CreateChangeUserNameMsg(id, text, m_UserId, 0);
        SendtoRemote(s, m);
    }
}

void MainWindow::ShowGroupItemInfo(void *item)
{
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    uint32_t id = p->data(0, Qt::UserRole).value<uint32_t>();
    qDebug() << id;
    auto m = CreateReqUserInfoMsg(id, m_UserId, 0);
    SendtoRemote(s, m);

}

void MainWindow::ChangeUsersGroupInfo()
{
    qDebug() << __FUNCTION__;
    auto m = CreateReqUsersGroupInfo(m_UserId, 0, m_UsersGroupList->currentItem()->data(Qt::UserRole).value<uint32_t>());
    SendtoRemote(s, m);

}

void MainWindow::RemoveUsersGroupMember()
{
    qDebug() << __FUNCTION__;
    auto m = CreateReqUsersGroupMember(m_UserId, 0, m_UsersGroupList->currentItem()->data(Qt::UserRole).value<uint32_t>());
    SendtoRemote(s, m);
}

void MainWindow::RemoveUsersGroupItem()
{
    qDebug() << __FUNCTION__;
    QListWidgetItem* item = m_UsersGroupList->currentItem();
    auto p = dynamic_cast<UsersGroupItem*>(m_UsersGroupList->itemWidget(item));
    assert(p != nullptr);
    if(p->isAdmin())
    {
        std::vector<uint32_t> ids = { item->data(Qt::UserRole).value<uint32_t>() };
        auto m = CreateDeleteUsersGroups(m_UserId, 0, ids);
        SendtoRemote(s, m);
    }
    else
    {
        std::vector<uint32_t> MemberId = {m_UserId};
        auto m = CreateDelUsersGroupMembers(m_UserId, 0, item->data(Qt::UserRole).value<uint32_t>(), MemberId);
        SendtoRemote(s, m);
    }
}

void MainWindow::RemoveUdpChatFriend(uint32_t id)
{
    m_UdpChatList.remove(id);
    if(!m_UdpChatList.size())
    {
        m_FriendVideoUdpPacketMap.clear();
        m_FriendVideoDataQueueMap.clear();
        m_FriendAudioUdpPacketMap.clear();
        m_FriendAudioDataQueueMap.clear();
        if(m_UdpRecvicer)
        {
            m_UdpRecvicer->terminate();
            m_UdpRecvicer->wait();
        }
        qDebug() << "m_UdpRecvicer thread stop";
        delete m_UdpRecvicer;
        m_UdpRecvicer = nullptr;
    }
}

void MainWindow::ShowFriendsGroupTree()
{
    m_GroupTree->show();
    m_GroupTree->raise();
}

void MainWindow::ShowUserGroupList()
{
    m_UsersGroupList->show();
    m_UsersGroupList->raise();
}

void MainWindow::HandleRecvFileData(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    qDebug() << __FUNCTION__;
    qDebug() << "FileNum : " << FileNum << " Id : " << Id << " FileCode : " << FileCode << " Size : " << Size;
    MessagePtr m;
    if(FileCode == FILEDATATRANSFER)
    {
        m = CreateFileTransferContinueMsg(m_UserId, Id, 0, FileNum);
    }
    else
    {
        m = CreateDownloadFileDataMsg(m_UserId, 0, FileNum);
    }
    SendtoRemote(s, m);
}

void MainWindow::ChangeProfile(uint32_t Id, QString FullPath)
{
    if(m_UserId == Id)
    {
        m_Me->setProfile(FullPath.split('/').last());
        m_Profile->setPixmap(QPixmap::fromImage(QImage(FullPath)).scaled(m_Profile->size()));
    }
}

void MainWindow::AddRemoteFriend()
{
    bool isOK;
    uint32_t Id = QInputDialog::getInt(this, "Input", "输入用户账号", 0, 0, 2147483647, 1, &isOK);
    if(isOK)
    {
        auto m = CreateAddRemoteFriendMsg(m_UserId, Id, 0, m_UserId, m_Me->getName());
        SendtoRemote(s, m);
        QMessageBox message(QMessageBox::NoIcon, "message", tr("申请好友请求"));
        message.open();
    }
}

void MainWindow::NewUsersGroup()
{
    bool isOK;
    QString text = QInputDialog::getText(this, "Input", "请输入群组名", QLineEdit::Normal, "我的群组", &isOK);
    if(isOK)
    {
        auto m = CreateNewUsersGroupMsg(m_UserId, 0, text);
        SendtoRemote(s, m);
    }
}

void MainWindow::WantToJoinInOtherGroup()
{
    bool isOK;
    uint32_t GroupId = QInputDialog::getInt(this, "Input", "输入群组ID", 0, 0, 2147483647, 1, &isOK);
    if(isOK)
    {
        auto m = CreateReqJoinInOtherGroupMsg(m_UserId, 0, m_UserId, GroupId, m_Me->getName());
        SendtoRemote(s, m);
        QMessageBox message(QMessageBox::NoIcon, "message", tr("申请群组请求"));
        message.open();
    }
}

void MainWindow::ShowNetworkSpaceWin()
{
    NetworkSpaceWin *w = new NetworkSpaceWin();
    connect(this, SIGNAL(FileDataBlockSend(uint64_t,uint32_t,int,int)), w, SLOT(HandleSendOrRecvSignal(uint64_t,uint32_t,int,int)));
    connect(this, SIGNAL(ResSendToServer(MessagePtr)), w, SLOT(HandleReqSendMessage(MessagePtr)));
    connect(this, SIGNAL(ResRecvFromServer(MessagePtr)), w, SLOT(HandleReqRecvMessage(MessagePtr)));
    connect(this, SIGNAL(UserFileSendOrDownloadEnd(uint64_t)), w, SLOT(HandleSendFileEnd(uint64_t)));
    connect(this, SIGNAL(NetworkSpaceRefresh(MessagePtr)), w, SLOT(ShowFilesInfo(MessagePtr)));
    connect(this, SIGNAL(FileDataBlockRecv(uint64_t,uint32_t,int,int)), w, SLOT(HandleSendOrRecvSignal(uint64_t,uint32_t,int,int)));
    w->show();
}

//void MainWindow::UdpSend()
//{
//    static int count = 0;
//    static std::string str(10000, 'a');
//    if(!m_UdpChatList.size())
//    {
//        m_UdpTimer.stop();
//        return;
//    }
//    else
//    {
//        int FriendId = m_UdpChatList.front();
//        auto m = CreateUdpChatMsg(FriendId, m_UserId, UDPTRANSFERMSG, 0, str);
//        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), m_UdpPort);
//    }
//    UdpSendToRemote(0, count++, str.c_str(), str.size() + 1);
//}

//void MainWindow::UdpRead()
//{
//    static uint64_t maxTimeOk = 0;
//    static int count = 0;
//    uint64_t Currenttime = QDateTime::currentDateTime().toMSecsSinceEpoch();
////        for(auto packetMapIt = m_FriendUdpPacketMap.begin(); packetMapIt != m_FriendUdpPacketMap.end(); ++packetMapIt)
////        {
////            auto UdpTimeMap = packetMapIt->first;
////            auto UdpPacketMap = packetMapIt->second;
////            auto end = UdpTimeMap.upper_bound(Currenttime - UDP_MAX_DELAY);
////            for(auto it = UdpTimeMap.begin(); it != end; ++it)
////            {
////                auto packetItemIt = UdpPacketMap.find(it->second);
////                if(packetItemIt != UdpPacketMap.end())
////                {
////                    delete packetItemIt->second;
////                    UdpPacketMap.erase(packetItemIt);

////                }
////            }
////            UdpTimeMap.erase(UdpTimeMap.begin(), end);
////        }
//    while(m_UdpSocket.hasPendingDatagrams())
//    {
//        qDebug() << __FUNCTION__;
//        int size = m_UdpSocket.pendingDatagramSize();
//        uint64_t time;
//        uint32_t packetNum;
//        uint16_t totalSize, packetStart, packetSize;
//        UdpPacket *packet;
//        auto m = Message::CreateObject();
//        m->setRawSize(size);
//        m_UdpSocket.readDatagram(m->rawBuf(), size);
//        m->refresh();
//        auto packetIt = m_FriendUdpPacketMap.find(m->srcId());
//        assert(packetIt != m_FriendUdpPacketMap.end());
//        std::map<uint16_t, UdpPacket*> &UdpPacketMap = packetIt->second;
//        std::map<uint64_t, uint16_t> &UdpTimeMap = packetIt->first;
//        m->getUdpInfo(packetNum, totalSize, packetStart, time);
//        packetSize = size - sizeof(MsgHead);
//        qDebug() << count++ << " " << size << " " << totalSize << " " << " " << time;
//        if(time > Currenttime - UDP_MAX_DELAY)
//        {
//            auto it = UdpPacketMap.find(packetNum);
//            if(it != UdpPacketMap.end())
//                packet = it->second;
//            else
//            {
//                packet = new UdpPacket;
//                packet->size = 0;
//                packet->bitlist = 0;
//                packet->time = time;
//                packet->totalSize = totalSize;
//                packet->data = std::shared_ptr<char>(new char[totalSize]);
//                UdpPacketMap.insert(std::pair<uint16_t, UdpPacket*>(packetNum, packet));
//                UdpTimeMap.insert(std::pair<uint64_t, uint16_t>(time, packetNum));
//            }

//            memcpy(packet->data.get() + packetStart, m->data(), packetSize);
//            packet->size += packetSize;
//            qDebug() << count++ << " " << size << " " << totalSize << " " << packet->size << " " << time;
//            if(packet->size == totalSize && time > maxTimeOk)
//            {
//                maxTimeOk = time;
//                qDebug() << "isok" << " PackNum : " << packetNum << " time:" << time << "current time : " << Currenttime;
//            }

//        }
//    }


////        if(maxTimeOk > Currenttime - UDP_MAX_DELAY)
////        {
////            for(auto packetMapIt = m_FriendUdpPacketMap.begin(); packetMapIt != m_FriendUdpPacketMap.end(); ++packetMapIt)
////            {
////                std::list<std::pair<uint64_t, UdpPacket*>> dataList;
////                auto UdpTimeMap = packetMapIt->first;
////                auto UdpPacketMap = packetMapIt->second;
////                auto end = UdpTimeMap.upper_bound(maxTimeOk);
////                for(auto it = UdpTimeMap.begin(); it != end; ++it)
////                {
////                    auto packetItemIt = UdpPacketMap.find(it->second);
////                    if(packetItemIt != UdpPacketMap.end() && packetItemIt->second->totalSize == packetItemIt->second->size)
////                    {
////                        qDebug() << "totalsize : " << packetItemIt->second->totalSize << " time : " << packetItemIt->second->time;
////                        dataList.push_back(std::pair<uint64_t, UdpPacket*>(packetItemIt->second->time, packetItemIt->second));
////                        UdpPacketMap.erase(packetItemIt);
////                    }
////                }
////                UdpTimeMap.erase(UdpTimeMap.begin(), end);
////            }
////        }

//}

//void MainWindow::UdpSendToRemote(uint32_t FriendId, int PacketNum, const char *data, int size)
//{
//    const static int MAX_SIZE = 1472 - 36;
//    uint64_t time = QDateTime::currentDateTime().toMSecsSinceEpoch();
//    for(int start = 0; start < size; start += UDP_MAX_SIZE)
//    {
//        int PacketSize = (start + UDP_MAX_SIZE) <= size ? UDP_MAX_SIZE : (size - start);
//        auto m = CreateUdpChatMsg(FriendId, m_UserId, UDPTRANSFERMSG, 0, (data + start), size, start, PacketSize, PacketNum, time);
//        m_UdpSocket.writeDatagram((char *)m->tobuf(), m->size(), QHostAddress("129.204.4.80"), 9000);
//    }
//}
