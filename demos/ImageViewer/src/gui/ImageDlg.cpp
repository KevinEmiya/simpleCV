#include "ImageDlg.h"
#include "ui_ImageDlg.h"

#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringList>

#include "filter/QCvEdgeDetectFilter.h"
#include "filter/QCvMatFilterChain.h"
#include "util/QCvDataUtils.h"

ImageDlg::ImageDlg(QWidget* parent) : QDialog(parent),
                                      ui(new Ui::ImageDlg)
{
    ui->setupUi(this);
    // Prepare Image Selection Dialog
    m_imgSelectDlg = new QFileDialog(this);
    m_imgSelectDlg->setWindowTitle(tr("Please select an image"));
    m_imgSelectDlg->setDirectory(".");
    m_imgSelectDlg->setGeometry(0, 0, 640, 480);
    QStringList nameFilters;
    nameFilters << "*.jpg"
                << "*.png"
                << "*.bmp";
    m_imgSelectDlg->setFilter(QDir::Files | QDir::NoSymLinks | QDir::AllDirs);
    m_imgSelectDlg->setNameFilters(nameFilters);
    m_imgSelectDlg->hide();
    connect(ui->btnOpen, &QPushButton::clicked, this, &ImageDlg::onReadImage);
    m_filters = new QCvMatFilterChain(this);
    m_filters->append(new QCvEdgeDetectFilter());
    connect(ui->btnEdge, &QPushButton::clicked, this, &ImageDlg::onExtractEdge);
}

ImageDlg::~ImageDlg()
{
    delete ui;
}

void ImageDlg::onReadImage()
{
    m_imgSelectDlg->show();
    if (m_imgSelectDlg->exec() == QDialog::Accepted)
    {
        QString path = m_imgSelectDlg->selectedFiles()[0];
        ui->imgLabel->setText("");
        m_imgMat = cv::imread(path.toStdString().c_str());
        this->showImage(m_imgMat);
    }
    m_imgSelectDlg->hide();
    ui->btnEdge->setEnabled(true);
}

void ImageDlg::onExtractEdge()
{
    cv::Mat edgeImg = m_filters->execFilter(m_imgMat);
    showImage(edgeImg);
}

void ImageDlg::showImage(const cv::Mat& mat)
{
    QImage img = QCvDataUtils::cvMatToQImage(mat);
    ui->imgLabel->setPixmap(QPixmap::fromImage(img));
    resize(width() - ui->imgLabel->width() + img.width(),
           height() - ui->imgLabel->height() + img.height());
}
