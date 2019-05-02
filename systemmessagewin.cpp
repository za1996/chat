#include "systemmessagewin.h"
#include "ui_systemmessagewin.h"
#include "systemmessageitem.h"
#include "global.h"

SystemMessageWin::SystemMessageWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemMessageWin)
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
    HandleMessage(50);
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
}

SystemMessageWin::~SystemMessageWin()
{
    delete ui;
}

void SystemMessageWin::HandleMessage(int n)
{
    static int start = 0;
    int temp = m_SysMsgCache.size() < n ? 0 : m_SysMsgCache.size() - n;
    if(start < temp)start = temp;

    for(; start < m_SysMsgCache.size(); ++start)
    {
        QString Text;
        json info = json::parse((char *)m_SysMsgCache[start].m->data());
        if(m_SysMsgCache[start].m->Type() == MESSAGETYPE(TRANSFERDATAGROUP, ADDREMOTEFRIENDACTION))
        {
            Text = QString("ID:%1,Name:%2 请求添加好友.").arg(info["UserId"].get<json::number_unsigned_t>()).arg(QString::fromStdString(info["UserName"].get<std::string>()));
        }
        else if(m_SysMsgCache[start].m->Type() == MESSAGETYPE(TRANSFERDATAGROUP, REQJOININGROUPACTION))
        {
            Text = QString("ID:%1,Name:%2 请求进群%3.").arg(info["UserId"].get<json::number_unsigned_t>()).arg(QString::fromStdString(info["UserName"].get<std::string>())).arg(info["GroupId"].get<json::number_unsigned_t>());
        }
        else
        {
            Text = "其他信息";
        }
        QListWidgetItem *item = new QListWidgetItem(ui->MsgListWidget);
        SystemMessageItem *mitem = new SystemMessageItem(m_SysMsgCache[start].OnlyShow, start);
        mitem->SetMsgText(Text);
        item->setSizeHint(QSize(0, mitem->height()));
        if(!m_SysMsgCache[start].OnlyShow && m_SysMsgCache[start].Finish)
        {
            mitem->SetButtonHide(m_SysMsgCache[start].isOK);
        }
        ui->MsgListWidget->addItem(item);
        ui->MsgListWidget->setItemWidget(item, mitem);
    }
}
