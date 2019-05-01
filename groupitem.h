#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QWidget>
#include <QLabel>


class GroupItem : public QWidget
{
    Q_OBJECT
public:
    explicit GroupItem(QWidget *parent = nullptr, void *item = nullptr, uint32_t Id = 0);
    ~GroupItem();

    void setProfile(const QString &profile);
    void setName(const QString &name);
    void setDesc(const QString &desc);
    void setDate(const QString &date);
    void setMessageCount(const int count);
    void hideDateLabel();
    void hideMessageCountLabel();

private:
    enum{HEIGHT = 60};
    QLabel *m_Profile;
    QLabel *m_Name;
    QLabel *m_Desc;
    QLabel *m_Date;
    QLabel *m_MsgCount;

    QAction *m_changeName;
    QAction *m_deleteFriend;
    QAction *m_changeGroup;
    QAction *m_showInfo;
    void *m_item;

    QBitmap createEllipseMask(const QSize &size, const QPoint &point, const int rx, const int ry);

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void changeNameSign(void *item);
    void changeGroupSign(void *item);
    void deleteFriendSign(void *item);
    void showInfoSign(void *item);

private slots:
    void changeName();
    void changeGroup();
    void deleteFriend();
    void showInfo();
};

#endif // GROUPITEM_H
