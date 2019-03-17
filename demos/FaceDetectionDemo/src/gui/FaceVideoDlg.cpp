#include "FaceVideoDlg.h"
#include "ui_FaceVideoDlg.h"

#include "filter/QCvFaceDetectFilter.h"
#include "widget/QCvCamView.h"

#include <QFileDialog>
#include <QStackedLayout>

FaceVideoDlg::FaceVideoDlg(QWidget* parent) : QDialog(parent),
                                              ui(new Ui::FaceVideoDlg)
{
    ui->setupUi(this);

    m_statusBar = new QStatusBar(this);
    ui->verticalLayout->addWidget(m_statusBar);

    QStackedLayout* layout = new QStackedLayout(ui->frame);
    m_camView = new QCvCamView(ui->frame);
    m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_camView);

    m_faceFilter = new QCvFaceDetectFilter("face");
    m_camView->appendFilter(m_faceFilter);

    connect(m_camView, &QCvCamView::camOpenError, this, [this]() {
        m_statusBar->showMessage("Failed to open camera!", 5000);
    });

    connect(ui->btnLoad, &QPushButton::clicked, this, &FaceVideoDlg::onBtnLoad);
    connect(ui->btnDetect, &QPushButton::clicked, this, &FaceVideoDlg::onBtnDetect);
}

FaceVideoDlg::~FaceVideoDlg()
{
    delete ui;
}

void FaceVideoDlg::onBtnDetect(bool clicked)
{
    m_camView->onStreamSwitch(clicked);
}

void FaceVideoDlg::onBtnLoad()
{
    onBtnDetect(false);
    ui->btnDetect->setEnabled(false);

    QString dataDir = QDir::currentPath() + "/../../../../sdk/opencv_release/share/OpenCV/haarcascades";
    if (!QDir(dataDir).exists())
    {
        dataDir = ".";
    }
    QString classifierName = QFileDialog::getOpenFileName(this,
                                                          "Please Select a Cascade Classifier",
                                                          dataDir,
                                                          "XML Files (*.xml)");
    if (!classifierName.isEmpty())
    {
        bool isValid = m_faceFilter->load(classifierName);
        if (isValid)
        {
            ui->btnDetect->setEnabled(true);
            ui->btnDetect->setChecked(false);
        }
        else
        {
            m_statusBar->showMessage("Invalid classifier!", 5000);
        }
    }
    else
    {
        if (m_faceFilter->isClassifierValid())
        {
            ui->btnDetect->setEnabled(true);
            ui->btnDetect->setChecked(false);
            ui->btnDetect->click();
        }
    }
}
