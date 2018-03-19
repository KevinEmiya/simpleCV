#ifndef QCVEDGEDETECTFILTER_H
#define QCVEDGEDETECTFILTER_H

#include "filter/QCvMatFilter.h"

#include <QString>

class QCvEdgeDetectFilter : public QCvMatFilter
{
  public:
    QCvEdgeDetectFilter(QString name, QObject* parent = nullptr);

  public:
    void setThresholds(int thres);

  protected:
    void execFilter(const cv::Mat& mat, cv::Mat& outMat);

  private:
    int m_thres;
};

#endif // QCVEDGEDETECTFILTER_H
