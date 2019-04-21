#include "mainwindow.h"
#include "groupitem.h"
#include "personalinfo.h"
#include "message.h"
#include "global.h"
#include "chatwindow.h"
#include <nlohmann/json.hpp>
#include "groupiteminfo.h"
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

using nlohmann::json;


MainWindow::MainWindow(uint32_t UserId, QWidget *parent) :
    QWidget(parent)
  , m_bIsAutoHide(false)
  , m_enDriection(NONE)
  , m_UserId(UserId)
{
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    //init
    m_MainBoard = new QWidget(this);
    m_TitleBar = new TitleBar(this);
    m_GroupTree = new QTreeWidget(m_MainBoard);
    m_Profile = new QLabel(m_MainBoard);
    m_UserName = new QLabel(m_MainBoard);
    m_UserDesc = new QLabel(m_MainBoard);
    m_SerachLineEdit = new QLineEdit(m_MainBoard);
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
    m_GroupTree->addAction(m_addGroup);
    m_GroupTree->setContextMenuPolicy(Qt::ActionsContextMenu);



    m_SerachLineEdit->resize(m_MainBoard->width(), 30);
    m_SerachLineEdit->setStyleSheet("QLineEdit::focus{ background-color: white; border-bottom:1px solid rgb(200,200,200);}QLineEdit{  border-style:none;background-color: rgba(255, 255, 255, 25%); }");
    m_SerachLineEdit->setPlaceholderText("搜索 : 号码 / 名称 / 备注");
    m_SerachLineEdit->installEventFilter(this);

    m_Profile->resize(PROFILESIZE, PROFILESIZE);
    m_Profile->setObjectName("profile");
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



    UpdatePos();

    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), qApp, SLOT(quit()));
    connect(m_GroupTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemClick(QTreeWidgetItem*,int)));
    connect(m_GroupTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemDoubleClick(QTreeWidgetItem*,int)));
    connect(m_addGroup, SIGNAL(triggered(bool)), this, SLOT(AddUserGroup()));

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
    InitHandle();
    GetRemoteInfo();
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

//private

void MainWindow::InitHandle()
{
    m_HandleMap.insert(MESSAGETYPE(RESINFOGROUP, RESUSERINFOCODE), std::bind(&MainWindow::ShowUserInfo, this, std::placeholders::_1));


    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESCHANGEFRIENDOTHERNAMECODE), std::bind(&MainWindow::ChangeUserName, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESADDFRIENDSGROUPCODE), std::bind(&MainWindow::AddFriendsGroup, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESDELFRIENDSCODE), std::bind(&MainWindow::DeleteFriends, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESCHANGEGROUP, RESCHANGEFRIENDSGROUPCODE), std::bind(&MainWindow::ChangeFriendsGroup, this, std::placeholders::_1));
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
    GroupItem *ptable = new GroupItem(nullptr, pitem);
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


void MainWindow::AddUserGroup(QString &Name, uint32_t GroupID)
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


void MainWindow::GetRemoteInfo()
{
    if(InitMySelf() && InitFriendsGroup() && InitFriends())
    {
        qDebug() << "connect";
        connect(this, SIGNAL(HasMessage(uint32_t,std::shared_ptr<Message>)), this, SLOT(HandleMessage(uint32_t,std::shared_ptr<Message>)));
        connect(&s, SIGNAL(readyRead()), this, SLOT(MessageRead()));

        return;
    }
error:
    //网络异常则会运行到这里
    qDebug() << "error\n";
    this->close();
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
            AddGroupItem(GroupId, Id, Name, OtherName, Desc, Profile, Sex);

        }
        return true;
    }
    return false;
}


//slot
void MainWindow::HandleMessage(uint32_t id, std::shared_ptr<Message> m)
{
    qDebug() << "HandleMessage " << id;
    if(SERVERID == id || false)
    {
        auto it = m_HandleMap.find(m->Type());
        if(it != m_HandleMap.end())
        {
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

        ChatWindow *w = new ChatWindow();
        w->show();
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
