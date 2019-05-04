#include "networkspacewin.h"
#include "ui_networkspacewin.h"
#include "global.h"
#include "filewidgetitem.h"
#include "mainwindow.h"
#include <QFileDialog>

NetworkSpaceWin::NetworkSpaceWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkSpaceWin)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    this->setStyleSheet("QWidget#MainBoard{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #169ada, stop: 0.5 #3ec2ff, stop:1 #66eaff)};");
    this->setAttribute(Qt::WA_TranslucentBackground);
    QWidget::setAttribute(Qt::WA_DeleteOnClose);

    m_TitleBar = new TitleBar(this);
    m_TitleBar->move(10, 10);
    m_TitleBar->setBackgroundColor(0, 0, 0, true);
    m_TitleBar->setTitleWidth(ui->MainBoard->width());
    m_TitleBar->setWindowBorderWidth(10 * 2);
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_TitleBar->setMargins(5, 0, 0, 0);
    m_TitleBar->loadStyleSheet("D:/titlebarstyle.css");
    m_TitleBar->setTitleContent("网络空间", 15);
    ui->FileListWidget->raise();
    connect(ui->TransferInfoButton, SIGNAL(clicked(bool)), this, SLOT(ShowTransferInfo()));
    connect(ui->SpaceButton, SIGNAL(clicked(bool)), this, SLOT(ShowUserNetworkSpace()));
    connect(ui->UploadButton, SIGNAL(clicked(bool)), this, SLOT(SendFileToServer()));
    connect(m_TitleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(close()));

}

NetworkSpaceWin::~NetworkSpaceWin()
{
    delete ui;
}

void NetworkSpaceWin::ShowTransferInfo()
{
    ui->TransferInfoListWidget->raise();
}

void NetworkSpaceWin::ShowUserNetworkSpace()
{
    ui->FileListWidget->raise();
}

void NetworkSpaceWin::SendFileToServer()
{
    static uint64_t LocalFileNum = 0;
    QString FullPath = QFileDialog::getOpenFileName(this, tr("选择文件"), "C:");
    if(!FullPath.isEmpty())
    {
        QFileInfo info(FullPath);
        m_ReadySendFile.insert(LocalFileNum, info);
        auto m = CreateReadySendUserFileToServerMsg(m_ThisIsId, 0, LocalFileNum, info.fileName().toStdString(), UPLOADUSERFILE, m_ThisIsId);
        SendtoRemote(s, m);
        qDebug() << "send readysendfile";
        LocalFileNum++;
    }
}

void NetworkSpaceWin::HandleReqSendMessage(MessagePtr m)
{
    json info = json::parse((char *)m->data());
    uint64_t LocalNum = info["ClientFileNum"].get<json::number_unsigned_t>();
    uint64_t FileNum = info["FileNum"].get<json::number_unsigned_t>();
    auto it = m_ReadySendFile.find(LocalNum);
    if(it != m_ReadySendFile.end())
    {
        AddFileTransferItem(FileNum, true, it->size(), it->fileName());
        m_MainWin->AddSendFile(FileNum, m_ThisIsId, UPLOADUSERFILE, it->absoluteFilePath().toStdString());
        m_ReadySendFile.erase(it);
    }
}

void NetworkSpaceWin::AddFileTransferItem(uint64_t FileNum, bool isUpload, int FileSize, const QString& FileName)
{
    FileWidgetItem *fitem = new FileWidgetItem(FileNum, isUpload);
    QListWidgetItem *item = new QListWidgetItem(ui->TransferInfoListWidget);
    item->setSizeHint(QSize(0, fitem->height()));
    fitem->SetFileName(FileName);
    fitem->SetProBarMaxVal(FileSize);
    if(!isUpload)
    {
        fitem->SetHiddenDownloadButton();
    }
    ui->TransferInfoListWidget->addItem(item);
    ui->TransferInfoListWidget->setItemWidget(item, fitem);
    m_FileTransferItemMap.insert(FileNum, item);
//    connect(fitem, SIGNAL(CancelFile(uint64_t)), this, SLOT(ForceCloseFile(uint64_t)));
    qDebug() << __FUNCTION__ << " FileSize: " << FileSize;
    fitem->show();
}

void NetworkSpaceWin::HandleSendOrRecvSignal(uint64_t FileNum, uint32_t Id, int FileCode, int Size)
{
    qDebug() << __FUNCTION__;
    qDebug() << "id : " << Id << " FileCode : " << (FileCode == FILEDATATRANSFER) << " File Size : " << Size;
    if(Id == m_ThisIsId && (FileCode == UPLOADUSERFILE))
    {
        auto it = m_FileTransferItemMap.find(FileNum);
        if(it != m_FileTransferItemMap.end())
        {
            FileWidgetItem *item = dynamic_cast<FileWidgetItem *>(ui->TransferInfoListWidget->itemWidget(*it));
            assert(item != nullptr);
            if(item)
            {
                item->AddVal(Size);
            }
        }
    }
}

void NetworkSpaceWin::HandleSendFileEnd(uint64_t FileNum)
{
    auto it = m_FileTransferItemMap.find(FileNum);
    if(it != m_FileTransferItemMap.end())
    {
        FileWidgetItem * item = dynamic_cast<FileWidgetItem *>(ui->TransferInfoListWidget->itemWidget(*it));
        item->TransferOver(true);
    }
}
