#include "getfileinfo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GetFileInfo w;
    w.show();

    return a.exec();
}
