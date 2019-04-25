#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QTextEdit>
#include <QTimer>

class MessageWidget : protected QTextEdit
{
    Q_OBJECT
public:
    MessageWidget();
    void SetText(const QString &text);

    void SetWidth(int width);

    using QTextEdit::width;
    using QTextEdit::height;
    using QTextEdit::size;


protected:

private:
    bool m_HasText;
    QTimer m_timer;
    int m_oldWidth;


    bool onOneLine(const QRect &start, const QRect &end);


private slots:
    void areaChanged();

signals:
    void sizeChange();
};

#endif // MESSAGEWIDGET_H
