#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPaintEvent>
#include <QHash>

#include "titlebar.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum {SPACESIZE = 10, PROFILESIZE = 80};
    QTreeWidget *m_GroupTree;
    TitleBar *m_TitleBar;
    QLabel *m_Profile;
    QLabel *m_UserName;
    QLabel *m_UserDesc;
    QLineEdit *m_SerachLineEdit;
    QWidget *m_MainBoard;
    QAction *m_addGroup;
    QHash<QString, QTreeWidgetItem *> *m_GroupMap;

    void AddGroupItem(QTreeWidgetItem *parent);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void paintEvent(QPaintEvent *event);
signals:

private slots:
    void onGroupItemClick(QTreeWidgetItem *pitem, int col);
    void onGroupItemDoubleClick(QTreeWidgetItem *pitem, int col);
    void UpdatePos(void);
    void AddGroup();
    void DelGroupItem(void *item);
    void ChangeGroupItem(void *item);
    void ChangeGroupItemName(void *item);
};

#endif // MAINWINDOW_H
