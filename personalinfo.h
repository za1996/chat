#ifndef PERSONALINFO_H
#define PERSONALINFO_H

#include <QWidget>

#include <titlebar.h>
#include "profileuploadwin.h"

namespace Ui {
class PersonalInfo;
}

class PersonalInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PersonalInfo(QWidget *parent, const QString &Name, uint32_t id, int Sex,const QString &Date, const QString& Desc, const QString& Profile, bool Editable);
    ~PersonalInfo();

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);


private:
    Ui::PersonalInfo *ui;

    TitleBar *m_TitleBar;
    ProfileUploadWin *m_ProfileWin;
private slots:
    void ChangeMyselfInfo();

public slots:
    void ChangeProfile(uint32_t, const QString&);
};

#endif // PERSONALINFO_H
