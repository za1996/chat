#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QDialog>

namespace Ui {
class RegisterWin;
}

class RegisterWin : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWin(QWidget *parent = 0);
    ~RegisterWin();

private:
    Ui::RegisterWin *ui;

private slots:
    void Register();
};

#endif // REGISTERWIN_H
