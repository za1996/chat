#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <memory>
#include <QDebug>
#include "messagetype.h"

class Message;
typedef std::shared_ptr<Message> MessagePtr;
struct SysMsgCacheItem
{
    bool OnlyShow;
    bool Finish;
    bool isOK;
    MessagePtr m;
    SysMsgCacheItem() : OnlyShow(true), Finish(true), isOK(true) {}

    SysMsgCacheItem(bool onlyshow, bool finish, bool ok, const MessagePtr& msg) :
        OnlyShow(onlyshow), Finish(finish), isOK(ok), m(msg) {}
};

struct SendFileItem
{
    uint64_t RemoteFileNum;
    uint32_t Id;
    int Length;
    int NowLength;
    int FileCode;
    std::string FileName;
    SendFileItem() : RemoteFileNum(0) {}
    SendFileItem(uint64_t FileNum, uint32_t id, int length, int code, const std::string Name) :
        RemoteFileNum(FileNum), Length(length), Id(id), FileCode(code), FileName(Name), NowLength(0) {}
};

struct DowloadFileItem
{
    uint64_t FileNum;
    uint32_t Id;
    int Length;
    int FileCode;
    std::string Name;
    std::string LocalPath;
    DowloadFileItem() {}
    DowloadFileItem(uint64_t FileNum, uint32_t id, int length, int code, const std::string &name, const std::string &path) :
        FileNum(FileNum), Id(id), Length(length), FileCode(code), Name(name), LocalPath(path) {}
};


struct UdpPacket
{
    uint32_t packetNum;
    uint64_t time;
    uint16_t size;
    int totalSize;
    uint32_t bitlist;
    std::vector<uchar> data;

    UdpPacket() : packetNum(0), time(0), size(0), totalSize(0), bitlist(0) {}
};



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
    void setUdpInfo(uint16_t totalSize, uint16_t packetStart, uint32_t packetNum, uint64_t time);
    void setTcpFileNum(uint64_t FileNum);

    void setData(const char* buf, const int n);
    void setData(const std::string &data);
    void setRawSize(const int n);
    void refresh();
    const void* data() { return m_data; }
    size_t size();

    const void* tobuf();
    char* rawBuf() { return m_buf; }

    bool hasHead() { return m_hasHead; }
    bool isError() { return TypeGroup() == SYSRESEVENTGROUP && TypeCode() == SYSRESERRORCODE; }
    uint32_t srcId() { return m_msgHead.src_id; }
    uint32_t destId() { return m_msgHead.dest_id; }
    uint32_t TypeGroup() { return TYPEGROUP(m_msgHead.msg_type); }
    uint32_t TypeCode() { return TYPEACTION(m_msgHead.msg_type); }
    uint32_t Type() { return m_msgHead.msg_type; }
    uint32_t dataSize() { return m_msgHead.msg_len; }
    uint32_t GetSpace(int n) { return m_msgHead.space[n]; }
    uint64_t GetFileNum();

    void getUdpInfo(uint32_t &packetNum, uint16_t &totalSize, uint16_t &packetStart, uint64_t &time);

    QDebug& operator <<(QDebug &s);
    void showUdpInfo();

private:
    Message();
    MsgHead m_msgHead;
    char *m_data;
    char *m_buf;
    bool m_hasHead;
    int m_dataSize;
};


#endif // MESSAGE_H
