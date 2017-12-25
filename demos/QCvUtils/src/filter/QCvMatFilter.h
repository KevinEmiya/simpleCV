#ifndef QCVMATFILTER_H
#define QCVMATFILTER_H

#include <opencv2/core.hpp>

#include <QString>

class QCvMatFilter
{
  public:
    QCvMatFilter(QString name) : m_name(name)
    {
        m_enabled = true;
    }

  public:
    QString name() { return m_name; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    cv::Mat filter(const cv::Mat& mat)
    {

        return m_enabled ? execFilter(mat) : mat;
    }

  protected:
    virtual cv::Mat execFilter(const cv::Mat& mat) = 0;

  protected:
    QString m_name;
    bool m_enabled;
};

#endif // QCVMATFILTER_H
