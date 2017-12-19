#ifndef QCVEDGEDETECTFILTER_H
#define QCVEDGEDETECTFILTER_H

#include "QCvMatFilter.h"

#include <QString>

enum OPERATOR_TYPE
{
    Canny,
    Sobel,
    LoG
};

class QCvEdgeDetectFilter : public QCvMatFilter
{
  public:
    QCvEdgeDetectFilter(QString name, OPERATOR_TYPE type = OPERATOR_TYPE::Canny)
        : m_type(type), QCvMatFilter(name)
    {
        m_lowerThres = 160;
        m_higherThres = 240;
    }

  public:
    void setThresholds(int lower, int higher);
    cv::Mat execFilter(const cv::Mat& mat);

  private:
    OPERATOR_TYPE m_type;
    int m_lowerThres;
    int m_higherThres;
};

#endif // QCVEDGEDETECTFILTER_H
