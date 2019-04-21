#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <memory>
#include <QDebug>
#include "messagetype.h"





#pragma pack(push)
#pragma pack(4)
struct MsgHead
{
    uint32_t src_id;
    uint32_t dest_id;
    uint32_t msg_type;
    uint32_t flag;
    uint32_t msg_len;
    uint32_t space[4];
};
#pragma pack(pop)

class Message : public std::enable_shared_from_this<Message>
{
public:
    static std::shared_ptr<Message> CreateObject()
    {
        std::shared_ptr<Message> obj(new Message);
        return obj->shared_from_this();
    }
    ~Message();

    void setHead(uint32_t srcId, uint32_t destId, uint32_t mType1, uint32_t mType2, uint32_t flag);
    void setHead(const MsgHead& head);

    void setData(const char* buf, const int n);
    void setData(const std::string &data);
    const void* data() { return m_data; }
    size_t size();

    const void* tobuf();

    bool hasHead() { return m_hasHead; }
    bool isError() { return TypeGroup() == SYSRESEVENTGROUP && TypeCode() == SYSRESERRORCODE; }
    uint32_t srcId() { return m_msgHead.src_id; }
    uint32_t destId() { return m_msgHead.dest_id; }
    uint32_t TypeGroup() { return TYPEGROUP(m_msgHead.msg_type); }
    uint32_t TypeCode() { return TYPEACTION(m_msgHead.msg_type); }
    uint32_t Type() { return m_msgHead.msg_type; }
    uint32_t dataSize() { return m_msgHead.msg_len; }

    QDebug& operator <<(QDebug &s);

private:
    Message();
    MsgHead m_msgHead;
    char *m_data;
    char *m_buf;
    bool m_hasHead;
    int m_dataSize;
};

typedef std::shared_ptr<Message> MessagePtr;

#endif // MESSAGE_H