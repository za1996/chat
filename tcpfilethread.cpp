#include "tcpfilethread.h"
#include "message.h"
#include "mainwindow.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

TcpFileThread::TcpFileThread() :
    m_UserId(0),
    m_Socket(nullptr)
{
    m_mutex = new QMutex();
}

void TcpFileThread::run()
{
    m_Socket = new QTcpSocket();
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
        sleep(1);
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
    send();
}

void TcpFileThread::send()
{
    static char filebuffer[4096];
    for(;true;)
    {
        QMutexLocker lock(m_mutex);
        qDebug() << __FUNCTION__;
        if(!m_FileQueue.size_approx())
        {
            qDebug() << "tcpthread sleep";
            m_FileEmpty.wait(lock.mutex());
            qDebug() << "tcpthread wake up";
        }

        SendFileItem item;
        QFile file;
        if(m_FileQueue.try_dequeue(item))
        {
            qDebug() << __FUNCTION__ << "ready open";
            file.setFileName(QString::fromStdString(item.FileName));
            file.open(QIODevice::ReadOnly);

        }
        else
        {
            continue;
        }
        uint64_t size = file.size();
        qDebug() << __FUNCTION__ << "ready send";
        if(size > 0xfffffff)
        {

        }
        for(uint32_t sbyte = 0; sbyte < size; msleep(40))
        {
            int realbyte = file.read(filebuffer, 4096);
            auto m = CreateFileDataUploadMsg(m_UserId, 0, filebuffer, realbyte, item.RemoteFileNum);
            SendtoRemote(m);
            sbyte += realbyte;
        }
        auto m = CreateSendProfileEndMsg(m_UserId, 0, item.RemoteFileNum);
        SendtoRemote(m);

    }
}

void TcpFileThread::read()
{
    qDebug() << __FUNCTION__;
    std::list<MessagePtr> mlist;
    getMessage(*m_Socket, -1, mlist);
    for(auto mit = mlist.begin(); mit != mlist.end(); ++mit)
    {
        uint32_t FileNum = (*mit)->GetSpace(0);
        auto fit = m_FileNumStoreMap.find(FileNum);
        if(fit != m_FileNumStoreMap.end())
        {
            (*fit)->write((char *)((*mit)->data()), (*mit)->dataSize());
            auto it = m_DowloadFileMap.find(FileNum);
            assert(it != m_DowloadFileMap.end());
            SendSignal(FileNum, (*it).Id, (*it).FileCode, (*mit)->dataSize());

        }
    }
}

void TcpFileThread::AddFile(uint32_t FileNum, std::string FileName)
{
    SendFileItem item(FileNum, FileName);
    m_FileQueue.enqueue(item);
    m_FileEmpty.notify_one();
}

bool TcpFileThread::SendtoRemote(MessagePtr m)
{
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

void TcpFileThread::SendSignal(uint32_t FileNum, uint32_t Id, int FileCode, int Size)
{
    m_MainWin->EmitRecvFileData(FileNum, Id, FileCode, Size);
}
