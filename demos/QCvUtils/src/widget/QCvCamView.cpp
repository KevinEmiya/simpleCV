#include "QCvCamView.h"

#include <QImage>
#include <QPainter>
#include <QDebug>

#include "util/QCvDataUtils.h"

QCvCamView::QCvCamView(QWidget *parent) : QWidget(parent)
{
    m_cap = new cv::VideoCapture();
    m_fps = 60;
    m_updateTimer = new QTimer(this);
    m_updateTimer->start(1000 / m_fps);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
}

QCvCamView::~QCvCamView()
{
    if(m_cap != NULL)
    {
        delete m_cap;
        m_cap = NULL;
    }
}

void QCvCamView::onStreamSwitch(bool open)
{
    if(open && !m_streamOpen) //打开摄像头
    {
        m_streamOpen = true;
        if(!m_cap->isOpened())
        {
            m_cap->open(0);
            if(!m_cap->isOpened())
            {
                emit camOpenError();
            }
        }
        update();
    }
    else if (!open && m_streamOpen) //关闭摄像头
    {
        m_streamOpen = false;
        update();
    }
}

void QCvCamView::onFpsChanged(int fps)
{
    if(fps <= 0)
    {
        emit invalidFpsError();
        return;
    }

    m_updateTimer->stop();
    m_fps = fps;
    update();
    m_updateTimer->start(1000 / fps);
}

void QCvCamView::paintEvent(QPaintEvent *event)
{
    if(m_streamOpen && m_cap->isOpened())
    {
        QPainter painter(this);
        m_cap->read(m_frame);
        if(!m_frame.empty())
        {
            painter.setRenderHints(QPainter::Antialiasing, true);//抗锯齿
            painter.scale(rect().width() * 1.0 / m_frame.cols, rect().height() * 1.0 / m_frame.rows);
            painter.drawImage(0, 0, QCvDataUtils::cvMatToQImage(m_frame));
        }
        else
        {
            emit emptyFrameError();
        }
    }
}
