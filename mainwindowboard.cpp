#include "mainwindowboard.h"

MainWindowBoard::MainWindowBoard(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    m_MainWindow = new MainWindow(this);
    m_MainWindow->raise();
    this->resize(m_MainWindow->width() + 2 * SHADOWPAD, m_MainWindow->height() + 2 * SHADOWPAD);
    m_MainWindow->move(SHADOWPAD, SHADOWPAD);
}
