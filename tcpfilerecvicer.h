#ifndef TCPFILERECVICER_H
#define TCPFILERECVICER_H

#include <QThread>
#include <QTcpSocket>

class TcpFileRecvicer : public QThread
{
    Q_OBJECT
public:
    TcpFileRecvicer();
    void SetUserId(uint32_t Id) { m_UserId = Id; }
    void run();
    void RecvSignal(uint64_t FileNum, uint32_t Id, int FileCode, int Size);
    void wait();

private:
    uint32_t m_UserId;
    QTcpSocket *m_Socket;


private slots:
    void read();
};

#endif // TCPFILERECVICER_H
