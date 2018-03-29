#include "CamDlg.h"
#include "ui_CamDlg.h"

#include "widget/QCvCamView.h"
#include "util/QCvDataUtils.h"
#include "CapDlg.h"

#include <QStackedLayout>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

CamDlg::CamDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CamDlg)
{
    ui->setupUi(this);

    QStackedLayout* layout = new QStackedLayout(ui->frame);
    m_camView = new QCvCamView(ui->frame);
    m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_camView);
    connect(ui->btnOpenCam, &QPushButton::clicked, m_camView, &QCvCamView::onStreamSwitch);
    connect(ui->btnOpenCam, &QPushButton::clicked, this, &CamDlg::onBtnOpenClicked);
    connect(ui->btnCap, &QPushButton::clicked, this, &CamDlg::onCapFrame);
    connect(m_camView, &QCvCamView::camOpenError, this, &CamDlg::onCamOpenError);
    connect(m_camView, &QCvCamView::emptyFrameError, this, &CamDlg::onEmptyFrameError);

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, SIGNAL(timeout()), this, SLOT(onStatusTimer()));

    QRegExp fpsReg("^100|[1-9]{0,1}[0-9]$");
    ui->fpsEdit->setValidator(new QRegExpValidator(fpsReg, this));
    connect(ui->fpsEdit, SIGNAL(textChanged(QString)), this, SLOT(onFpsChanged(QString)));

    m_capDlg = new CapDlg(this);
    m_capDlg->setModal(true);
    m_capDlg->setWindowFlags(m_capDlg->windowFlags()|Qt::FramelessWindowHint);
    m_capDlg->hide();
    connect(m_capDlg, &CapDlg::closed, this, &CamDlg::onCapDlgClosed);
}

CamDlg::~CamDlg()
{
    delete ui;
}

void CamDlg::onBtnOpenClicked(bool clicked)
{
    ui->statusLabel->clear();
    if(clicked) // open
    {
        ui->btnOpenCam->setText("Close Camera");
    }
    else
    {
        ui->btnOpenCam->setText("Open Camera");
    }
    ui->btnOpenCam->setChecked(clicked);
    ui->btnCap->setEnabled(clicked);
}

void CamDlg::onCamOpenError()
{
    ui->statusLabel->setText("Open Camera Failed!");
    m_statusTimer->start(5000);
    ui->btnOpenCam->setText("Open Camera");
    ui->btnOpenCam->setChecked(false);
    ui->btnCap->setEnabled(false);
}

void CamDlg::onEmptyFrameError()
{
    ui->statusLabel->setText("Frame Capture Failed!");
    m_statusTimer->start(5000);
}

void CamDlg::onStatusTimer()
{
    m_statusTimer->stop();
    ui->statusLabel->clear();
}

void CamDlg::onFpsChanged(QString fpsStr)
{
    if(!fpsStr.isEmpty())
    {
        m_camView->onFpsChanged(fpsStr.toInt());
    }
}

void CamDlg::onCapFrame()
{
    cv::Mat currentFrame = m_camView->currentFrame();
    if(!currentFrame.empty())
    {
        m_camView->onRenderSwitch(false);
        m_capDlg->show();
        m_capDlg->setFrame(currentFrame);
    }
    else
    {
        ui->statusLabel->setText("Current frame is empty! Please try later...");
        m_statusTimer->start(5000);
    }
}

void CamDlg::onCapDlgClosed()
{
    m_camView->onRenderSwitch(true);
    ui->btnCap->setChecked(false);
}
