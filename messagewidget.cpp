#include "messagewidget.h"
#include <QDebug>

MessageWidget::MessageWidget(QWidget *parent) :
    QTextEdit(parent),
    m_oldWidth(0)
{
    connect(this, SIGNAL(sizeChange(int)), this, SLOT(areaChanged(int)));
//    connect(&m_timer, SIGNAL(timeout()), this, SLOT(areaChanged()));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->resize(20, 20);
    this->setReadOnly(true);
    this->setStyleSheet("background:transparent;border-width:0;border-style:outset");
}


void MessageWidget::areaChanged(int width)
{
    qDebug() << __FUNCTION__;
    this->setMaximumWidth(width);
    if(true)
    {
        this->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
        QRect start = this->cursorRect();
        this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        QRect end = this->cursorRect();
        qDebug("Cursor Start Pos : %d %d %d %d\n", start.x(), start.y(), start.width(), start.height());
        qDebug("Cursor End Pos : %d %d %d %d\n", end.x(), end.y(), end.width(), end.height());
        if(!onOneLine(start, end))
        {
            this->setMinimumWidth(width);
            this->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
            start = this->cursorRect();
            this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            end = this->cursorRect();
            qDebug("New Cursor Start Pos : %d %d %d %d\n", start.x(), start.y(), start.width(), start.height());
            qDebug("New Cursor End Pos : %d %d %d %d\n", end.x(), end.y(), end.width(), end.height());
            int len = end.x() - start.x();
            if(onOneLine(start, end) && len < width - 10)
            {
                this->setMinimumWidth(len + 10);
                this->setMaximumWidth(len + 10);
            }
        }
    }
    int realwidth = this->minimumWidth() ? this->minimumWidth() : document()->size().width() + 2;
    this->resize(realwidth, document()->size().height());

    this->setMinimumWidth(0);
    qDebug() << "end";
    qDebug() << document()->size().width();
    qDebug() << document()->size().height();
    qDebug() << document()->size();
    qDebug() << this->size();
}

void MessageWidget::SetText(const QString &text)
{
    this->insertHtml(text);
    m_HasText = true;
    this->show();
    SetWidth(100);
}

void MessageWidget::SetWidth(int width)
{
    qDebug() << "start";
    qDebug() << document()->size().width();
    qDebug() << document()->size().height();
    qDebug() << document()->size();
    qDebug() << this->size();
//    if(m_HasText)
//    {
//        int realwidth = width;
//        int oldWidth = m_oldWidth;
//        this->setMinimumWidth(width);
//        this->setMaximumWidth(width);
//        this->setMinimumWidth(0);
//        qDebug() << "oldwidth : " << oldWidth;
//        if(width > oldWidth)
//        {
//            this->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
//            this->moveCursor(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
//            QRect start = this->cursorRect();
//            this->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
//            QRect end = this->cursorRect();
//            qDebug("Cursor Start Pos : %d %d %d %d\n", start.x(), start.y(), start.width(), start.height());
//            qDebug("Cursor End Pos : %d %d %d %d\n", end.x(), end.y(), end.width(), end.height());
//            if(end.x() - start.x() < document()->size().width() - 10 && onOneLine(start, end))
//            {
//                qDebug() << "in real";
//                realwidth = end.x() - start.x();
//                this->setMaximumWidth(realwidth + 20);
//                this->setMinimumWidth(realwidth + 20);
//                this->setMaximumHeight(end.y() + end.height());
//                this->setMinimumHeight(end.y() + end.height());
//                this->setMinimumHeight(0);
//                goto end;
//            }
//        }

//        this->setMaximumHeight(document()->size().height());
//        this->setMinimumHeight(document()->size().height());
//        this->setMinimumHeight(0);
//end:
//        m_oldWidth = realwidth + 20;
//    }
    emit sizeChange(width);
}

bool MessageWidget::onOneLine(const QRect &start, const QRect &end)
{
    int starty, endy;
    starty = start.y() > 0 ? start.y() : 0;
    endy = end.y() > 0 ? end.y() : 0;
    bool isOneLine = false;
    if(starty == endy && endy - starty < start.height())
        isOneLine = true;
    return isOneLine;
}
