#include "mainwindow.h"
#include "groupitem.h"

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
#include <assert.h>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setObjectName("mainwindow");

    //init
    m_MainBoard = new QWidget(this);
    m_TitleBar = new TitleBar((this->parentWidget() ? this->parentWidget() : this));
    m_GroupTree = new QTreeWidget(this);
    m_Profile = new QLabel(this);
    m_UserName = new QLabel(this);
    m_UserDesc = new QLabel(this);
    m_SerachLineEdit = new QLineEdit(this);
    m_GroupMap = new QHash<QString, QTreeWidgetItem *>();

    this->resize(320, 680);
    this->setMaximumSize(320, 682);
    this->setMinimumSize(320, 680);
    this->setStyleSheet("QWidget#mainwindow{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");

    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(this->width());
    m_TitleBar->setButtonType(MIN_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");

    m_GroupTree->setHeaderHidden(true);
//    m_GroupTree->setExpandsOnDoubleClick(false);// 关闭双击自动展开.
    m_GroupTree->setRootIsDecorated(false);
    m_GroupTree->setIndentation(0);
    m_GroupTree->setContentsMargins(0, 0, 0, 0);
    m_GroupTree->resize(this->width(), this->height() * 0.65);
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



    m_SerachLineEdit->resize(this->width(), 30);
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

    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(m_GroupTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemClick(QTreeWidgetItem*,int)));
    connect(m_GroupTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onGroupItemDoubleClick(QTreeWidgetItem*,int)));
    connect(m_addGroup, SIGNAL(triggered(bool)), this, SLOT(AddGroup()));

    qDebug() << "TitleBar Height" << m_TitleBar->height() << endl;

    QTreeWidgetItem *pRootFriendItem = new QTreeWidgetItem();
    QLabel *pItemName = new QLabel(m_GroupTree);
    pItemName->setText("我的好友");
    pRootFriendItem->setSizeHint(0, QSize(0, 13));
    m_GroupTree->addTopLevelItem(pRootFriendItem);
    m_GroupMap->insert(pItemName->text(), pRootFriendItem);



    for(int i = 0; i < 10; i++)
    {
        AddGroupItem(pRootFriendItem);
    }
}

MainWindow::~MainWindow()
{
    delete m_GroupTree;
    delete m_TitleBar;
    delete m_Profile;
    delete m_UserName;
    delete m_UserDesc;
    delete m_SerachLineEdit;
    delete m_addGroup;
    delete m_GroupMap;
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

    this->QWidget::paintEvent(event);
}

//private
void MainWindow::UpdatePos()
{
    m_TitleBar->move(this->pos().x(), this->pos().y());
    m_Profile->move(SPACESIZE, m_TitleBar->height() + SPACESIZE);
    m_UserName->adjustSize();
    m_UserName->move(m_Profile->pos().x() + m_Profile->width() + SPACESIZE, m_Profile->pos().y());
    m_UserDesc->adjustSize();
    m_UserDesc->move(m_UserName->pos().x(), m_UserName->pos().y() + m_UserName->height() + SPACESIZE);
    m_SerachLineEdit->move(0, m_Profile->pos().y() + m_Profile->height() + SPACESIZE);
    m_GroupTree->move(0, m_SerachLineEdit->pos().y() + m_SerachLineEdit->height());
}

void MainWindow::AddGroupItem(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *pitem = new QTreeWidgetItem();
    GroupItem *ptable = new GroupItem(nullptr, pitem);
    parent->addChild(pitem);
    ptable->setProfile("D:/imggggg.bmp");
    ptable->setName("name");
    ptable->setDesc("desc");
    ptable->setDate("date");
    pitem->setSizeHint(0, QSize(0, ptable->height()));
    qDebug() << "GroupItem height : " << ptable->height() << endl;
    m_GroupTree->setItemWidget(pitem, 0, ptable);
    connect(ptable, SIGNAL(deleteFriendSign(void*)), this, SLOT(DelGroupItem(void*)));
    connect(ptable, SIGNAL(changeGroupSign(void*)), this, SLOT(ChangeGroupItem(void*)));
    connect(ptable, SIGNAL(changeNameSign(void*)), this, SLOT(ChangeGroupItemName(void*)));
}

//slot
void MainWindow::AddGroup()
{
    bool isOK;
    QString text = QInputDialog::getText(this, "Input Dialog",
                                                           "Please input your comment",
                                                           QLineEdit::Normal,
                                                           "your comment",
                                                           &isOK);
    if(isOK && !m_GroupMap->count(text))
    {
        QTreeWidgetItem *pRootFriendItem = new QTreeWidgetItem();
        QLabel *pItemName = new QLabel(m_GroupTree);
        pItemName->setText("我的好友");
        pRootFriendItem->setSizeHint(0, QSize(0, 26));
        m_GroupTree->addTopLevelItem(pRootFriendItem);
        m_GroupTree->setItemWidget(pRootFriendItem, 0, pItemName);
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

    }
}

void MainWindow::DelGroupItem(void *item)
{
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    assert(p && p->parent());

    GroupItem *gitem = dynamic_cast<GroupItem *>(p->treeWidget()->itemWidget(p, 0));
    p->parent()->removeChild(p);
    disconnect(gitem, SIGNAL(deleteFriendSign(void*)), this, SLOT(DelGroupItem(void*)));
    disconnect(gitem, SIGNAL(changeGroupSign(void*)), this, SLOT(ChangeGroupItem(void*)));
    disconnect(gitem, SIGNAL(changeNameSign(void*)), this, SLOT(ChangeGroupItemName(void*)));
    gitem->height();
//    delete gitem; //这里有问题 delete后会出错

}

void MainWindow::ChangeGroupItem(void *item)
{
    bool isOK;
    QTreeWidgetItem *p = static_cast<QTreeWidgetItem *>(item);
    assert(p && p->parent());

    QStringList stritems(m_GroupMap->keys());

    QInputDialog::getItem(this, "SelectGroup", "Group:", stritems, 0, false, &isOK);

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
}
