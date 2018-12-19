#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QMouseEvent>

class AccountItem : public QWidget
{
    Q_OBJECT

public:
    AccountItem(QWidget *parent = nullptr);

public:
    void setAccountInfo(int index, QString accountName, QString headFilePath);
    QString getAccountName();
public slots:
    void onRemoveAccount() ;

private:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *event) ;
    QString getHeadImageDirPath();

Q_SIGNALS:
    void signalShowAccountInfo(QString headPicPath , QString accountName);
    void signalRemoveAccount(int index) ;

private:
    bool m_mousePress;
    QLabel *m_accountNumber;
    QToolButton *m_deleteButton;
    QLabel *m_Icon;
    int m_index;
    QString filePath;
};


#endif // ACCOUNTITEM_H
