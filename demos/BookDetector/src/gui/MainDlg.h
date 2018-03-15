#ifndef MANDLG_H
#define MANDLG_H

#include <QDialog>
#include <QLabel>
#include <QStackedLayout>
#include <opencv2/highgui.hpp>

#include "widget/QCvCamView.h"

namespace Ui
{
class MainDlg;
}

class MainDlg : public QDialog
{
    Q_OBJECT

  public:
    explicit MainDlg(QWidget* parent = 0);
    ~MainDlg();

  protected:
    virtual void resizeEvent(QResizeEvent* event);

  private:
    void loadImage(cv::Mat& img);
    void showImg(const cv::Mat& mat);

  private slots:
    void onBtnLoad();
    void onBtnLoadCamInt();

  private:
    Ui::MainDlg* ui;
    QLabel* m_labelImg;
    QCvCamView* m_camView;
    QStackedLayout* m_layout;

    cv::Mat m_origImg;
    cv::Mat m_imgCache;
    bool m_featDetected;
};

#endif // MANDLG_H
