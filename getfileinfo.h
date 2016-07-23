#ifndef GETFILEINFO_H
#define GETFILEINFO_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Ui {
class GetFileInfo;
}

class GetFileInfo : public QWidget
{
    Q_OBJECT

public:
    explicit GetFileInfo(QWidget *parent = 0);
    ~GetFileInfo();

    void getFileInfo(QString &fileName);
    qint64 du(QString path);
    QString changeSizeFmt(qint64 size);

protected:
    void dragEnterEvent(QDragEnterEvent *event);

    void dropEvent(QDropEvent *event);

    void dragLeaveEvent(QDragLeaveEvent *event);

private slots:
    void on_btn_file_clicked();

private:
    Ui::GetFileInfo *ui;

    QString m_fileName;     //文件名
};

#endif // GETFILEINFO_H
