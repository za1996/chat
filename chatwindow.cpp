#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "global.h"
#include "messagewidget.h"

#include <QPainter>
#include <QPushButton>
#include <QtCore/qmath.h>
#include <QResizeEvent>
#include <QMessageBox>
#include <tchar.h>
#include <QRegExp>


using namespace cv;

ChatWindow::ChatWindow(uint32_t Id, uint32_t FriendId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow),
    m_MeId(Id),
    m_FriendId(FriendId),
    m_DelHandle(nullptr),
    m_VideoWindow(nullptr),
    m_VideoWindowLayout(nullptr),
    m_AudioPlayer(nullptr),
    m_IsUdpChatNow(false),
    m_EmotionWidgetShow(false)
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
    m_TitleBar->setTitleContent("我的好友", 15);


    m_EmotionWidget = new QWidget();
    m_EmotionWidget->resize(200, 200);
    m_EmotionWidget->hide();
    m_EmotionWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_EmotionWidget->installEventFilter(this);


    ui->MsgEdit->installEventFilter(this);
    ui->MsgEdit->setFont(QFont(tr("Consolas"), 14));


    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->VideoButton, SIGNAL(clicked(bool)), this, SLOT(ReqVideoChat()));
    connect(&m_ShowVideoTimer, SIGNAL(timeout()), this, SLOT(ShowVideoInfo()));
    connect(ui->SendButton, SIGNAL(clicked(bool)), this, SLOT(SendChatWordMessage()));
    connect(ui->EmoticonButton, SIGNAL(clicked(bool)), this, SLOT(ShowEmotionWidget()));
    connect(ui->WordButton, SIGNAL(clicked(bool)), this, SLOT(ChangeWordStyle()));

    Inithandle();
}

ChatWindow::~ChatWindow()
{
    delete ui;
    if(m_DelHandle)m_DelHandle(m_FriendId);
    delete m_EmotionWidget;
}

//init
void ChatWindow::Inithandle()
{
    m_HandleMap.insert(MESSAGETYPE(REQUDPEVENTGROUP, REQUDPCHATACTION), std::bind(&ChatWindow::ReqUdpChatMsgHandle, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(RESUDPREQGROUP, RESREADYUDPCHATSENDCODE), std::bind(&ChatWindow::ReqUdpChatMsgHandle, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(REQUDPEVENTGROUP, RESCLOSEUDPCHATCODE), std::bind(&ChatWindow::ReqUdpChatMsgHandle, this, std::placeholders::_1));
}

// 虚函数
void ChatWindow::paintEvent(QPaintEvent *event)
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

void ChatWindow::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
}

bool ChatWindow::eventFilter(QObject *target, QEvent *event)
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
    return QWidget::eventFilter(target,event);
}

void ChatWindow::moveEvent(QMoveEvent *event)
{
    m_EmotionWidget->move(ui->MainBoard->mapToGlobal(QPoint(ui->WordButton->x(), ui->WordButton->y() - m_EmotionWidget->height())));
    QWidget::moveEvent(event);
}

//public fun
void ChatWindow::SetDelHandle(std::function<void (uint32_t)> f)
{
    m_DelHandle = f;
}

//private fun
void ChatWindow::ReqVideoChat()
{
    if(!m_IsUdpChatNow)
    {
        m_IsUdpChatNow = true;
        if(!m_VideoWindowLayout && !m_VideoWindow)
        {
            m_VideoWindowLayout = new QHBoxLayout(ui->MainBoard);
            m_VideoWindowLayout->setAlignment(Qt::AlignHCenter);
            m_VideoWindow = new VideoWindow(ui->MainBoard);
            m_VideoWindowLayout->addWidget(m_VideoWindow);
            connect(m_VideoWindow, SIGNAL(VideoWindowCloseButtonClick()), this, SLOT(CloseVideoWindow()));
        }
        this->resize(this->width() + m_VideoWindow->width(), this->height());
        ui->MainLayout->addLayout(m_VideoWindowLayout);
        auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQUDPCHATACTION, 0);
        SendtoRemote(s, m);
    }

}

QImage ChatWindow::MatToQImage(cv::Mat &mtx)
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

void ChatWindow::RemoveVideoChat()
{
    m_IsUdpChatNow = false;
    ui->MainLayout->removeItem(m_VideoWindowLayout);
    m_ShowVideoTimer.stop();
    this->resize(ui->MsgEdit->width(), this->height());
    delete m_AudioPlayer;
    emit ChatWindowUdpChatEnd(m_FriendId);
}


//public slot
void ChatWindow::HandleMessage(uint32_t id, std::shared_ptr<Message> m)
{
    qDebug() << __FUNCTION__ << " messageid : " << id << "chat id : " << m_FriendId;
    if(id == m_FriendId)
    {
        qDebug() << __FUNCTION__ << "ready fun start";
        auto it = m_HandleMap.find(m->Type());
        if(it != m_HandleMap.end())
        {
            qDebug() << __FUNCTION__ << "fun start";
            (*it)(m);
            qDebug() << __FUNCTION__ << "fun end";
        }
        qDebug() << __FUNCTION__ << "ready fun end";
    }
}



//private slot
void ChatWindow::CloseVideoWindow()
{
    RemoveVideoChat();
    auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQCLOSEUDPCHATACTION, 0);
    SendtoRemote(s, m);
}

void ChatWindow::SendChatWordMessage()
{
    QRegExp rx("<p.*>(.*)</p>");
    int pos(0);
    QStringList slist;
    while((pos = rx.indexIn(ui->MsgEdit->toHtml(), pos)) != -1) {
        slist.push_back(rx.capturedTexts().at(1));
        pos += rx.matchedLength();
    }
    qDebug() << slist;
    mw = new MessageWidget;
    mw->SetText(slist[0]);
    ui->MsgEdit->clear();
}

void ChatWindow::ShowEmotionWidget()
{
    qDebug() << "in ShowEmotionWidget";
    qDebug() << "mw width " << mw->width();
    mw->SetWidth(mw->width() - 10);
//    if(!m_EmotionWidgetShow)
//    {
//        QPoint point = ui->MainBoard->mapToGlobal(QPoint(ui->WordButton->x(), ui->WordButton->y() - m_EmotionWidget->height()));
//        qDebug() << point;
//        m_EmotionWidget->move(point);
//        m_EmotionWidget->show();
//        m_EmotionWidget->raise();
//        m_EmotionWidgetShow = true;
//    }
//    else
//    {
//        m_EmotionWidget->hide();
//        m_EmotionWidgetShow = false;
//    }
    qDebug() << "end ShowEmotionWidget";
}

void ChatWindow::ChangeWordStyle()
{
    qDebug() << "mw width " << mw->width();
    mw->SetWidth(mw->width() + 10);
}



//消息处理函数
void ChatWindow::ReqUdpChatMsgHandle(MessagePtr m)
{
    if(m->Type() == MESSAGETYPE(REQUDPEVENTGROUP, REQUDPCHATACTION) && !m_IsUdpChatNow)
    {
        ReqVideoChat();
        auto m = CreateReqUdpChatMsg(m_FriendId, m_MeId, REQUDPCHATSUCCESSACTION, 0);
        SendtoRemote(s, m);

    }
    else if(m->Type() == MESSAGETYPE(REQUDPEVENTGROUP, RESCLOSEUDPCHATCODE))
    {
        RemoveVideoChat();
    }
    else if(m->Type() == MESSAGETYPE(RESUDPREQGROUP, RESREADYUDPCHATSENDCODE) && m_IsUdpChatNow)
    {
        m_ShowVideoTimer.start(100);
        m_AudioPlayer = new AudioPlayer;
        m_AudioPlayer->addUser(m_FriendId);

    }
}

void ChatWindow::ShowVideoInfo()
{
//    qDebug() << __FUNCTION__;
    if(isUdpChatNow())
    {
        auto it = m_FriendVideoDataQueueMap.find(m_FriendId);
        UdpPacketPtr Packet;
        if(it != m_FriendVideoDataQueueMap.end())
        {
            if(it->second.try_dequeue(Packet))
            {
                    Mat frame;
                    frame = imdecode(Mat(Packet->data), IMREAD_COLOR);
                    m_VideoWindow->SetVideoPic(QPixmap::fromImage(MatToQImage(frame)));

            }
        }
        it = m_FriendAudioDataQueueMap.find(m_FriendId);
        if(it != m_FriendAudioDataQueueMap.end())
        {
            if(it->second.try_dequeue(Packet))
            {
                m_AudioPlayer->setCurrentUser(m_FriendId);
                m_AudioPlayer->play(Packet->data.data(), Packet->totalSize, AUDIO_FREQ, AL_FORMAT_MONO8);
            }
        }
    }
}
