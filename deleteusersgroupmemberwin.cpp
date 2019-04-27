#include "deleteusersgroupmemberwin.h"
#include "ui_deleteusersgroupmemberwin.h"
#include "global.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QScrollBar>
#include <QLabel>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

DeleteUsersGroupMemberWin::DeleteUsersGroupMemberWin(QWidget *parent, const std::map<uint32_t, QString> &map, uint32_t GroupId, uint32_t MeId) :
    QWidget(parent),
    ui(new Ui::DeleteUsersGroupMemberWin),
    m_GroupId(GroupId),
    m_MeId(MeId)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#MainBoard{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);

    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->MainBoard->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");
    ui->MemberListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->MemberListWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    ui->MemberListWidget->verticalScrollBar()->setSingleStep(10);
    ui->MemberListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for(auto it = map.begin(); it != map.end(); ++it)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->MemberListWidget);
        QLabel *member = new QLabel();
        member->setText(it->second);
        member->adjustSize();
        item->setData(Qt::UserRole, it->first);
        item->setSizeHint(QSize(0, member->height()));
        ui->MemberListWidget->addItem(item);
        ui->MemberListWidget->setItemWidget(item, member);
    }

    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->DelButton, SIGNAL(clicked(bool)), this, SLOT(DeleteMember()));
}

DeleteUsersGroupMemberWin::~DeleteUsersGroupMemberWin()
{
    qDebug() << "~DeleteUsersGroupMemberWin";
    delete ui;
}

void DeleteUsersGroupMemberWin::paintEvent(QPaintEvent *event)
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

void DeleteUsersGroupMemberWin::HandMessage(uint32_t id, std::shared_ptr<Message> m)
{
    qDebug() << __FUNCTION__;
    if(id == SERVERID)
    {
        qDebug() << __FUNCTION__ << " in server";
        if(m->Type() == MESSAGETYPE(RESCHANGEGROUP, RESDELMEMBERSUCCEESCODE))
        {
            qDebug() << __FUNCTION__ << " right type";
            json info = json::parse((char *)m->data());
            if(info["GroupId"].get<json::number_unsigned_t>() == m_GroupId)
            {
                qDebug() << __FUNCTION__ << " close";
                this->close();
            }
        }
    }
}

void DeleteUsersGroupMemberWin::DeleteMember()
{
    auto list = ui->MemberListWidget->selectedItems();
    qDebug() << __FUNCTION__ << " " << list;
    if(list.size())
    {
        std::vector<uint32_t> Memberlist;
        for(auto it = list.begin(); it != list.end(); ++it)
        {
            Memberlist.push_back((*it)->data(Qt::UserRole).value<uint32_t>());
        }
        qDebug() << Memberlist;
        auto m = CreateDelUsersGroupMembers(m_MeId, 0, m_GroupId, Memberlist);
        SendtoRemote(s, m);
        ui->MemberListWidget->setSelectionMode(QAbstractItemView::NoSelection);
        ui->TisLabel->setText("正在删除，成功会自动关闭");
    }
}
