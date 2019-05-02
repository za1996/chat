#ifndef SYSTEMMESSAGEWIN_H
#define SYSTEMMESSAGEWIN_H
#include "titlebar.h"
#include <QWidget>

namespace Ui {
class SystemMessageWin;
}

class SystemMessageWin : public QWidget
{
    Q_OBJECT

public:
    explicit SystemMessageWin(QWidget *parent = 0);
    ~SystemMessageWin();



private:
    Ui::SystemMessageWin *ui;
    TitleBar *m_TitleBar;
    void HandleMessage(int n);

};

#endif // SYSTEMMESSAGEWIN_H
