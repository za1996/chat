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
    qDebug() << "~Message";
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
