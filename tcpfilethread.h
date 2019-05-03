#ifndef TCPFILETHREAD_H
#define TCPFILETHREAD_H


#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>
#include <QFile>
#include "global.h"
class TcpFileThread : public QThread
{
    Q_OBJECT
public:
    TcpFileThread();
    void SetUserId(uint32_t Id) { m_UserId = Id; }
    void run();
    void send(uint64_t FileNum);
    void AddFile(uint64_t FileNum, uint32_t Id, int FileCode, std::string FileName);
    void SendedSignal(uint64_t FileNum, uint32_t Id, int FileCode, int Size);

private:
    uint32_t m_UserId;
    QTcpSocket *m_Socket;
    QWaitCondition m_FileEmpty;
    QWaitCondition m_PauseCond;
    QString *m_FileName;


    QMutex *m_mutex;
    QMutex *m_PauseMutex;

    bool SendtoRemote(MessagePtr m);

signals:
    void ReadySend(uint64_t, uint32_t);

private slots:
    void read();
    void RealSend(uint64_t , uint32_t);
};

#endif // TCPFILETHREAD_H
