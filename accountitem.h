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
    void setAccountInfo(void *item, const QString& accountName, const QString& headFilePath, const QString& Password);
    QString getAccountName();
public slots:
    void onRemoveAccount() ;

private:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *event) ;
    QString getHeadImageDirPath();

Q_SIGNALS:
    void signalShowAccountInfo(QString headPicPath , QString accountName, QString Password);
    void signalRemoveAccount(void *) ;

private:
    bool m_mousePress;
    QLabel *m_accountNumber;
    QToolButton *m_deleteButton;
    QLabel *m_Icon;
    void *m_item;
    QString filePath;
    QString m_Password;
};


#endif // ACCOUNTITEM_H
