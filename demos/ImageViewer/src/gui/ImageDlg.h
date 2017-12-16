#ifndef IMAGEDLG_H
#define IMAGEDLG_H

#include <QDialog>

class QString;

namespace Ui
{
class ImageDlg;
}

class ImageDlg : public QDialog
{
    Q_OBJECT

  public:
    explicit ImageDlg(QWidget* parent = 0);
    ~ImageDlg();

private:
    void showImage(QString imgPath);

  private:
    Ui::ImageDlg* ui;
};

#endif // IMAGEDLG_H
