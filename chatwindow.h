#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>

#include "titlebar.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = 0);
    ~ChatWindow();

private:
    Ui::ChatWindow *ui;
    TitleBar *m_TitleBar;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void ReqVideoChat();
};

#endif // CHATWINDOW_H
