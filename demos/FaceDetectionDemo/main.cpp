#include "FaceVideoDlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FaceVideoDlg w;
    w.show();

    return a.exec();
}
