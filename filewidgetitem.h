#ifndef FILEWIDGETITEM_H
#define FILEWIDGETITEM_H

#include <QWidget>

namespace Ui {
class FileWidgetItem;
}

class FileWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit FileWidgetItem(uint64_t FileNum, bool isUpload, QWidget *parent = 0);
    ~FileWidgetItem();

    void SetFileName(const QString& FileName);
    void SetProBarMaxVal(int Size);
    void AddVal(int val);
    void TransferOver(bool Success);
    int GetFileSize() { return m_FileSize; }
    bool IsUpload() { return m_isUpload; }
    void SetHiddenDownloadButton();

signals:
    void DownloadFile(uint64_t);
    void CancelFile(uint64_t);

private slots:
    void OnDownloadClick();
    void OnCancelClick();

private:
    Ui::FileWidgetItem *ui;
    uint64_t m_FileNum;
    int m_FileSize;
    bool m_isUpload;
};

#endif // FILEWIDGETITEM_H
