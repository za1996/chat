#ifndef GLOBAL_H
#define GLOBAL_H

#include <list>
#include <vector>
#include <memory>
#include <QtNetwork>
#include "message.h"

int getMessage(QTcpSocket &s, int n, std::list<std::shared_ptr<Message>> &mlist);
bool SendtoRemote(QTcpSocket &s, MessagePtr m);
bool WaitForRead(QTcpSocket &s, std::list<MessagePtr> &mlist, const int count);
MessagePtr CreateReqUserInfoMsg(uint32_t reqID, uint32_t srcID, uint32_t flag);
MessagePtr CreateChangeUserNameMsg(uint32_t changeID, const QString &Name, uint32_t srcID, uint32_t flag);
MessagePtr CreateAddUserGroupMsg(const QString &UserGroupName, uint32_t srcID, uint32_t flag);
MessagePtr CreateDelFriendsMsg(const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag);
MessagePtr CreateChangeFriendsGroupMsg(uint32_t GroupId, const std::vector<uint32_t> &IDList, uint32_t srcID, uint32_t flag);
extern QTcpSocket s;

#endif // GLOBAL_H
