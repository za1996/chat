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
    void AddFile(uint32_t FileNum, uint32_t Id, int FileCode, std::string FileName);
    void SendSignal(uint32_t FileNum, uint32_t Id, int FileCode, int Size);

private:
    uint32_t m_UserId;
    QTcpSocket *m_Socket;
    QWaitCondition m_FileEmpty;
    QWaitCondition m_PauseCond;
    QString *m_FileName;


    QMutex *m_mutex;
    QMutex *m_PauseMutex;

    void send();
    bool SendtoRemote(MessagePtr m);

private slots:
    void read();
};

#endif // TCPFILETHREAD_H
