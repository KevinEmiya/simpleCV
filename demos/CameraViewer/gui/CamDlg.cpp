#include "CamDlg.h"
#include "ui_CamDlg.h"

#include "widget/QCvCamView.h"

#include <QStackedLayout>

CamDlg::CamDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CamDlg)
{
    ui->setupUi(this);

    QStackedLayout* layout = new QStackedLayout(ui->frame);
    m_camView = new QCvCamView(ui->frame);
    m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_camView);
    connect(ui->btnOpenCam, SIGNAL(clicked(bool)), m_camView, SLOT(onStreamSwitch(bool)));
    connect(ui->btnOpenCam, SIGNAL(clicked(bool)), this, SLOT(onBtnOpenClicked(bool)));
    connect(m_camView, SIGNAL(camOpenError()), this, SLOT(onCamOpenError()));
    connect(m_camView, SIGNAL(emptyFrameError()), this, SLOT(onEmptyFrameError()));

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, SIGNAL(timeout()), this, SLOT(onStatusTimer()));

    QRegExp fpsReg("^100|[1-9]{0,1}[0-9]$");
    ui->fpsEdit->setValidator(new QRegExpValidator(fpsReg, this));
    connect(ui->fpsEdit, SIGNAL(textChanged(QString)), this, SLOT(onFpsChanged(QString)));
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
}

void CamDlg::onCamOpenError()
{
    ui->statusLabel->setText("Open Camera Failed!");
    m_statusTimer->start(5000);
    ui->btnOpenCam->click();
}

void CamDlg::onEmptyFrameError()
{
    ui->statusLabel->setText("Frame Capture Failed!");
    m_statusTimer->start(5000);
    ui->btnOpenCam->click();
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
