#include <QApplication>

#include "gui/CamDlg.h"

#include "opencv2/highgui.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CamDlg dlg;
    dlg.show();

    return a.exec();
}
