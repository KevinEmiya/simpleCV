#ifndef IMAGEDLG_H
#define IMAGEDLG_H

#include <QDialog>

class QString;
class QCvMatFilterChain;
class QFileDialog;

#include "opencv2/opencv.hpp"

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
    void showImage(const cv::Mat& mat);

  private slots:
    void onReadImage();
    void onExtractEdge();

  private:
    Ui::ImageDlg* ui;
    QCvMatFilterChain* m_filters;
    cv::Mat m_imgMat;

    QFileDialog* m_imgSelectDlg;
};

#endif // IMAGEDLG_H
