#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "global.h"
#include "messagewidget.h"
#include "chatwindowmessageitem.h"
#include "mainwindow.h"
#include "filewidgetitem.h"

#include <QPainter>
#include <QPushButton>
#include <QtCore/qmath.h>
#include <QResizeEvent>
#include <QMessageBox>
#include <tchar.h>
#include <QRegExp>
#include <nlohmann/json.hpp>
#include <QFileDialog>
using json = nlohmann::json;


using namespace cv;

ChatWindow::ChatWindow(uint32_t Id, uint32_t FriendId, const QString &Name, QWidget *parent) :
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
    m_TitleBar->setTitleContent(Name, 15);


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

    m_FileTransferLayout = new QHBoxLayout(ui->MainBoard);
    m_FileTransferLayout->setAlignment(Qt::AlignHCenter);
    m_FileTransferInfo = new QListWidget(ui->MainBoard);
    m_FileTransferInfo->resize(200, 600);
    m_FileTransferInfo->setMaximumHeight(600);
    m_FileTransferInfo->setMinimumWidth(200);
    m_FileTransferLayout->addWidget(m_FileTransferInfo);
    ui->MainLayout->addLayout(m_FileTransferLayout);
    m_FileTransferInfo->hide();


    ui->MsgEdit->installEventFilter(this);
    ui->MsgEdit->setFont(QFont(tr("Consolas"), 14));

    ui->MsgList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->MsgList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->MsgList->setSelectionRectVisible(false);
    ui->MsgList->setStyleSheet("QListWidget#MsgList::item { background-color:rgba(255,255,255,0%);} QListWidget#MsgList{border-style:none;}");
    ui->MsgList->setSelectionMode(QAbstractItemView::NoSelection);


    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));
    connect(ui->CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->VideoButton, SIGNAL(clicked(bool)), this, SLOT(ReqVideoChat()));
    connect(&m_ShowVideoTimer, SIGNAL(timeout()), this, SLOT(ShowVideoInfo()));
    connect(ui->SendButton, SIGNAL(clicked(bool)), this, SLOT(SendChatWordMessage()));
    connect(ui->EmoticonButton, SIGNAL(clicked(bool)), this, SLOT(ShowEmotionWidget()));
//    connect(ui->WordButton, SIGNAL(clicked(bool)), this, SLOT(ChangeWordStyle()));
    connect(m_EmotionWidget, SIGNAL(cellClicked(int,int)), this, SLOT(AddEmotion(int, int)));
    connect(ui->FileButton, SIGNAL(clicked(bool)), this, SLOT(ReadySendFile()));
    connect(ui->TransferInfoButton, SIGNAL(clicked(bool)), this, SLOT(OnTransferInfoClick()));

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

    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERCHATDATAACTION), std::bind(&ChatWindow::RecvChatData, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERREQFILESENDACTION), std::bind(&ChatWindow::ResSendFile, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, RESTRANSFERFILEACTION), std::bind(&ChatWindow::SendFileOrClose, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERFILEENDACTION), std::bind(&ChatWindow::RecvFileEnd, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, TRANSFERFILECONTINUE), std::bind(&ChatWindow::SendFileCountinue, this, std::placeholders::_1));
    m_HandleMap.insert(MESSAGETYPE(TRANSFERDATAGROUP, REQFORCECLOSEFILEACTION), std::bind(&ChatWindow::RemoteForceCloseFile, this, std::placeholders::_1));

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
    else if(target == m_EmotionWidget)
    {
        if(event->type() == QEvent::WindowDeactivate)
        {
            m_EmotionWidget->hide();
        }
    }
    return QWidget::eventFilter(target,event);
}

void ChatWindow::moveEvent(QMoveEvent *event)
{
    m_EmotionWidget->move(ui->MainBoard->mapToGlobal(QPoint(ui->EmoticonButton->x(), ui->EmoticonButton->y() - m_EmotionWidget->height())));
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
//    this->resize(ui->MsgEdit->width(), this->height());
    this->resize(this->width() - m_VideoWindow->width(), this->height());
    delete m_AudioPlayer;
    emit ChatWindowUdpChatEnd(m_FriendId);
}

void ChatWindow::AddFileTransferItem(uint64_t FileNum, bool isUpload, int FileSize, const QString& FileName)
{
    FileWidgetItem *fitem = new FileWidgetItem(FileNum, isUpload);
    QListWidgetItem *item = new QListWidgetItem(m_FileTransferInfo);
    item->setSizeHint(QSize(0, fitem->height()));
    fitem->SetFileName(FileName);
    fitem->SetProBarMaxVal(FileSize);
    m_FileTransferInfo->addItem(item);
    m_FileTransferInfo->setItemWidget(item, fitem);
    m_FileTransferItemMap.insert(FileNum, item);
    connect(fitem, SIGNAL(DownloadFile(uint64_t)), this, SLOT(DownloadFile(uint64_t)));
    connect(fitem, SIGNAL(CancelFile(uint64_t)), this, SLOT(ForceCloseFile(uint64_t)));
    qDebug() << __FUNCTION__ << " FileSize: " << FileSize;
    fitem->show();
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

void ChatWindow::HandleSendOrRecvSignal(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    qDebug() << __FUNCTION__;
    qDebug() << "id : " << Id << " FileCode : " << (FileCode == FILEDATATRANSFER) << " File Size : " << Size;
    if(Id == m_FriendId && FileCode == FILEDATATRANSFER)
    {
        auto it = m_FileTransferItemMap.find(FileNum);
        if(it != m_FileTransferItemMap.end())
        {
            FileWidgetItem *item = dynamic_cast<FileWidgetItem *>(m_FileTransferInfo->itemWidget(*it));
            assert(item != nullptr);
            if(item)
            {
                item->AddVal(Size);
            }
        }
    }
}



//private slot
void ChatWindow::OnTransferInfoClick()
{
    static bool show = false;
    if(!show)
    {
        m_FileTransferInfo->show();
        this->resize(this->width() + m_FileTransferInfo->width(), this->height());
        show = true;
    }
    else
    {
        m_FileTransferInfo->hide();
        this->resize(this->width() - m_FileTransferInfo->width(), this->height());
        show = false;
    }
}

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
    if(slist.size() && slist[0] != "<br />")
    {
        uint64_t time = QDateTime::currentDateTime().toMSecsSinceEpoch();
        auto m = CreateChatWordMsg(m_FriendId, m_MeId, TRANSFERCHATDATAACTION, 0, slist[0].toStdString(), time);
        SendtoRemote(s, m);
        QListWidgetItem *item = new QListWidgetItem(ui->MsgList);
        ChatWindowMessageItem *mItem = new ChatWindowMessageItem(nullptr, true, time, slist[0], ui->MsgList->width());
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

void ChatWindow::ShowEmotionWidget()
{
    qDebug() << "in ShowEmotionWidget";

//    if(!m_EmotionWidgetShow)
//    {
        QPoint point = ui->MainBoard->mapToGlobal(QPoint(ui->EmoticonButton->x(), ui->EmoticonButton->y() - m_EmotionWidget->height()));
        qDebug() << point;
        m_EmotionWidget->move(point);
        m_EmotionWidget->show();
        m_EmotionWidget->raise();
        m_EmotionWidgetShow = true;
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

void ChatWindow::AddEmotion(int row, int col)
{
    QString filepath = QString("E:/University_Final_Text_Qt_Project/classic/%1.png").arg(row * 5 + col);
    QString html = QString("<img src=\"%1\" height=\"30\" width=\"30\">").arg(filepath);
    ui->MsgEdit->insertHtml(html);
    m_EmotionWidget->hide();
}

void ChatWindow::ReadySendFile()
{
    QString FullPath = QFileDialog::getOpenFileName(this, tr("选择文件"), "C:");
    if(!FullPath.isEmpty())
    {
        QFileInfo info(FullPath);
        m_ReadySendFile.insert(GlobalFileNum, info);
        auto m = CreateReadySendFileToFriend(m_MeId, m_FriendId, 0, info.size(), info.fileName().toStdString(), GlobalFileNum);
        SendtoRemote(s, m);
        qDebug() << "send readysendfile";
        AddFileTransferItem(GlobalFileNum, true, info.size(), info.fileName());
//        OnTransferInfoClick();
        GlobalFileNum++;
    }
}

void ChatWindow::DownloadFile(uint64_t FileNum)
{
    qDebug() << __FUNCTION__ << " FileNum" << FileNum;
    auto it = m_ReadyDownloadFile.find(FileNum);
    if(it != m_ReadyDownloadFile.end())
    {
        qDebug() << "ready download";
        m_MainWin->AddDownloadFile(FileNum, *it);
        m_ReadyDownloadFile.erase(it);
    }
}


void ChatWindow::ForceCloseFile(uint64_t FileNum)
{
    auto it = m_FileTransferItemMap.find(FileNum);
    if(it != m_FileTransferItemMap.end())
    {
        FileWidgetItem *item = dynamic_cast<FileWidgetItem *>(m_FileTransferInfo->itemWidget(*it));
        assert(item != nullptr);
        if(item)
        {
            bool isUpload = item->IsUpload();
            if(isUpload)
            {
                m_ReadySendFile.remove(FileNum);
                m_MainWin->CloseSendFileNum(FileNum, true);

            }
            else
            {
                m_ReadyDownloadFile.remove(FileNum);
                m_MainWin->CloseDownloadFileNum(FileNum, true);
            }
            auto m = CreateReqForceCloseFileMsg(m_MeId, m_FriendId, 0, FileNum);
            SendtoRemote(s, m);
        }
    }
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

void ChatWindow::RecvChatData(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t time = info["time"].get<json::number_unsigned_t>();
    QString text = QString::fromStdString(info["Msg"].get<std::string>());
    QListWidgetItem *item = new QListWidgetItem(ui->MsgList);
    ChatWindowMessageItem *mItem = new ChatWindowMessageItem(nullptr, false, time, text, ui->MsgList->width());
    mItem->show();
    mItem->UpdateWidth(ui->MsgList->width());
    item->setSizeHint(QSize(0, mItem->height()));
    ui->MsgList->addItem(item);
    ui->MsgList->setItemWidget(item, mItem);
    ui->MsgList->scrollToBottom();
}

void ChatWindow::ResSendFile(MessagePtr m)
{

    json info = json::parse((char *)m->data());
    int Size = info["FileSize"].get<json::number_unsigned_t>();
    std::string Name = info["Name"].get<std::string>();
    DowloadFileItem item(info["SenderFileNum"].get<json::number_unsigned_t>(), m_FriendId, Size, FILEDATATRANSFER, Name, QString("%1/%2/%3").arg(CACHEPATH).arg(m_MeId).arg("recvfile/").toStdString());
    m_ReadyDownloadFile.insert(item.FileNum, item);
    AddFileTransferItem(item.FileNum, false, item.Length, QString::fromStdString(Name));
    qDebug() << __FUNCTION__ << " FileNum :" << item.FileNum;

//    m_MainWin->AddDownloadFile(item.FileNum, item);
}

void ChatWindow::SendFileOrClose(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t LocalNum = info["FileNum"].get<json::number_unsigned_t>();
    auto it = m_ReadySendFile.find(LocalNum);
    if(it != m_ReadySendFile.end())
    {
        m_MainWin->AddSendFile(info["FileNum"].get<json::number_unsigned_t>(), m_FriendId, FILEDATATRANSFER, it->absoluteFilePath().toStdString());
        m_ReadySendFile.erase(it);
    }
}

void ChatWindow::SendFileCountinue(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    m_MainWin->SignalSendFile(FileNum);
}

void ChatWindow::RecvFileEnd(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    auto it = m_FileTransferItemMap.find(FileNum);
    if(it != m_FileTransferItemMap.end())
    {
        FileWidgetItem * item = dynamic_cast<FileWidgetItem *>(m_FileTransferInfo->itemWidget(*it));
        item->TransferOver(true);
    }
    m_MainWin->CloseDownloadFileNum(FileNum, false);
}

void ChatWindow::HandleFileSendedEnd(uint64_t FileNum)
{
    auto it = m_FileTransferItemMap.find(FileNum);
    if(it != m_FileTransferItemMap.end())
    {
        FileWidgetItem * item = dynamic_cast<FileWidgetItem *>(m_FileTransferInfo->itemWidget(*it));
        item->TransferOver(true);
    }
}

void ChatWindow::RemoteForceCloseFile(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    qDebug() << __FUNCTION__;
    auto it = m_FileTransferItemMap.find(FileNum);
    if(it != m_FileTransferItemMap.end())
    {
        qDebug() << "ready delete";
        FileWidgetItem *item = dynamic_cast<FileWidgetItem *>(m_FileTransferInfo->itemWidget(*it));
        assert(item != nullptr);
        if(item)
        {
            item->TransferOver(false);
            bool isUpload = item->IsUpload();
            if(isUpload)
            {
                m_ReadySendFile.remove(FileNum);
                m_MainWin->CloseSendFileNum(FileNum, true);

            }
            else
            {
                m_ReadyDownloadFile.remove(FileNum);
                m_MainWin->CloseDownloadFileNum(FileNum, true);
            }
        }
    }
}

void ChatWindow::HandleCacheMessage(const std::list<MessagePtr> &mlist)
{
    for(auto it = mlist.begin(); it != mlist.end(); ++it)
    {
        HandleMessage(m_FriendId, *it);
    }
}




