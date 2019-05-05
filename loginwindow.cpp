#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "titlebar.h"
#include "accountitem.h"
#include "mainwindow.h"
#include "message.h"
#include "global.h"
#include "registerwin.h"

#include <nlohmann/json.hpp>

#include <memory>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QBitmap>
#include <QPainter>
#include <QMovie>
#include <QLineEdit>
#include <QFile>
#include <QDebug>
#include <QtNetwork>
#include <list>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    init();
    loadTopGif(m_gifPath);
    creatShadow(ui->LoginBorder);
    showTitleBar();
    ui->headPix->setStyleSheet("QLabel#headPix{background-color:rgb(255 , 255 , 255);}");
    creatShadow(ui->headPix);
    loadAccountList();
    this->loadStyleSheet("D:/style.css");

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::CreateDatabase()
{
    QString error_tip  = "%1:[%2]%3";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./data_station.db");
    if(!db.open())
    {
        qDebug() << error_tip.arg("open data base 'data_station' failed").arg(db.lastError().type()).arg(db.lastError().text());
    }
    QString CreateUserTable = "CREATE TABLE IF NOT EXISTS User(Id INT PRIMARY KEY, Password VARCHAR(255) default '', Profile VARCHAR(255) default '');";
    QString UserChatInfoTable = "CREATE TABLE IF NOT EXISTS ChatInfo(Id INT PRIMARY KEY, Sender INT, Recvicer INT, Message VARCHAR(255));";
    QSqlQuery db_operator(db);
    if(!db_operator.exec(CreateUserTable))
    {
        qDebug() << error_tip.arg("create table 'data_list' failed").arg(db_operator.lastError().type()).arg(db_operator.lastError().text());
    }
    if(!db_operator.exec(UserChatInfoTable))
    {
        qDebug() << error_tip.arg("create table 'data_list' failed").arg(db_operator.lastError().type()).arg(db_operator.lastError().text());
    }
}

void LoginWindow::init()
{
    // 初始化各种变量(后面改成函数)
    this->m_borderRadius = 15;
    this->m_shadowPad = 20;
    this->m_gifPath = QString("D:/tumblr_ojbecmgIT51qeyvpto1_500.gif");


    // 初始化窗口的设置
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initLoginBorder();
    initTitleBar();


    // 账号行初始化
    ui->accountComboBox->setEditable(true);
    QLineEdit *accountEdit = ui->accountComboBox->lineEdit();
    accountEdit->setPlaceholderText("号码");

    // 头像mask
    QBitmap mask(ui->headPix->size());
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), 3, 3);
    ui->headPix->setMask(mask);

    // 密码行初始化
    ui->passwordEdit->setPlaceholderText("密码");
//    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    // 初始化信号和槽的绑定
    // do something...
    initConnections();

    if(QFileInfo("./data_station.db").isFile())
    {
        QString error_tip  = "%1:[%2]%3";
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("./data_station.db");
        if(!db.open())
        {
            qDebug() << error_tip.arg("open data base 'data_station' failed").arg(db.lastError().type()).arg(db.lastError().text());
        }
    }
    else
    {
        CreateDatabase();
    }
}

void LoginWindow::loadTopGif(const QString &gifPath)
{
    QLabel *pBack = new QLabel(this);
    QMovie *movie = new QMovie();

    pBack->setGeometry(m_shadowPad, m_shadowPad, this->width() - m_shadowPad * 2, (this->height() - m_shadowPad * 2) * 0.55);
    QBitmap mask(pBack->size());
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), m_borderRadius, m_borderRadius);
    painter.setPen(QColor(0, 0, 0));
    for(int i = 0; i < m_borderRadius; i++)
        painter.drawLine(QPoint(0, mask.height() - i), QPoint(mask.width(), mask.height() - i));

    pBack->setMask(mask);
    movie->setFileName(gifPath);
    pBack->setMovie(movie);
    movie->start();
}

void LoginWindow::initLoginBorder()
{
    ui->LoginBorder->move(m_shadowPad, m_shadowPad);
    ui->LoginBorder->setFixedSize(this->width() - m_shadowPad * 2, this->height() - m_shadowPad * 2);
    //ui->LoginBorder->setStyleSheet("border-radius : 15;background-color:rgb(255, 255, 255);");
}

void LoginWindow::creatShadow(QWidget *widget)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor("#444444"));
    //设置阴影圆角
    shadow->setBlurRadius(30);
    //给嵌套QWidget设置阴影
    widget->setGraphicsEffect(shadow);
}

void LoginWindow::initTitleBar()
{
    m_titleBar = new TitleBar(this);
    m_titleBar->setBackgroundColor(0, 0, 0, true);
    m_titleBar->move(m_shadowPad, m_shadowPad);
    m_titleBar->setWindowBorderWidth(2 * m_shadowPad);
    m_titleBar->setButtonType(MIN_BUTTON);
    m_titleBar->setMargins(5, 0, m_borderRadius, 0);
    m_titleBar->loadStyleSheet("D:/titlebarstyle.css");
}

void LoginWindow::showTitleBar()
{
    m_titleBar->raise();
    m_titleBar->show();
}

void LoginWindow::initConnections()
{
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onCloseButtonClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onMinButtonClicked()));
    connect(ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(onLoginIn()));
    connect(ui->registerAccount, SIGNAL(clicked(bool)), this, SLOT(onRegister()));
    connect(ui->findPassword, SIGNAL(clicked(bool)), this, SLOT(onFindPassword()));
}

void LoginWindow::onCloseButtonClicked()
{
    this->close();
}

void LoginWindow::onMinButtonClicked()
{
    this->showMinimized();
}

void LoginWindow::loadStyleSheet(const QString &sheetPath)
{
    QFile file(sheetPath);
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void LoginWindow::loadAccountList()
{
    m_accountList = new QListWidget(this);
    ui->accountComboBox->setModel(m_accountList->model());
    ui->accountComboBox->setView(m_accountList);

//    QString pic[3] = {"59864232_p0.png", "59888355_p0.png", "59770022_p0.jpg"};

//    for (int i = 0; i < 3; i ++)
//    {
//        AccountItem *account_item = new AccountItem();
//        account_item->setAccountInfo(i, pic[i], QString("D:/59864232_p0.png"));
//        connect(account_item, SIGNAL(signalShowAccountInfo(QString, QString)), this, SLOT(onShowAccountInfo(QString , QString)));
//        connect(account_item, SIGNAL(signalRemoveAccount(int)), this, SLOT(onRemoveAccount(int)));
//        QListWidgetItem *list_item = new QListWidgetItem(m_accountList);
//        m_accountList->setItemWidget(list_item, account_item);
//    }
    QString SelSql = "SELECT * FROM User;";
    QString error_tip  = "%1:[%2]%3";
    QSqlQuery db_operator(db);

    if(!db_operator.exec(SelSql))
    {
        qDebug() << error_tip.arg("select failed").arg(db_operator.lastError().type()).arg(db_operator.lastError().text());
    }
    else
    {
        qDebug() << SelSql;
        while(db_operator.next())
        {
            AccountItem *account_item = new AccountItem();
            QListWidgetItem *list_item = new QListWidgetItem(m_accountList);
            uint32_t Id = db_operator.value(0).toUInt();
            QString Profile = db_operator.value(2).toString();
            QString FullPath = UserProfileCachePath(Id, Profile);
            if(!Profile.isEmpty() && QFileInfo(FullPath).isFile())
            {

            }
            else
            {
                FullPath = "D:/imggggg.bmp";
            }
            account_item->setAccountInfo(list_item, QString("%1").arg(Id), FullPath, db_operator.value(1).toString());
            connect(account_item, SIGNAL(signalShowAccountInfo(QString,QString,QString)), this, SLOT(onShowAccountInfo(QString,QString,QString)));
            connect(account_item, SIGNAL(signalRemoveAccount(void*)), this, SLOT(onRemoveAccount(void*)));
            m_accountList->setItemWidget(list_item, account_item);
        }

    }

}

void LoginWindow::onShowAccountInfo(QString filePath, QString accountName, QString Password)
{
    ui->accountComboBox->setEditText(accountName);
    ui->accountComboBox->hidePopup();

    ui->headPix->setPixmap(QPixmap(filePath).scaled(ui->headPix->width(), ui->headPix->height()));
    if(!Password.isEmpty())
    {
        ui->passwordEdit->setText(Password);
    }
}

void LoginWindow::onRemoveAccount(void *item)
{
    QListWidgetItem *listItem = static_cast<QListWidgetItem *>(item);
    AccountItem *p = dynamic_cast<AccountItem *>(m_accountList->itemWidget(listItem));
    uint32_t Id = p->getAccountName().toUInt();
    QString error_tip  = "%1:[%2]%3";
    QSqlQuery db_operator(db);
    QString del_sql = "DELETE FROM User WHERE Id = %1;";
    if(!db_operator.exec(del_sql.arg(Id)))
    {
        qDebug() << error_tip.arg("insert failed").arg(db_operator.lastError().type()).arg(db_operator.lastError().text());
    }
    m_accountList->removeItemWidget(listItem);
    delete listItem;
}

void LoginWindow::onLoginIn()
{
    using json = nlohmann::json;
    auto m = Message::CreateObject();
    m->setHead(ui->accountComboBox->currentText().toInt(), SERVERID, LOGINEVENTGROUP, LOGINACTION, 0);
    json j;
    j["ID"] = ui->accountComboBox->currentText().toInt();
    j["PWD"] = ui->passwordEdit->text().toStdString();
    qDebug() << "pwd : " << ui->passwordEdit->text().toStdString().c_str();
    std::string str = j.dump();
    m->setData(str.c_str(), str.size() + 1);
    s.write((char *)m->tobuf(), m->size());
    qDebug() << "socket state : " << s.state();
    while(s.waitForReadyRead())
    {
        std::list<MessagePtr> mlist;
        if(getMessage(s, 1, mlist) <= 0) continue;
        else
        {
            qDebug() << "list size : " << mlist.size();
            m = mlist.front();
            break;
        }
    }
    if(!m->isError())
    {
        QString error_tip  = "%1:[%2]%3";
        QSqlQuery db_operator(db);
        QString Password;
        uint32_t Id = ui->accountComboBox->currentText().toUInt();
        QString insert_sql = "INSERT OR REPLACE INTO User(Id, Password) VALUES(%1, '%2');";
        if(ui->savePassword->isChecked())
        {
            Password = ui->passwordEdit->text();
        }
        else
        {
            Password = "";
        }
        if(!db_operator.exec(insert_sql.arg(Id).arg(Password)))
        {
            qDebug() << error_tip.arg("insert failed").arg(db_operator.lastError().type()).arg(db_operator.lastError().text());
        }
        db.commit();
        m_MainWin = new MainWindow(ui->accountComboBox->currentText().toInt());
        m_MainWin->show();
        this->close();
    }
    else
    {
        qDebug() << "data : " << m->data();
        json info = json::parse((char *)m->data());
        std::string str = info["Msg"].get<std::string>();
        QMessageBox::information(nullptr, "Error", QString::fromStdString(str), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void LoginWindow::onLoginRes()
{
//    std::list<std::shared_ptr<Message>> mlist = getMessage(s, 1);
//    qDebug() << "mlist size : " << mlist.size();
//    if(mlist.size() < 1) return;
//    std::shared_ptr<Message> &m = mlist.front();
//    QDebug q = qDebug();
//    m->operator <<(q);
}

void LoginWindow::onRegister()
{
    RegisterWin *w = new RegisterWin();
    w->show();
}

void LoginWindow::onFindPassword()
{
    bool ok;
    uint32_t Id = QInputDialog::getInt(nullptr, "输入", tr("请输入账号:"), 0, 0, 2147483647, 1, &ok);
    if(ok)
    {
        QString Text = QInputDialog::getText(nullptr, "输入", tr("您父母的姓名:"), QLineEdit::Normal, "", &ok);
        if(Text.isEmpty())
        {
            QMessageBox message(QMessageBox::Question,tr("警告"), tr("不能为空"), QMessageBox::Yes, NULL);
            message.setWindowFlags(Qt::WindowStaysOnTopHint);
            message.exec();
        }
        else
        {
            auto m = CreateFindPasswordMsg(0, Id, Text.toStdString());
            SendtoRemote(s, m);
            std::list<MessagePtr> mlist;
            if(WaitForRead(s, mlist, 1))
            {
                m = mlist.front();
                json info = json::parse((char *)m->data());
                if(!m->isError())
                {
                    QString Text = QString("您的密码:%1").arg(QString::fromStdString(info["Password"].get<std::string>()));
                    QMessageBox message(QMessageBox::Information, tr("警告"), Text, QMessageBox::Yes, NULL);
                    message.setWindowFlags(Qt::WindowStaysOnTopHint);
                    message.exec();
                }
                else
                {
                    QMessageBox message(QMessageBox::Question, tr("警告"), QString::fromStdString(info["Msg"].get<std::string>()), QMessageBox::Yes, NULL);
                    message.setWindowFlags(Qt::WindowStaysOnTopHint);
                    message.exec();
                }
            }
        }
    }
}

