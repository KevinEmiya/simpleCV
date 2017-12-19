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
    QCvEdgeDetectFilter* cannyFilter = new QCvEdgeDetectFilter("canny");
    m_filters->append(cannyFilter);
    connect(ui->btnEdge, &QPushButton::clicked, this, &ImageDlg::onExtractEdge);

    connect(ui->lowThresSlider, &QSlider::sliderMoved, this, [this, cannyFilter]() {
        cannyFilter->setThresholds(ui->lowThresSlider->value(), ui->highThresSlider->value());
        if (m_extractingEdge)
        {
            m_edgeMat = m_filters->execFilter(m_imgMat);
            showImage(m_edgeMat);
        }
    });
    connect(ui->highThresSlider, &QSlider::sliderMoved, this, [this, cannyFilter]() {
        cannyFilter->setThresholds(ui->lowThresSlider->value(), ui->highThresSlider->value());
        if (m_extractingEdge)
        {
            m_edgeMat = m_filters->execFilter(m_imgMat);
            showImage(m_edgeMat);
        }
    });
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
        this->showImage(m_imgMat, true);
    }
    m_imgSelectDlg->hide();
    ui->btnEdge->setEnabled(true);
}

void ImageDlg::resizeEvent(QResizeEvent* event)
{
    if (ui->imgLabel->pixmap() != NULL)
    {
        cv::Mat matCache = m_extractingEdge ? m_edgeMat : m_imgMat;
        QImage img = QCvDataUtils::cvMatToQImage(matCache);
        ui->imgLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->imgLabel->width(),
                                                               ui->imgLabel->height()));
    }
    QDialog::resizeEvent(event);
}

void ImageDlg::onExtractEdge(bool clicked)
{
    m_extractingEdge = clicked;
    showImage(clicked ? m_filters->execFilter(m_imgMat) : m_imgMat);
}

void ImageDlg::showImage(const cv::Mat& mat, bool showOrigSize)
{
    QImage img = QCvDataUtils::cvMatToQImage(mat);
    if (showOrigSize)
    {
        ui->imgLabel->setPixmap(QPixmap::fromImage(img));
        resize(width() - ui->imgLabel->width() + img.width(),
               height() - ui->imgLabel->height() + img.height());
    }
    else
    {
        ui->imgLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->imgLabel->width(),
                                                               ui->imgLabel->height()));
    }
}
