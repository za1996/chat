#ifndef PROFILEUPLOADWIN_H
#define PROFILEUPLOADWIN_H

#include <QWidget>
#include <QImage>
#include <QLabel>

#include "titlebar.h"

namespace Ui {
class ProfileUploadWin;
}

class ProfileUploadWin : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileUploadWin(bool isGroup, uint32_t id, QWidget *parent = 0);
    ~ProfileUploadWin();

private:
    Ui::ProfileUploadWin *ui;
    TitleBar *m_TitleBar;
    QImage *m_Img;
    QLabel *m_Label;
    QString m_File;
    uint32_t m_UsersGroupId;
    bool m_isGroup;

protected:
    void paintEvent(QPaintEvent *event);


private slots:
    void SelectFile();
    void UploadImg();
    void ReadImg();


};

#endif // PROFILEUPLOADWIN_H
