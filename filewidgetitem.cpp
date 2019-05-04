#include "filewidgetitem.h"
#include "ui_filewidgetitem.h"

#include <QFontMetrics>

FileWidgetItem::FileWidgetItem(uint64_t FileNum, bool isUpload, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWidgetItem),
    m_FileNum(FileNum),
    m_FileSize(0),
    m_isUpload(isUpload)
{
    ui->setupUi(this);
    if(isUpload)
    {
        ui->DownloadButton->hide();
    }
    ui->ProgressBar->setValue(0);
    connect(ui->DownloadButton, SIGNAL(clicked(bool)), this, SLOT(OnDownloadClick()));
    connect(ui->CancelButton, SIGNAL(clicked(bool)), this, SLOT(OnCancelClick()));
}

FileWidgetItem::~FileWidgetItem()
{
    delete ui;
}

void FileWidgetItem::SetFileName(const QString &FileName)
{
    QString Name = FileName;
    QFontMetrics fontWidth(ui->FileNameLabel->font());
    int width = fontWidth.width(Name);
    int max = ui->ProgressBar->width() - ui->DownloadButton->width() - ui->CancelButton->width();
    if(width >= max)  //当字符串宽度大于最大宽度时进行转换
    {
        Name = fontWidth.elidedText(Name, Qt::ElideRight, max);  //右部显示省略号
    }
    ui->FileNameLabel->setText(Name);
}

void FileWidgetItem::SetProBarMaxVal(int Size)
{
    ui->ProgressBar->setRange(0, Size);
    m_FileSize = Size;
}


void FileWidgetItem::AddVal(int val)
{
    int old = ui->ProgressBar->value();
    int max = ui->ProgressBar->maximum();
    val = val + old;
    if(val > max)
        val = max;
    ui->ProgressBar->setValue(val);
}



void FileWidgetItem::OnDownloadClick()
{
    ui->DownloadButton->hide();
    emit DownloadFile(m_FileNum);
}

void FileWidgetItem::OnCancelClick()
{
    ui->DownloadButton->hide();
    ui->CancelButton->hide();
    ui->MsgLabel->setText("取消");
    ui->MsgLabel->show();
    emit CancelFile(m_FileNum);
}

void FileWidgetItem::TransferOver(bool Success)
{
    QString Text = "取消";
    if(Success)
    {
        Text = "成功";
    }
    ui->DownloadButton->hide();
    ui->CancelButton->hide();
    ui->MsgLabel->setText(Text);
    ui->MsgLabel->show();
}
void FileWidgetItem::SetHiddenDownloadButton()
{
    ui->DownloadButton->hide();
}
