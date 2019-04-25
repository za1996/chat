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
    void SetVideoPic(const QPixmap &pixmap);

private:
    Ui::VideoWindow *ui;

signals:
    void VideoWindowCloseButtonClick();

private slots:
    void OnClosePress();
};

#endif // VIDEOWINDOW_H
