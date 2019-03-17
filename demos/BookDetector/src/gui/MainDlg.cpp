#include "MainDlg.h"
#include "ui_MainDlg.h"

#include <QFileDialog>

#include <opencv2/core/persistence.hpp>
#include <opencv2/highgui.hpp>

#include "component/PatternDetector.h"
#include "filter/QCvMatchResultFilter.h"
#include "util/QCvDataUtils.h"

#include <QDebug>

MainDlg::MainDlg(QWidget* parent) : QDialog(parent),
                                    ui(new Ui::MainDlg)
{
    ui->setupUi(this);
    m_featDetected = false;

    //init widgets

    m_labelImg = new QLabel(this);
    m_labelImg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_labelImg->setMinimumWidth(320);
    m_labelImg->setMinimumHeight(240);

    m_camView = new QCvCamView(this);
    m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_camView->setMinimumWidth(320);
    m_camView->setMinimumHeight(240);
    PatternDetector* detector = new PatternDetector(this);
    QCvMatchResultFilter* filter = new QCvMatchResultFilter(detector, this);
    m_camView->appendFilter(filter);

    m_layout = new QStackedLayout(ui->frame);
    m_layout->addWidget(m_labelImg);
    m_layout->addWidget(m_camView);
    m_layout->setCurrentWidget(m_labelImg);

    //btn load image
    connect(ui->btnLoad, &QPushButton::clicked, this, &MainDlg::onBtnLoad);
    connect(ui->btnLoadCamInt, &QPushButton::clicked, this, &MainDlg::onBtnLoadCamInt);

    //btn generate model
    connect(ui->btnGen, &QPushButton::clicked, this, [this, detector]() {
        if (!m_origImg.empty())
        {
            //detect feature
            cv::Mat retMat;
            detector->train(m_origImg, retMat);

            m_featDetected = true;
            ui->btnMatch->setEnabled(true);

            showImg(retMat);
        }
    });
    //btn match scene image
    connect(ui->btnMatch, &QPushButton::clicked, this, [this, detector]() {
        if (m_featDetected)
        {
            m_layout->setCurrentWidget(m_camView);
            m_camView->onStreamSwitch(true);
            resize(rect().width() - m_labelImg->size().width() + m_camView->currentFrame().size().width,
                   rect().height() - m_labelImg->size().height() + m_camView->currentFrame().size().height);
        }
    });
}

MainDlg::~MainDlg()
{
    delete ui;
}

void MainDlg::onBtnLoad()
{
    m_camView->onStreamSwitch(false);
    ui->btnGen->setDisabled(true);
    ui->btnMatch->setDisabled(true);

    cv::Mat img;
    loadImage(img);
    ui->btnGen->setEnabled(true);
    if (m_featDetected)
    {
        ui->btnMatch->setEnabled(true);
    }

    if (!img.empty())
    {
        m_origImg = img;
        m_layout->setCurrentWidget(m_labelImg);
        showImg(img);
        m_featDetected = false;
        ui->btnMatch->setEnabled(false);
    }
    else
    {
        m_camView->onStreamSwitch(true);
    }
}

void MainDlg::onBtnLoadCamInt()
{
    m_camView->onStreamSwitch(false);
    bool sampleLoaded = ui->btnGen->isEnabled();
    ui->btnGen->setDisabled(true);
    ui->btnMatch->setDisabled(true);

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Please Select a Calibration File",
                                                    ".", "Config Files (*.xml *.yml *.json)");
    if(!fileName.isEmpty())
    {
        m_camView->updateCalibrarion(fileName);
        m_camView->onStreamSwitch(true);
    }

    if(sampleLoaded)
    {
        ui->btnGen->setEnabled(true);
    }
    if (m_featDetected)
    {
        ui->btnMatch->setEnabled(true);
    }
}

void MainDlg::resizeEvent(QResizeEvent* event)
{
    if (m_labelImg->pixmap() != NULL && m_labelImg->isVisible())
    {
        QImage img = QCvDataUtils::cvMatToQImage(m_imgCache);
        m_labelImg->setPixmap(QPixmap::fromImage(img).scaled(m_labelImg->width(),
                                                             m_labelImg->height()));
    }
    QDialog::resizeEvent(event);
}

void MainDlg::loadImage(Mat& img)
{
    QString imgFileName = QFileDialog::getOpenFileName(this,
                                                       "Please Select a Muster Image",
                                                       ".", "Image Files (*.png *.jpg)");
    if (!imgFileName.isEmpty())
    {
        img = cv::imread(imgFileName.toStdString().c_str());
    }
}

void MainDlg::showImg(const cv::Mat& mat)
{
    QImage img = QCvDataUtils::cvMatToQImage(mat);
    m_imgCache = mat;
    m_labelImg->setPixmap(QPixmap::fromImage(img));
    resize(width() - m_labelImg->width() + img.width(),
           height() - m_labelImg->height() + img.height());
}
