#include "QCvCamView.h"

#include <QDebug>
#include <QImage>
#include <QPainter>

QCvCamView::QCvCamView(QWidget* parent) : QWidget(parent)
{
    m_cap = new cv::VideoCapture();
    m_fps = 60;
    m_updateTimer = new QTimer(this);
    m_updateTimer->start(1000 / m_fps);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));

    m_camera = new QCvCamera(this);
}

QCvCamView::~QCvCamView()
{
    if (m_cap != NULL)
    {
        delete m_cap;
        m_cap = NULL;
    }
}

void QCvCamView::onStreamSwitch(bool open)
{
    if (open && !m_cap->isOpened()) //open the camera
    {
        bool ret = m_cap->open(0);
        if (!ret || !m_cap->isOpened())
        {
            emit camOpenError();
            return;
        }
        update();
    }
    else if (!open && m_cap->isOpened()) //close the camera
    {
        m_cap->release();
        update();
    }
}

void QCvCamView::onRenderSwitch(bool open)
{
    m_isRendering = open;
}

void QCvCamView::appendFilter(QCvMatFilter* filter)
{
    filter->setParent(this);
    filter->setCamera(m_camera);
    m_filters.append(filter);
}

void QCvCamView::setFilterEnabled(QString name, bool enabled)
{
    foreach (QCvMatFilter* filter, m_filters)
    {
        if (filter->name() == name)
        {
            filter->setEnabled(enabled);
        }
    }
}

bool QCvCamView::updateCalibrarion(QString fileName)
{
    if (!fileName.isEmpty())
    {
        return m_camera->loadCalibrationData(fileName);
    }
    else
    {
        return false;
    }
}

cv::Mat QCvCamView::currentFrame()
{
    return m_frame;
}

void QCvCamView::onFpsChanged(int fps)
{
    if (fps <= 0)
    {
        emit invalidFpsError();
        return;
    }

    m_updateTimer->stop();
    m_fps = fps;
    update();
    m_updateTimer->start(1000 / fps);
}

void QCvCamView::paintEvent(QPaintEvent* event)
{
    if (m_isRendering && m_cap->isOpened())
    {
        QPainter painter(this);
        m_cap->read(m_frame);
        execFilters(m_frame, m_frame);
        if (!m_frame.empty())
        {
            painter.setRenderHints(QPainter::Antialiasing, true); //抗锯齿
            painter.scale(rect().width() * 1.0 / m_frame.cols, rect().height() * 1.0 / m_frame.rows);
            painter.drawImage(0, 0, QCvDataUtils::cvMatToQImage(m_frame));
        }
        else
        {
            emit emptyFrameError();
        }
    }
}

void QCvCamView::execFilters(cv::Mat& inMat, cv::Mat& outMat)
{
    foreach (QCvMatFilter* filter, m_filters)
    {
        filter->filter(inMat, outMat);
    }
}
