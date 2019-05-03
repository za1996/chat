#include "filewidgetitem.h"
#include "ui_filewidgetitem.h"

#include <QFontMetrics>

FileWidgetItem::FileWidgetItem(uint32_t FileNum, bool isLocalNum, bool isUpload, QWidget *parent = 0) :
    QWidget(parent),
    ui(new Ui::FileWidgetItem),
    m_FileNum(FileNum),
    m_isLocalNum(isLocalNum)
{
    ui->setupUi(this);
    if(isUpload)
    {
        ui->DownloadButton->hide();
    }
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
    QFontMetrics fontWidth(ui->ProgressBar->font());
    int width = fontWidth.width(Name);
    int max = ui->ProgressBar->width() - ui->DownloadButton->width() - ui->CancelButton->width();
    if(width >= max)  //当字符串宽度大于最大宽度时进行转换
    {
        Name = fontWidth.elidedText(Name, Qt::ElideRight, max);  //右部显示省略号
    }
    ui->ProgressBar->setFormat(Name);
    ui->ProgressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void FileWidgetItem::SetProBarMaxVal(int Size)
{
    ui->ProgressBar->setRange(0, Size);
}

void FileWidgetItem::SetRemoteFileNum(uint32_t FileNum)
{
    m_FileNum = FileNum;
    m_isLocalNum = false;
}

void FileWidgetItem::AddVal(int val)
{
    int old = ui->ProgressBar->value();
    ui->ProgressBar->setValue(old + val);
}



void FileWidgetItem::OnDownloadClick()
{
    emit DownloadFile();
}

void FileWidgetItem::OnCancelClick()
{
    ui->DownloadButton->hide();
    ui->CancelButton->hide();
    ui->MsgLabel->setText("取消");
    emit CancelFile();
}
