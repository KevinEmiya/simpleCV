#ifndef QCVGAUSSFILTER_H
#define QCVGAUSSFILTER_H

#include "QCvMatFilter.h"

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
    cv::Mat execFilter(const cv::Mat &mat);

private:
    cv::Size m_kernelSize;
    unsigned int m_sigma;
};

#endif // QCVGAUSSFILTER_H
