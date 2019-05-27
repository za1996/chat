#include "tcpfilethread.h"
#include "message.h"
#include "mainwindow.h"

#include <QMetaObject>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

TcpFileThread::TcpFileThread() :
    m_UserId(0),
    m_Socket(nullptr)
{
    moveToThread(this);
    m_mutex = new QMutex();
}

void TcpFileThread::run()
{
    m_Socket = new QTcpSocket();
    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(timeslot()), Qt::QueuedConnection);
    qDebug() << "run start";
    m_FileEmpty.notify_one();

    qDebug() << "run end";
    m_Socket->connectToHost("129.204.4.80", 8888);
    if(!m_Socket->waitForConnected())
    {
        qDebug() << "connect error";
    }
    m_Socket->setSocketOption(QAbstractSocket::LowDelayOption, true);
    for(;m_Socket->state() != QAbstractSocket::ConnectedState;sleep(1))
    {
        qDebug() << m_Socket->state();
    }

    if(m_UserId)
    {
        qDebug() << "ready send merge";
        auto m = CreateSocketMergeMsg(0, m_UserId, 1);
        if(!SendtoRemote(m))
        {
            qDebug() << "send error";
        }
    }
    else
    {
        return;
    }
//    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(read()));

//    sleep(10);
//    auto m = CreateTestMessage(m_UserId, 0, "Test");
//    if(!SendtoRemote(m))
//    {
//        qDebug() << "send error";
//    }
//    send();
    qDebug() << __FUNCTION__ << "thread id : " << QThread::currentThreadId();
    connect(this, SIGNAL(ReadySend(uint64_t,uint32_t)), this, SLOT(RealSend(uint64_t,uint32_t)));
    timer->start(1000);
    this->exec();
}

void TcpFileThread::send(uint64_t FileNum)
{
    qDebug() << __FUNCTION__;
    emit ReadySend(FileNum, 4096);
}

void TcpFileThread::read()
{
    qDebug() << __FUNCTION__;
    std::list<MessagePtr> mlist;
    getMessage(*m_Socket, -1, mlist);
    for(auto mit = mlist.begin(); mit != mlist.end(); ++mit)
    {
        uint64_t FileNum = (*mit)->GetSpace(0);
        auto fit = m_FileNumStoreMap.find(FileNum);
        if(fit != m_FileNumStoreMap.end())
        {
            (*fit)->write((char *)((*mit)->data()), (*mit)->dataSize());
            auto it = m_DowloadFileMap.find(FileNum);
            assert(it != m_DowloadFileMap.end());

        }
    }
}

void TcpFileThread::AddFile(uint64_t FileNum, uint32_t Id, int FileCode, std::string FileName)
{
    std::shared_ptr<QFile> file(new QFile(QString::fromStdString(FileName)));
    SendFileItem item(FileNum, Id, file->size(), FileCode, FileName);
    m_SendFileMap.insert(FileNum, item);
    file->open(QIODevice::ReadOnly);
    qDebug() << " qfile size : " << file->size();
    m_FileNumOpenSendMap.insert(FileNum, file);
    send(FileNum);
}

bool TcpFileThread::SendtoRemote(MessagePtr m)
{
    qDebug() << __FUNCTION__ << "thread id : " << QThread::currentThreadId();
    if(m_Socket->state() != QAbstractSocket::ConnectedState) return false;
    int size = m_Socket->write((char *)m->tobuf(), m->size());
    if(size != m->size()) return false;
    if(!m_Socket->waitForBytesWritten())
    {
        qDebug() << __FUNCTION__ << "wirte error";
        return false;
    }
    qDebug() << "send size" << size;
    return true;
}

void TcpFileThread::SendedSignal(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    m_MainWin->EmitSendFileData(FileNum, Id, FileCode, Size);
}

void TcpFileThread::RealSend(uint64_t FileNum, uint32_t Size)
{
    static char buffer[8192];
    auto fit = m_FileNumOpenSendMap.find(FileNum);
    auto sit = m_SendFileMap.find(FileNum);
//    assert(fit != m_FileNumOpenSendMap.end() && sit != m_SendFileMap.end());
    if(fit != m_FileNumOpenSendMap.end() && sit != m_SendFileMap.end())
    {
        MessagePtr m;
        qDebug() << __FUNCTION__;
        if((*sit).NowLength >= (*sit).Length)
        {
            //发送完 发送完成包
            int FileCode = (*sit).FileCode;
            if(FileCode == FILEDATATRANSFER)
            {
                m = CreateFileTransferEndMsg(m_UserId, (*sit).Id, 0, (*sit).RemoteFileNum);
            }
            else if(FileCode == UPLOADUSERPROFILE || FileCode == UPLOADUSERSGROUPPROFILE)
            {
                m = CreateSendProfileEndMsg(m_UserId, 0, (*sit).RemoteFileNum);
            }
            else if(FileCode == UPLOADUSERFILE)
            {
                m = CreateSendUserFileToServerEndMsg(m_UserId, 0, (*sit).RemoteFileNum);;
            }
            else
            {
                return;
            }
            QMetaObject::invokeMethod(this, std::bind(&TcpFileThread::SendtoRemote, this, m));
//            SendtoRemote(m);
            m_MainWin->CloseSendFileNum((*sit).RemoteFileNum, false);
        }
        else
        {
            int realbyte = (*fit)->read(buffer, Size < 8192 ? Size : 8192);
            if((*sit).FileCode == FILEDATATRANSFER)
            {
                m = CreateFileDataTransferMsg(m_UserId, (*sit).Id, 0, buffer, realbyte, FileNum);
            }
            else
            {
                m = CreateFileDataUploadMsg(m_UserId, 0, buffer, realbyte, FileNum);
            }
//            SendtoRemote(m);
            QMetaObject::invokeMethod(this, std::bind(&TcpFileThread::SendtoRemote, this, m));
            (*sit).NowLength += realbyte;
            SendedSignal(FileNum, (*sit).Id, (*sit).FileCode, realbyte);
        }
    }
    else
    {
        m_FileNumOpenSendMap.remove(FileNum);
        m_SendFileMap.remove(FileNum);
    }
}

void TcpFileThread::timeslot()
{
    qDebug() << __FUNCTION__ << "thread id : " << QThread::currentThreadId();
}
