#include "loginwindow.h"
#include <QApplication>
#include <QDebug>
#include <QtNetwork>
#include "chatwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    s.connectToHost("129.204.126.195", 8888);
    s.waitForConnected(10000);
    LoginWindow w;
    w.show();


    qDebug("window return\n");

    return a.exec();
}
