#include "groupitem.h"
#include <QLabel>
#include <QBitmap>
#include <QPainter>
#include <QPoint>
#include <QAction>
#include <QDebug>

GroupItem::GroupItem(QWidget *parent, void *item) : QWidget(parent)
{
    this->setMinimumSize(240, HEIGHT + 10);
    this->setMaximumHeight(HEIGHT + 10);
    m_item = item;

    m_Profile = new QLabel(this);
    m_Name = new QLabel(this);
    m_Desc = new QLabel(this);
    m_Date = new QLabel(this);
    m_MsgCount = new QLabel(this);

    m_Profile->resize(HEIGHT, HEIGHT);
    m_MsgCount->resize(HEIGHT, HEIGHT / 2);

    m_Profile->setMask(createEllipseMask(m_Profile->size(), QPoint(HEIGHT / 2, HEIGHT / 2), HEIGHT / 2 - 5, HEIGHT / 2 - 5));
    m_MsgCount->setMask(createEllipseMask(m_MsgCount->size(), QPoint(HEIGHT / 2, HEIGHT / 4), 9, 9));

    m_MsgCount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_Date->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("Microsoft YaHei", 12, 50);
    m_Name->setFont(font);

    m_MsgCount->setObjectName("MsgCount");
    this->setStyleSheet("QLabel{background-color:transparent;}QLabel#MsgCount{background-color:rgb(247, 76, 49);}");

    m_changeName = new QAction(tr("&修改备注"), this);
    m_changeGroup = new QAction(tr("&修改分组"), this);
    m_showInfo = new QAction(tr("&查看资料"), this);
    m_deleteFriend = new QAction(tr("&删除好友"), this);

    addAction(m_changeName);
    addAction(m_changeGroup);
    addAction(m_showInfo);
    addAction(m_deleteFriend);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(m_changeName, SIGNAL(triggered(bool)), this, SLOT(changeName()));
    connect(m_changeGroup, SIGNAL(triggered(bool)), this, SLOT(changeGroup()));
    connect(m_showInfo, SIGNAL(triggered(bool)), this, SLOT(showInfo()));
    connect(m_deleteFriend, SIGNAL(triggered(bool)), this, SLOT(deleteFriend()));
}

GroupItem::~GroupItem()
{
    qDebug() << "~GroupItem";
    delete m_Profile;
    delete m_Name;
    delete m_Desc;
    delete m_Date;
    delete m_MsgCount;

    delete m_changeName;
    delete m_deleteFriend;
    delete m_changeGroup;
    delete m_showInfo;
}

void GroupItem::resizeEvent(QResizeEvent *event)
{
    m_Profile->setGeometry(0, 0, HEIGHT, HEIGHT);
    m_Name->setGeometry(HEIGHT, 0, this->width() - 2 * HEIGHT, HEIGHT / 2);
    m_Desc->setGeometry(HEIGHT, HEIGHT / 2, this->width() - 2 * HEIGHT, HEIGHT / 2);
    m_Date->setGeometry(this->width() - HEIGHT, 0, HEIGHT, HEIGHT / 2);
    m_MsgCount->setGeometry(this->width() - HEIGHT, HEIGHT / 2, HEIGHT, HEIGHT / 2);
    this->QWidget::resizeEvent(event);
}

//private

QBitmap GroupItem::createEllipseMask(const QSize &size, const QPoint &point, const int rx, const int ry)
{
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawEllipse(point, rx, ry);
    return mask;
}


//public function
void GroupItem::setProfile(const QString &profile)
{
    m_Profile->setPixmap(QPixmap(profile).scaled(HEIGHT, HEIGHT));
}

void GroupItem::setName(const QString &name)
{
    m_Name->setText(name);
}

void GroupItem::setDesc(const QString &desc)
{
    m_Desc->setText(desc);
}

void GroupItem::setDate(const QString &date)
{
    m_Date->setText(date);
    m_Date->show();
}

void GroupItem::setMessageCount(const int count)
{
    m_MsgCount->show();
}

void GroupItem::hideDateLabel()
{
    m_Date->hide();
}

void GroupItem::hideMessageCountLabel()
{
    m_MsgCount->hide();
}


//signal
void GroupItem::changeName()
{
    qDebug("Item changeName");
    emit changeNameSign(m_item);
}

void GroupItem::changeGroup()
{
    qDebug("Item changeGroup");
    emit changeGroupSign(m_item);
}

void GroupItem::showInfo()
{
    qDebug("Item showInfo");
    emit showInfoSign(m_item);
}

void GroupItem::deleteFriend()
{
    qDebug("Item deleteFriend");
    emit deleteFriendSign(m_item);
}

