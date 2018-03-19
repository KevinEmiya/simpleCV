#ifndef QCVMATFILTER_H
#define QCVMATFILTER_H

#include <opencv2/core.hpp>

#include <QObject>
#include <QString>

#include "component/QCvCamera.h"

class QCvMatFilter : public QObject
{
    Q_OBJECT
  public:
    QCvMatFilter(QString name, QObject* parent = nullptr) : QObject(parent)
    {
        m_enabled = true;
        m_name = name;
    }

  public:
    QString name() { return m_name; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    void filter(const cv::Mat& inMat, cv::Mat& outMat)
    {
        if (m_enabled)
        {
            execFilter(inMat, outMat);
        }
        else
        {
            outMat = inMat.clone();
        }
    }

    void setCamera(const QCvCamera* camera)
    {
        m_camera = camera;
    }

  protected:
    virtual void execFilter(const cv::Mat& mat, cv::Mat& outMat) = 0;

  private:
    QString m_name;
    bool m_enabled;
    // intrinsic parameters of the camera
    const QCvCamera* m_camera;
};

#endif // QCVMATFILTER_H
