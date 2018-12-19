#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include "titlebar.h"


#include <QMainWindow>
#include <QString>
#include <QListWidget>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    void loadStyleSheet(const QString &sheetPath);

private:
    void init();
    void initConnections();
    void loadTopGif(const QString &gifPath);
    void initLoginBorder();
    void initTitleBar();
    void showTitleBar();
    void creatShadow(QWidget *widget);
    void loadAccountList();


private slots:
    void onCloseButtonClicked();
    void onMinButtonClicked();
    void onShowAccountInfo(QString filePath, QString accountName);
    void onLoginIn();
    void onRemoveAccount(int m_item);

private:
    Ui::LoginWindow *ui;

    int m_borderRadius;
    int m_shadowPad;
    QString m_gifPath;
    TitleBar *m_titleBar;
    QListWidget *m_accountList;




};

#endif // LOGINWINDOW_H
