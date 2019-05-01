#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QTextEdit>
#include <QTimer>

class MessageWidget : public QTextEdit
{
    Q_OBJECT
public:
    MessageWidget(int width, QWidget *parent = nullptr);
    void SetText(const QString &text);

    void SetWidth(int width);

//    using QTextEdit::width;
//    using QTextEdit::height;
//    using QTextEdit::size;
//    using QTextEdit::move;
//    using QTextEdit::hide;
//    using QTextEdit::show;


protected:

private:
    bool m_HasText;
    QTimer m_timer;
    int m_oldWidth;


    bool onOneLine(const QRect &start, const QRect &end);


private slots:
    void areaChanged(int width);

signals:
    void sizeChange(int width);
};

#endif // MESSAGEWIDGET_H
