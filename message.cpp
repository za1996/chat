#include "message.h"
#include <algorithm>
#include <cstring>
#include <QDebug>

Message::Message() :
    m_data(nullptr),
    m_buf(nullptr),
    m_dataSize(0),
    m_hasHead(false)
{
    m_msgHead.msg_len = 0;
}

Message::~Message()
{
    if(m_buf) delete[] m_buf;
}

void Message::setHead(const MsgHead &head)
{
    m_msgHead = head;
    m_hasHead = true;
}

void Message::setHead(uint32_t srcId, uint32_t destId, uint32_t mType1, uint32_t mType2, uint32_t flag)
{
    m_msgHead.src_id = srcId;
    m_msgHead.dest_id = destId;
    m_msgHead.msg_type = MESSAGETYPE(mType1, mType2);
    m_msgHead.flag = flag;
    m_msgHead.msg_len = 0;
    m_hasHead = true;
}

void Message::setUdpInfo(uint16_t totalSize, uint16_t packetStart, uint32_t packetNum, uint64_t time)
{
    m_msgHead.space[0] = (totalSize << 16) | packetStart;
    m_msgHead.space[1] = packetNum;
    memcpy(&m_msgHead.space[2], &time, sizeof(uint64_t));
}

void Message::setTcpFileNum(uint64_t FileNum)
{
    memcpy(&m_msgHead.space[0], &FileNum, sizeof(uint64_t));
}

void Message::setData(const char *buf, const int n)
{
    m_buf = new char[n + sizeof(MsgHead)];
    m_data = m_buf + sizeof(MsgHead);
    memcpy(m_data, buf, n);
    if(hasHead()) m_msgHead.msg_len = n;
    m_dataSize = n;
}

void Message::setData(const std::string &data)
{
    setData(data.c_str(), data.size() + 1);
}

void Message::setRawSize(const int n)
{
    if(m_buf) delete[] m_buf;
    m_buf = new char[n];
    m_dataSize = n - sizeof(MsgHead);
    m_hasHead = true;
}

uint64_t Message::GetFileNum()
{
    uint64_t FileNum = 0;
    memcpy(&FileNum, &m_msgHead.space[0], sizeof(uint64_t));
    return FileNum;
}

const void* Message::tobuf()
{
    if(m_buf && hasHead())
    {
        memcpy(m_buf, &m_msgHead, sizeof(MsgHead));
        return m_buf;
    }
    else if(hasHead())
    {
        return &m_msgHead;
    }

    return nullptr;
}

void Message::refresh()
{
    memcpy(&m_msgHead, m_buf, sizeof(MsgHead));
    m_data = m_buf + sizeof(MsgHead);
    m_hasHead = true;
}

size_t Message::size()
{
    return m_dataSize + (hasHead() ? sizeof(MsgHead) : 0);
}

QDebug& Message::operator <<(QDebug &q)
{
    q << "srcId : " << srcId();
    q << "destId : " << destId();
    q << "mainType : " << TypeGroup();
    q << "secType : " << TypeCode();
    q << "msgLen : " << dataSize();
    if(dataSize() > 0) q << m_data;
    return q;
}

void Message::showUdpInfo()
{
    uint64_t time;
    memcpy(&time, &m_msgHead.space[2], sizeof(uint64_t));
    qDebug() << "totalSize:" << (m_msgHead.space[0] >> 16) << " PacketStart:" << (m_msgHead.space[0] & 0xffff) << " PacketNum:" << m_msgHead.space[1] << " time:" << time;
}

void Message::getUdpInfo(uint32_t &packetNum, uint16_t &totalSize, uint16_t &packetStart, uint64_t &time)
{
    packetNum = m_msgHead.space[1];
    totalSize = (m_msgHead.space[0] >> 16);
    packetStart = (m_msgHead.space[0] & 0xffff);
    memcpy(&time, &m_msgHead.space[2], sizeof(uint64_t));
}
