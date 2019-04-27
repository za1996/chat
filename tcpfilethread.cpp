#include "tcpfilethread.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

TcpFileThread::TcpFileThread() :
    m_UserId(0),
    m_Socket(nullptr)
{

}

void TcpFileThread::run()
{
    m_Socket = new QTcpSocket();
    m_mutex.unlock();
    m_Socket->connectToHost("129.204.4.80", 8080);
    if(m_UserId)
    {
        json info;
        info["RequestId"] = m_UserId;
    }
    else
    {
        return;
    }
    connect(m_Socket, SIGNAL(readyRead()), this, SLOT(read()));
    send();
}

void TcpFileThread::send()
{
    static char filebuffer[4096];
    for(;true;)
    {
        if(!m_FileQueue.size_approx() && !m_FileName)
        {
            m_FileEmpty.wait(&m_mutex);
        }
        std::string FileName;
        QFile file;
        if(m_FileQueue.try_dequeue(FileName))
        {
            file.setFileName(QString::fromStdString(FileName));
            file.open(QIODevice::ReadOnly);

        }
        else
        {
            continue;
        }
        uint64_t size = file.size();
        if(size > 0xfffffff)
        {

        }
        for(uint32_t sbyte = 0; sbyte < size;sleep(40))
        {
            int realbyte = file.read(filebuffer, 4096);
            auto m = CreateFileDataUploadMsg(m_UserId, 0, filebuffer, realbyte);
            SendtoRemote(*m_Socket, m);
        }

    }
}

void TcpFileThread::read()
{

}
