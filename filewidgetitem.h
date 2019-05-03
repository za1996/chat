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
    explicit FileWidgetItem(bool isUpload, QWidget *parent = 0);
    ~FileWidgetItem();

    void SetFileName(const QString& FileName);
    void SetProBarMaxVal(int Size);
    void AddVal(int val);

signals:
    void DownloadFile();
    void CancelFile();

private slots:
    void OnDownloadClick();
    void OnCancelClick();

private:
    Ui::FileWidgetItem *ui;
    uint32_t m_FileNum;
    bool isLocalNum;
};

#endif // FILEWIDGETITEM_H
