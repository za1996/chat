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
    void TransferOver();
    int GetFileSize() { return FileSize; }

signals:
    void DownloadFile(uint64_t);
    void CancelFile(uint64_t);

private slots:
    void OnDownloadClick();
    void OnCancelClick();

private:
    Ui::FileWidgetItem *ui;
    uint64_t m_FileNum;
    int FileSize;
};

#endif // FILEWIDGETITEM_H
