#ifndef DELETEUSERSGROUPMEMBERWIN_H
#define DELETEUSERSGROUPMEMBERWIN_H

#include <QWidget>
#include "titlebar.h"
#include "message.h"

namespace Ui {
class DeleteUsersGroupMemberWin;
}

class DeleteUsersGroupMemberWin : public QWidget
{
    Q_OBJECT

public:
    explicit DeleteUsersGroupMemberWin(QWidget *parent, const std::map<uint32_t, QString> &map, uint32_t GroupId, uint32_t MeId);
    ~DeleteUsersGroupMemberWin();

protected:
    void paintEvent(QPaintEvent *event);


private slots:
    void DeleteMember();
private:
    Ui::DeleteUsersGroupMemberWin *ui;
    TitleBar *m_TitleBar;
    uint32_t m_GroupId;
    uint32_t m_MeId;


public slots:

    void HandMessage(uint32_t, std::shared_ptr<Message>);

};

#endif // DELETEUSERSGROUPMEMBERWIN_H
