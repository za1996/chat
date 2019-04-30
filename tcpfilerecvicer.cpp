#include "tcpfilerecvicer.h"
#include "global.h"
#include "mainwindow.h"

TcpFileRecvicer::TcpFileRecvicer() :
    m_UserId(0),
    m_Socket(nullptr)
{

}

void TcpFileRecvicer::run()
{
    m_Socket = new QTcpSocket();
    qDebug() << "run start";

    m_Socket->connectToHost("129.204.4.80", 8888);
    if(!m_Socket->waitForConnected())
    {
        qDebug() << "connect error";
    }
    m_Socket->setSocketOption(QAbstractSocket::LowDelayOption, true);
    qDebug() << "run end";
    for(;m_Socket->state() != QAbstractSocket::ConnectedState;sleep(1))
    {
        qDebug() << m_Socket->state();
    }

    if(m_UserId)
    {
        qDebug() << "ready send merge";
        auto m = CreateSocketMergeMsg(0, m_UserId, 2);
        if(!SendtoRemote(*m_Socket, m))
        {
            qDebug() << "send error";
        }
        qDebug() << "send merge end";
    }
    else
    {
        return;
    }
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(read()));
    this->exec();
}

void TcpFileRecvicer::read()
{
    qDebug() << __FUNCTION__;
    std::list<MessagePtr> mlist;
    getMessage(*m_Socket, -1, mlist);
    for(auto mit = mlist.begin(); mit != mlist.end(); ++mit)
    {
        uint32_t FileNum = (*mit)->GetSpace(0);
        qDebug() << "FileNum : " << FileNum;
        auto fit = m_FileNumStoreMap.find(FileNum);
        qDebug() << "find FileNum Map";
        if(fit != m_FileNumStoreMap.end())
        {
            (*fit)->write((char *)((*mit)->data()), (*mit)->dataSize());
            auto it = m_DowloadFileMap.find(FileNum);
            assert(it != m_DowloadFileMap.end());
            qDebug() << "Send SIGNAL";
            SendSignal(FileNum, (*it).Id, (*it).FileCode, (*mit)->dataSize());

        }
    }
}

void TcpFileRecvicer::SendSignal(uint32_t FileNum, uint32_t Id, int FileCode, int Size)
{
    m_MainWin->EmitRecvFileData(FileNum, Id, FileCode, Size);
}
void TcpFileRecvicer::wait()
{

}
