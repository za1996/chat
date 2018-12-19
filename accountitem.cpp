#include "accountitem.h"
#include <QHBoxLayout>
#include <QDebug>

AccountItem::AccountItem(QWidget *parent)
    :QWidget(parent)
{
    m_mousePress = false ;
    m_Icon = new QLabel() ;
    m_accountNumber = new QLabel() ;
    m_deleteButton = new QToolButton();
    m_deleteButton->setObjectName("AccountItemDeleteButton");
    m_deleteButton->setIcon(QIcon("D:/739946be5561bb83a2a46cfb95f259dd_hover.png"));
    m_deleteButton->setStyleSheet("QToolButton#AccountItemDeleteButton{background-color:transparent;}");

    connect(m_deleteButton , SIGNAL(clicked(bool)) , this , SLOT(onRemoveAccount())) ;

    // 布局;
    QHBoxLayout *mainLayout = new QHBoxLayout(this) ;
    mainLayout->addWidget(m_Icon);
    mainLayout->addWidget(m_accountNumber) ;
    mainLayout->addStretch();
    mainLayout->addWidget(m_deleteButton);
    mainLayout->setContentsMargins(5 , 5 , 5 , 5);
    mainLayout->setSpacing(5);

    //this->setLayout(mainLayout);
}


void AccountItem::setAccountInfo(int index , QString accountName, QString headFilePath)
{
    m_index = index;
    m_accountNumber->setText(accountName);
    filePath = headFilePath;
    m_Icon->setPixmap(QPixmap(filePath).scaled(30, 30));
}

QString AccountItem::getAccountName()
{
    return m_accountNumber->text() ;
}

void AccountItem::onRemoveAccount()
{
    emit signalRemoveAccount(m_index);
}

void AccountItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_mousePress = true;
    }
}

void AccountItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_mousePress)
    {
        emit signalShowAccountInfo(filePath , m_accountNumber->text());
        m_mousePress = false;
    }
}


