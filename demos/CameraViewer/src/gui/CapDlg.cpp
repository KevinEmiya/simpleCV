#include "CapDlg.h"
#include "ui_CapDlg.h"

#include "util/QCvDataUtils.h"

#include <QDateTime>
#include <QFileDialog>

CapDlg::CapDlg(QWidget* parent) : QDialog(parent),
                                  ui(new Ui::CapDlg)
{
    ui->setupUi(this);
    connect(ui->btnSave, &QPushButton::clicked, this, &CapDlg::onBtnSave);
    connect(ui->btnAbort, &QPushButton::clicked, this, &CapDlg::onBtnAbort);
}

CapDlg::~CapDlg()
{
    delete ui;
}

void CapDlg::setFrame(const cv::Mat& frame)
{
    QImage img = QCvDataUtils::cvMatToQImage(frame);
    ui->labelImg->setPixmap(QPixmap::fromImage(img).scaled(ui->labelImg->width(),
                                                           ui->labelImg->height()));
    m_currentFrame = frame;
}

void CapDlg::onBtnSave()
{
    QString timeStample = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save Current Frame"),
                                                        QString("./%1.jpg").arg(timeStample),
                                                        tr("Images(*.png *.jpg);;All Files (*)"));
    if (!saveFileName.isEmpty())
    {
        QImage img = QCvDataUtils::cvMatToQImage(m_currentFrame);
        img.save(saveFileName);
        hide();
        emit closed();
    }
}

void CapDlg::onBtnAbort()
{
    hide();
    emit closed();
}
