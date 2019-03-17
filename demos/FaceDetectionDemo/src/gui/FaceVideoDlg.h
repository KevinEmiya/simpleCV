#ifndef FACEVIDEODLG_H
#define FACEVIDEODLG_H

#include <QDialog>
#include <QStatusBar>

namespace Ui {
class FaceVideoDlg;
}

class QCvCamView;
class QCvFaceDetectFilter;

class FaceVideoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FaceVideoDlg(QWidget *parent = 0);
    ~FaceVideoDlg();

private slots:
    void onBtnLoad();
    void onBtnDetect(bool clicked);

private:
    Ui::FaceVideoDlg *ui;
    QStatusBar* m_statusBar;
    QCvCamView* m_camView;
    QCvFaceDetectFilter* m_faceFilter;
};

#endif // FACEVIDEODLG_H
