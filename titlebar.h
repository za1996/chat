#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>

namespace Ui {
    class TitleBar;
}

enum ButtonType
{
    MIN_BUTTON = 0,         // 最小化和关闭按钮;
    MIN_MAX_BUTTON ,        // 最小化、最大化和关闭按钮;
    ONLY_CLOSE_BUTTON       // 只有关闭按钮;
};



class TitleBar : public QWidget
{

    Q_OBJECT


public:
    TitleBar(QWidget *parent);

    ~TitleBar();
    // 设置背景颜色和是否透明
    void setBackgroundColor(int r, int g, int b , bool isTransparent = false);
    // 设置图标
    void setTitleIcon(QString filePath , QSize IconSize = QSize(25 , 25));
    // 设置标题
    void setTitleContent(QString titleContent , int titleFontSize = 9);
    // 设置标题栏长度
    void setTitleWidth(int width);
    // 设置按键类型
    void setButtonType(ButtonType buttonType);
    // 设置窗口边框宽度;
    void setWindowBorderWidth(int borderWidth);

    // 保存/获取 最大化前窗口的位置及大小;
    void saveRestoreInfo(const QPoint point, const QSize size);
    void getRestoreInfo(QPoint& point, QSize& size);

    // 设置margins 控件离边框的距离
    void setMargins(int left, int top, int right, int bottom);

    // 设置控件之间的间距
    void setSpacing(int value);

    // 加载样式文件;
    void loadStyleSheet(const QString &sheetPath);

private:
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // 初始化控件;
    void initControl();
    // 信号槽的绑定;
    void initConnections();



protected:


signals:
    void signalButtonMinClicked();
    void signalButtonRestoreClicked();
    void signalButtonMaxClicked();
    void signalButtonCloseClicked();

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();


    // 变量
private:
    QLabel* m_pIcon;                    // 标题栏图标;
    QLabel* m_pTitleContent;            // 标题栏内容;
    QPushButton* m_pButtonMin;          // 最小化按钮;
    QPushButton* m_pButtonRestore;      // 最大化还原按钮;
    QPushButton* m_pButtonMax;          // 最大化按钮;
    QPushButton* m_pButtonClose;        // 关闭按钮;
    QHBoxLayout* mylayout;

    // 标题栏背景色;
    int m_colorR;
    int m_colorG;
    int m_colorB;

    // 最大化，最小化变量;
    QPoint m_restorePos;
    QSize m_restoreSize;
    // 移动窗口的变量;
    bool m_isPressed;
    QPoint m_startMovePos;
    // 标题栏内容;
    QString m_titleContent;
    // 按钮类型;
    ButtonType m_buttonType;
    // 窗口边框宽度;
    int m_windowBorderWidth;
    // 标题栏是否透明;
    bool m_isTransparent;



};

#endif // TITLEBAR_H
