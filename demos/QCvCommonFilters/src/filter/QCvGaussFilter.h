#ifndef QCVGAUSSFILTER_H
#define QCVGAUSSFILTER_H

#include "filter/QCvMatFilter.h"

class QCvGaussFilter : public QCvMatFilter
{
  public:
    QCvGaussFilter(QString name) : QCvMatFilter(name)
    {
        m_kernelSize = cv::Size(5, 5);
        m_sigma = 5;
    }

  public:
    void setKernelSize(unsigned int size);

  protected:
    void execFilter(const cv::Mat& inMat, cv::Mat& outMat);

  private:
    cv::Size m_kernelSize;
    unsigned int m_sigma;
};

#endif // QCVGAUSSFILTER_H
