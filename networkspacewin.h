#ifndef NETWORKSPACEWIN_H
#define NETWORKSPACEWIN_H

#include "titlebar.h"
#include "message.h"

#include <QWidget>
#include <QFileInfo>
#include <QListWidgetItem>

namespace Ui {
class NetworkSpaceWin;
}

class NetworkSpaceWin : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkSpaceWin(QWidget *parent = 0);
    ~NetworkSpaceWin();

private:
    Ui::NetworkSpaceWin *ui;
    TitleBar *m_TitleBar;
    QHash<uint64_t, QFileInfo> m_ReadySendFile;
    QHash<uint64_t, QFileInfo> m_ReadyDownloadFile;
    QHash<uint64_t, QListWidgetItem*> m_FileTransferItemMap;
    QAction *m_RefreshFiles;
    static uint64_t LocalFileNum;

private:
    void AddFileTransferItem(uint64_t FileNum, bool isUpload, int FileSize, const QString& FileName);

private slots:
    void ShowTransferInfo();
    void ShowUserNetworkSpace();
    void SendFileToServer();
    void RefreshFileInfo();
    void DownloadFileByServer();
    void DelSpaceFile();
    void ForceCloseRemoteFile(uint64_t);

public slots:
    void HandleReqSendMessage(MessagePtr);
    void HandleSendOrRecvSignal(uint64_t, uint32_t, int, int);
    void HandleSendFileEnd(uint64_t);
    void ShowFilesInfo(MessagePtr);
    void HandleReqRecvMessage(MessagePtr);

};

#endif // NETWORKSPACEWIN_H
