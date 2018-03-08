#ifndef QCVMATFILTER_H
#define QCVMATFILTER_H

#include <opencv2/core.hpp>

#include <QObject>
#include <QString>

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
