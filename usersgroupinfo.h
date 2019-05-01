#ifndef USERSGROUPINFO_H
#define USERSGROUPINFO_H

#include <QWidget>
#include "titlebar.h"
#include "profileuploadwin.h"

namespace Ui {
class UsersGroupInfo;
}

class UsersGroupInfo : public QWidget
{
    Q_OBJECT

public:
    explicit UsersGroupInfo(QWidget *parent, const QString& Name, uint32_t id, const QString& Desc, const QString& Profile, bool Editable);
    ~UsersGroupInfo();

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::UsersGroupInfo *ui;
    TitleBar *m_TitleBar;
    uint32_t m_GroupId;
    ProfileUploadWin *m_ProfileWin;
private slots:
    void SendChangeUsersGroupInfo();
};

#endif // USERSGROUPINFO_H
