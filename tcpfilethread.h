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

private:
    uint32_t m_UserId;
    QTcpSocket *m_Socket;
    QWaitCondition m_FileEmpty;
    QString *m_FileName;


    QMutex m_mutex;

    void send();

private slots:
    void read();
};

#endif // TCPFILETHREAD_H
