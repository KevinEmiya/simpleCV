#ifndef CAPDLG_H
#define CAPDLG_H

#include <QDialog>
#include <opencv2/core.hpp>

namespace Ui {
class CapDlg;
}

class CapDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CapDlg(QWidget *parent = 0);
    ~CapDlg();

signals:
    void closed();

public:
    void setFrame(const cv::Mat& frame);

private slots:
    void onBtnSave();
    void onBtnAbort();

private:
    Ui::CapDlg *ui;
    cv::Mat m_currentFrame;
};

#endif // CAPDLG_H
