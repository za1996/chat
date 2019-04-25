#include "chatwindowmessageitem.h"
#include "ui_chatwindowmessageitem.h"

ChatWindowMessageItem::ChatWindowMessageItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindowMessageItem)
{
    ui->setupUi(this);
}

ChatWindowMessageItem::~ChatWindowMessageItem()
{
    delete ui;
}
