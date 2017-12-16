#include "ImageDlg.h"
#include "ui_ImageDlg.h"

#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringList>

#include "util/QCvDataUtils.h"

ImageDlg::ImageDlg(QWidget* parent) : QDialog(parent),
                                      ui(new Ui::ImageDlg)
{
    ui->setupUi(this);
    // Prepare Image Selection Dialog
    QFileDialog* imgSelectDlg = new QFileDialog(this);
    imgSelectDlg->setWindowTitle(tr("Please select an image"));
    imgSelectDlg->setDirectory(".");
    imgSelectDlg->setGeometry(0, 0, 640, 480);
    QStringList nameFilters;
    nameFilters << "*.jpg"
                << "*.png"
                << "*.bmp";
    imgSelectDlg->setFilter(QDir::Files | QDir::NoSymLinks);
    imgSelectDlg->setNameFilters(nameFilters);
    imgSelectDlg->hide();
    connect(ui->btnOpen, &QPushButton::clicked, this, [this, imgSelectDlg]() {
        imgSelectDlg->show();
        if (imgSelectDlg->exec() == QDialog::Accepted)
        {
            QString path = imgSelectDlg->selectedFiles()[0];
            this->showImage(path);
        }
        imgSelectDlg->hide();
    });
}

ImageDlg::~ImageDlg()
{
    delete ui;
}

void ImageDlg::showImage(QString imgPath)
{
    ui->imgLabel->setText("");
    cv::Mat imgMat = cv::imread(imgPath.toStdString().c_str());
    QImage img = QCvDataUtils::cvMatToQImage(imgMat);
    ui->imgLabel->setPixmap(QPixmap::fromImage(img));
    resize(width() - ui->imgLabel->width() + img.width(),
           height() - ui->imgLabel->height() + img.height());
}
