#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QWidget>

namespace Ui {
class VideoWindow;
}

class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = 0);
    ~VideoWindow();

private:
    Ui::VideoWindow *ui;
};

#endif // VIDEOWINDOW_H
