#include "loginwindow.h"
#include <QApplication>
#include <QDebug>
#include <QtNetwork>
#include "chatwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    s.connectToHost("129.204.4.80", 8888);
    s.waitForConnected(10000);
    LoginWindow w;
    w.show();
    qRegisterMetaType<uint64_t>("uint64_t");
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<std::string>("std::string");

    qDebug("window return\n");

    return a.exec();
}
