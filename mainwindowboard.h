#ifndef MAINWINDOWBOARD_H
#define MAINWINDOWBOARD_H

#include <QWidget>

#include "mainwindow.h"

class MainWindowBoard : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowBoard(QWidget *parent = nullptr);

private:
    enum {SHADOWPAD = 10};
    MainWindow *m_MainWindow;
signals:

public slots:
};

#endif // MAINWINDOWBOARD_H
