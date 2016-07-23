#include "getfileinfo.h"
#include "ui_getfileinfo.h"

#include <QDebug>
#include <QMimeData>
#include <QFileInfo>
#include <QFileIconProvider>        //根据扩展名获取文件图标、类型
#include <QFileDialog>
#include <QDir>
#include <QDateTime>

GetFileInfo::GetFileInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GetFileInfo)
{
    ui->setupUi(this);

    setAcceptDrops(true);   //接受拖拽

    ui->groupBox->setEnabled(false);
}

GetFileInfo::~GetFileInfo()
{
    delete ui;
}

//获取文件信息
void GetFileInfo::getFileInfo(QString &fileName)
{
    QFileInfo info(fileName);

    qint64 size;
    if(info.isDir()){
        size = du(fileName);
    }else if(info.isFile()){
        size = info.size();
    }
    QString strSize = changeSizeFmt(size);
    ui->size_lineEdit->setText(strSize);

    QString createTime = info.created().toString("yyyy/MM/dd hh:mm:ss ddd");
    ui->create_lineEdit->setText(createTime);
    QString modifyTime = info.lastModified().toString("yyyy/MM/dd hh:mm:ss ddd");
    ui->modify_lineEdit->setText(modifyTime);
    QString readTime = info.lastRead().toString("yyyy/MM/dd hh:mm:ss ddd");
    ui->read_lineEdit->setText(readTime);

    //根据扩展名获取文件图标、类型
    QFileIconProvider provider;
    QString fileType = provider.type(info);
    ui->type_lineEdit->setText(fileType);
    QIcon icon = provider.icon(info);
    setWindowIcon(icon);

    //设置属性
    ui->dir_checkBox->setCheckState( info.isDir() ? Qt::Checked : Qt::Unchecked );
    ui->file_checkBox->setCheckState( info.isFile() ? Qt::Checked : Qt::Unchecked );
    ui->link_checkBox->setCheckState( info.isSymLink() ? Qt::Checked : Qt::Unchecked );
    ui->hide_checkBox->setCheckState( info.isHidden() ? Qt::Checked : Qt::Unchecked );
    ui->read_checkBox->setCheckState( info.isReadable() ? Qt::Checked : Qt::Unchecked );
    ui->write_checkBox->setCheckState( info.isWritable() ? Qt::Checked : Qt::Unchecked );
    ui->execute_checkBox->setCheckState( info.isExecutable() ? Qt::Checked : Qt::Unchecked );
}

//读取目录以及文件大小
qint64 GetFileInfo::du(QString path)
{
    QDir dir(path);
    qint64 size = 0;
    foreach (QFileInfo info, dir.entryInfoList(QDir::Files)) {
        //计算文件大小
        size += info.size();
    }
    foreach (QFileInfo subDir, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        //若存在子目录,则递归调用du函数
        size += du(path + QDir::separator() + subDir.fileName());   //补全文件名路径
    }
    return size;
}

//改变数据大小显示格式
QString GetFileInfo::changeSizeFmt(qint64 size)
{
    double curSize = size;
    QString unit = " B";
    if(curSize > 1024){
        curSize /= 1024;
        unit = " KB";
        if(curSize > 1024){
            curSize /= 1024;
            unit = " MB";
            if(curSize > 1024){
                curSize /= 1024;
                unit = " GB";
            }
        }
    }
    QString textSize = QString::number(curSize, 'f', 2);
    textSize.append(unit);

    //分离字符串
    QString tmpSize = QString::number(size);
    int count = tmpSize.count();
    for(int i = count - 3; i > 0; i -= 3){
        tmpSize.insert(i, ",");
    }
    tmpSize.append(tr(" 字节"));

    QString strSize;
    strSize = textSize + " (" + tmpSize + ")";

    return strSize;
}

void GetFileInfo::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "DragEnter!";
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void GetFileInfo::dropEvent(QDropEvent *event)
{
    qDebug() << "Drop!";
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    //往文本框中追加文件名
    foreach(QUrl url, urls) {
        m_fileName = url.toLocalFile();
        ui->file_lineEdit->setText(m_fileName);
        getFileInfo(m_fileName);
    }
}

void GetFileInfo::dragLeaveEvent(QDragLeaveEvent *event)
{
    qDebug() << "Drag Leave";
}

void GetFileInfo::on_btn_file_clicked()
{
    QString path = QDir::homePath();
    m_fileName = QFileDialog::getOpenFileName(this, tr("打开"), path, tr("All Files(*.*)"));
    if(!m_fileName.isEmpty()){
        ui->file_lineEdit->setText(m_fileName);

        //获取文件信息
        getFileInfo(m_fileName);
    }
}
