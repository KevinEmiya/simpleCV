#ifndef QCVCAMVIEW_H
#define QCVCAMVIEW_H

#include <QWidget>
#include <opencv2/videoio.hpp>
#include <QTimer>
#include <QList>

#include "component/QCvCamera.h"
#include "util/QCvDataUtils.h"
#include "filter/QCvMatFilter.h"

class QCvCamView : public QWidget
{
    Q_OBJECT
public:
    explicit QCvCamView(QWidget *parent = 0);
    virtual ~QCvCamView();

public:
    void appendFilter(QCvMatFilter* filter);
    void setFilterEnabled(QString name, bool enabled);
    cv::Mat currentFrame();
    bool updateCalibrarion(QString fileName);

signals:
    void camOpenError();
    void emptyFrameError();
    void invalidFpsError();

public slots:
    void onStreamSwitch(bool open);
    void onRenderSwitch(bool open);
    void onFpsChanged(int fps);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void execFilters(const cv::Mat& inMat, cv::Mat& outMat);

private:
    cv::VideoCapture* m_cap;
    QCvCamera* m_camera;
    bool m_isRendering;

    cv::Mat m_frame;
    int m_fps;
    QTimer* m_updateTimer;

    QList<QCvMatFilter*> m_filters;
};

#endif // QCVCAMVIEW_H
