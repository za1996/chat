#include "groupchatwin.h"
#include "ui_groupchatwin.h"
#include "global.h"
#include "chatwindowmessageitem.h"
#include <QKeyEvent>
#include <QStringList>

GroupChatWin::GroupChatWin(uint32_t GroupId, const QString UserName, QWidget *parent) :
    QWidget(parent),
    m_UserName(UserName),
    ui(new Ui::GroupChatWin)
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
    m_TitleBar->setButtonType(MIN_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");

    m_EmotionWidget = new QTableWidget(5, 5);
    m_EmotionWidget->verticalHeader()->setVisible(false);
    m_EmotionWidget->horizontalHeader()->setVisible(false);
    m_EmotionWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_EmotionWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_EmotionWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_EmotionWidget->resize(200, 200);
    m_EmotionWidget->hide();
    m_EmotionWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_EmotionWidget->installEventFilter(this);
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            QString filepath = QString("E:/University_Final_Text_Qt_Project/classic/%1.png").arg(i * 5 + j);
            QLabel *item = new QLabel(m_EmotionWidget);
            item->resize(40, 40);
            item->setPixmap(QPixmap(filepath).scaled(40, 40));
            m_EmotionWidget->setCellWidget(i, j, item);
        }
    }

    ui->MsgEdit->installEventFilter(this);
    ui->MsgEdit->setFont(QFont(tr("Consolas"), 14));

    ui->MsgList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->MsgList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->MsgList->setSelectionRectVisible(false);
    ui->MsgList->setStyleSheet("QListWidget#MsgList::item { background-color:rgba(255,255,255,0%);} QListWidget#MsgList{border-style:none;}");
    ui->MsgList->setSelectionMode(QAbstractItemView::NoSelection);
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->EmotionButton, SIGNAL(clicked(bool)), this, SLOT(ShowEmotionWidget()));
    connect(ui->SendButton, SIGNAL(clicked(bool)), this, SLOT(SendChatWordMessage()));
    connect(m_EmotionWidget, SIGNAL(cellClicked(int,int)), this, SLOT(AddEmotion(int, int)));

}

GroupChatWin::~GroupChatWin()
{
    delete ui;
}

void GroupChatWin::ShowEmotionWidget()
{
    qDebug() << "in ShowEmotionWidget";

//    if(!m_EmotionWidgetShow)
//    {
        QPoint point = ui->MainBoard->mapToGlobal(QPoint(ui->EmotionButton->x(), ui->EmotionButton->y() - m_EmotionWidget->height()));
        qDebug() << point;
        m_EmotionWidget->move(point);
        m_EmotionWidget->show();
        m_EmotionWidget->raise();
//        m_EmotionWidgetShow = true;
//    }
//    else
//    {
//        m_EmotionWidget->hide();
//        m_EmotionWidgetShow = false;
//    }
    qDebug() << "end ShowEmotionWidget";
}

bool GroupChatWin::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->MsgEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 SendChatWordMessage();
                 return true;
             }
        }
    }
    else if(target == m_EmotionWidget)
    {
        if(event->type() == QEvent::WindowDeactivate)
        {
            m_EmotionWidget->hide();
        }
    }
    return QWidget::eventFilter(target,event);
}

void GroupChatWin::SendChatWordMessage()
{
    QRegExp rx("<p.*>(.*)</p>");
    int pos(0);
    QStringList slist;
    while((pos = rx.indexIn(ui->MsgEdit->toHtml(), pos)) != -1) {
        slist.push_back(rx.capturedTexts().at(1));
        pos += rx.matchedLength();
    }
    qDebug() << slist;
    if(slist.size() && slist[0] != "<br />")
    {
        uint64_t time = QDateTime::currentDateTime().toMSecsSinceEpoch();
//        auto m = CreateChatWordMsg(m_FriendId, m_MeId, TRANSFERCHATDATAACTION, 0, slist[0].toStdString(), time);
//        SendtoRemote(s, m);
        QListWidgetItem *item = new QListWidgetItem(ui->MsgList);
        qDebug() << "msglist width : " << ui->MsgList->width();
        ChatWindowMessageItem *mItem = new ChatWindowMessageItem(nullptr, true, time, slist[0], ui->MsgList->width(), m_UserName);
        qDebug() << "msg : " << slist[0];
        mItem->show();
        qDebug() << "mitem height " << mItem->height();
        mItem->UpdateWidth(ui->MsgList->width());
        item->setSizeHint(QSize(0, mItem->height()));
        ui->MsgList->addItem(item);
        ui->MsgList->setItemWidget(item, mItem);
        ui->MsgEdit->clear();
        ui->MsgList->scrollToBottom();
    }
}

void GroupChatWin::SetTitleName(const QString &Name)
{
    m_TitleBar->setTitleContent(Name, 15);
}

void GroupChatWin::AddEmotion(int row, int col)
{
    QString filepath = QString("E:/University_Final_Text_Qt_Project/classic/%1.png").arg(row * 5 + col);
    QString html = QString("<img src=\"%1\" height=\"30\" width=\"30\">").arg(filepath);
    ui->MsgEdit->insertHtml(html);
    m_EmotionWidget->hide();
}
