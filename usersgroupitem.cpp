#include "usersgroupitem.h"
#include <QPainter>
#include <QDebug>

UsersGroupItem::UsersGroupItem(uint32_t GroupId, bool isAdmin, QWidget *parent) :
    QWidget(parent),
    m_isAdmin(isAdmin),
    m_GroupId(GroupId)
{
    this->setMinimumSize(240, HEIGHT + 10);
    this->setMaximumHeight(HEIGHT + 10);

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


    if(m_isAdmin)
    {
        qDebug() << "is Admin";
        m_deleteGroup = new QAction(tr("&解散群组"), this);
        m_changeGroupInfo = new QAction(tr("&修改群组信息"), this);
        m_changeGroupMember = new QAction(tr("&剔除成员"), this);
        addAction(m_changeGroupInfo);
        addAction(m_changeGroupMember);
        addAction(m_deleteGroup);
    }
    else
    {
        qDebug() << "is not Admin";
        m_deleteGroup = new QAction(tr("&离开群组"), this);
        m_changeGroupInfo = new QAction(tr("&查看群组信息"), this);
        addAction(m_changeGroupInfo);
        addAction(m_deleteGroup);
    }


    setContextMenuPolicy(Qt::ActionsContextMenu);
}

//public function
void UsersGroupItem::setProfile(const QString &profile)
{
    m_Profile->setPixmap(QPixmap(profile).scaled(HEIGHT, HEIGHT));
}

void UsersGroupItem::setName(const QString &name)
{
    m_Name->setText(name);
}

void UsersGroupItem::setDesc(const QString &desc)
{
    m_Desc->setText(desc);
}

void UsersGroupItem::setDate(const QString &date)
{
    m_Date->setText(date);
    m_Date->show();
}

void UsersGroupItem::setMessageCount(const int count)
{
    m_MsgCount->show();
}

void UsersGroupItem::hideDateLabel()
{
    m_Date->hide();
}

void UsersGroupItem::hideMessageCountLabel()
{
    m_MsgCount->hide();
}

//private

QBitmap UsersGroupItem::createEllipseMask(const QSize &size, const QPoint &point, const int rx, const int ry)
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

//虚函数
void UsersGroupItem::resizeEvent(QResizeEvent *event)
{
    m_Profile->setGeometry(0, 0, HEIGHT, HEIGHT);
    m_Name->setGeometry(HEIGHT, 0, this->width() - 2 * HEIGHT, HEIGHT / 2);
    m_Desc->setGeometry(HEIGHT, HEIGHT / 2, this->width() - 2 * HEIGHT, HEIGHT / 2);
    m_Date->setGeometry(this->width() - HEIGHT, 0, HEIGHT, HEIGHT / 2);
    m_MsgCount->setGeometry(this->width() - HEIGHT, HEIGHT / 2, HEIGHT, HEIGHT / 2);
    this->QWidget::resizeEvent(event);
}

//public slots
void UsersGroupItem::ChangeProfile(uint32_t id, const QString &Profile)
{
    if(id == m_GroupId)
    {
        m_Profile->setPixmap(QPixmap::fromImage(QImage(Profile)).scaled(m_Profile->size()));
    }
}
