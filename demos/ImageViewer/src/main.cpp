#include "gui/ImageDlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageDlg w;
    w.show();

    return a.exec();
}
