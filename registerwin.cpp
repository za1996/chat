#include "registerwin.h"
#include "ui_registerwin.h"
#include "global.h"

#include <QRegExp>
#include <QMessageBox>

RegisterWin::RegisterWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterWin)
{
    ui->setupUi(this);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);
    QRegExp rx("[a-zA-Z0-9][a-zA-Z0-9.-]+$");
    QRegExpValidator *validator = new QRegExpValidator(rx);
    ui->PwLineEdit->setValidator(validator);
    ui->AgainLineEdit->setValidator(validator);
    ui->PwLineEdit->setMaxLength(16);
    ui->AgainLineEdit->setMaxLength(16);
    ui->PwLineEdit->setEchoMode(QLineEdit::Password);
    ui->AgainLineEdit->setEchoMode(QLineEdit::Password);
    this->setWindowTitle("注册");
    connect(ui->CancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->OKButton, SIGNAL(clicked(bool)), this, SLOT(Register()));
}

RegisterWin::~RegisterWin()
{
    delete ui;
}

void RegisterWin::Register()
{
    QString Name = ui->NameLineEdit->text();
    QString Pwd = ui->PwLineEdit->text();
    QString AgainPwd = ui->AgainLineEdit->text();
    QString Answer = ui->AnswerLineEdit->text();
    if(Name.isEmpty() || Pwd.isEmpty() || AgainPwd.isEmpty() || Answer.isEmpty())
    {
        QMessageBox message(QMessageBox::Question,tr("警告"), tr("不能为空"), QMessageBox::Yes, NULL);
        message.setWindowFlags(Qt::WindowStaysOnTopHint);
        message.exec();
    }
    else
    {
        if(Pwd != AgainPwd)
        {
            QMessageBox message(QMessageBox::Question, tr("警告"), tr("密码不同"), QMessageBox::Yes, NULL);
            message.setWindowFlags(Qt::WindowStaysOnTopHint);
            message.exec();
        }
        else
        {
            auto m = CreateRegisterAccountMsg(0, Name, Pwd, Answer);
            SendtoRemote(s, m);
            std::list<MessagePtr> mlist;
            if(WaitForRead(s, mlist, 1))
            {
                m = mlist.front();
                json info = json::parse((char *)m->data());
                if(!m->isError())
                {
                    QString Text = QString("您的账号:%1").arg(info["ID"].get<json::number_unsigned_t>());
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
            else
            {
                QMessageBox message(QMessageBox::Question,tr("警告"), tr("连接出错"), QMessageBox::Yes, NULL);
                message.setWindowFlags(Qt::WindowStaysOnTopHint);
                message.exec();
            }
        }
    }
}
