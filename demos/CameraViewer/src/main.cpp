#include <QApplication>

#include "gui/CamDlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CamDlg dlg;
    dlg.show();

    return a.exec();
}
