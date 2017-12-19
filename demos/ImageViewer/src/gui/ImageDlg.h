#ifndef IMAGEDLG_H
#define IMAGEDLG_H

#include <QDialog>

class QString;
class QCvMatFilterChain;
class QFileDialog;

#include "opencv2/opencv.hpp"
#include <QResizeEvent>

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

  protected:
    void resizeEvent(QResizeEvent* event);

  private:
    void showImage(const cv::Mat& mat, bool showOrigSize = false);

  private slots:
    void onReadImage();
    void onExtractEdge(bool clicked);

  private:
    Ui::ImageDlg* ui;
    cv::Mat m_imgMat;
    QFileDialog* m_imgSelectDlg;

    cv::Mat m_edgeMat;
    QCvMatFilterChain* m_filters;
    bool m_extractingEdge;
};

#endif // IMAGEDLG_H
