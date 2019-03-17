#ifndef CAMDLG_H
#define CAMDLG_H

#include <QDialog>
#include <QTimer>

class QCvCamView;
class CapDlg;

namespace Ui {
class CamDlg;
}

class CamDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CamDlg(QWidget *parent = 0);
    ~CamDlg();

private slots:
    void onBtnOpenClicked(bool clicked);
    void onCamOpenError();
    void onEmptyFrameError();
    void onStatusTimer();
    void onFpsChanged(QString fpsStr);
    void onCapFrame();
    void onCapDlgClosed();

private:
    Ui::CamDlg *ui;
    QCvCamView* m_camView;
    QTimer* m_statusTimer;
    CapDlg* m_capDlg;
};

#endif // CAMDLG_H
