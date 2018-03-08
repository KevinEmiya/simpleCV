#include "MainDlg.h"
#include "ui_MainDlg.h"

#include <QDebug>
#include <QStackedLayout>
#include <QFileDialog>
#include <QDateTime>

#include "src/component/CamCalibrator.h"
#include "src/filter/QCvEdgePtFilter.h"

MainDlg::MainDlg(QWidget* parent) : QDialog(parent),
                                    ui(new Ui::MainDlg)
{
    ui->setupUi(this);

    //init camera view
    m_camView = new QCvCamView(this);
    QStackedLayout* layout = new QStackedLayout(ui->camFrame);
    m_camView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_camView);

    m_camView->onStreamSwitch(true);

    //init edge point filter
    QString edgePtFilterName = "EdgePt";
    QCvEdgePtFilter* filter = new QCvEdgePtFilter(edgePtFilterName);
    int boardCol = ui->editBoardCol->text().toInt();
    int boardRow = ui->editBoardRow->text().toInt();
    filter->setColCnt(boardCol);
    filter->setRowCnt(boardRow);
    filter->setEnabled(true);

    m_camView->appendFilter(filter);
    connect(ui->editBoardCol, &QLineEdit::textChanged, this, [this, filter](QString newCol) {
        filter->setColCnt(newCol.toInt());
    });
    connect(ui->editBoardRow, &QLineEdit::textChanged, this, [this, filter](QString newRow) {
        filter->setRowCnt(newRow.toInt());
    });
    connect(ui->btnCapture, &QPushButton::clicked, this, [this, filter](){
       ui->editBoardCol->setDisabled(true);
       ui->editBoardRow->setDisabled(true);
       ui->editSquareHeight->setDisabled(true);
       ui->editSquareWidth->setDisabled(true);
       filter->onCapture();
    });

    //init calibrator
    CamCalibrator* calibrator = new CamCalibrator(this);
    calibrator->setSquareWidth(ui->editSquareWidth->text().toFloat());
    calibrator->setSquareHeight(ui->editSquareHeight->text().toFloat());
    connect(filter, &QCvEdgePtFilter::imgPtsCaptured, calibrator, &CamCalibrator::onImgPts);
    connect(ui->editSquareHeight, &QLineEdit::textChanged, this, [this, calibrator](QString newH) {
        calibrator->setSquareHeight(newH.toFloat());
    });
    connect(ui->editSquareWidth, &QLineEdit::textChanged, this, [this, calibrator](QString newW) {
        calibrator->setSquareWidth(newW.toFloat());
    });
    connect(ui->btnReset, &QPushButton::clicked, [this, calibrator](){
        ui->editParams->clear();
        calibrator->reset();
        ui->editBoardCol->setEnabled(true);
        ui->editBoardRow->setEnabled(true);
        ui->editSquareHeight->setEnabled(true);
        ui->editSquareWidth->setEnabled(true);
    });
    connect(calibrator, &CamCalibrator::calibrated, this, [this](QString calibResult) {
        ui->editParams->clear();
        ui->editParams->setPlainText(calibResult);
    });
    connect(ui->btnSave, &QPushButton::clicked, this, [this, calibrator](){
        m_camView->onStreamSwitch(false);
        QString timeStample = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
        QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save Calibration Results"),
                                                            QString("./%1.json").arg(timeStample),
                                                            tr("Config Files(*.json *.xml *.yml);;All Files(*)"));
        if (!saveFileName.isEmpty())
        {
            calibrator->saveResults(saveFileName);
        }
        m_camView->onStreamSwitch(true);
    });
}

MainDlg::~MainDlg()
{
    m_camView->onStreamSwitch(false);
    delete ui;
}
