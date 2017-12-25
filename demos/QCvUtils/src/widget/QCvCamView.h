#ifndef QCVCAMVIEW_H
#define QCVCAMVIEW_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QTimer>

class QCvMatFilterChain;
class QCvMatFilter;

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

signals:
    void camOpenError();
    void emptyFrameError();
    void invalidFpsError();

public slots:
    void onStreamSwitch(bool open);
    void onFpsChanged(int fps);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    bool m_streamOpen;
    cv::VideoCapture* m_cap;
    QCvMatFilterChain* m_filterChain;
    cv::Mat m_frame;
    int m_fps;
    QTimer* m_updateTimer;
};

#endif // QCVCAMVIEW_H
