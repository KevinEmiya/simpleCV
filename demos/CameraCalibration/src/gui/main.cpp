#include "MainDlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDlg w;
    w.show();

    return a.exec();
}
