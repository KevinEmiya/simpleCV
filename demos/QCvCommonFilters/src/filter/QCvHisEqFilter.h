#ifndef QCVHISEQFILTER_H
#define QCVHISEQFILTER_H

#include "filter/QCvMatFilter.h"

class QCvHisEqFilter : public QCvMatFilter
{
  public:
    QCvHisEqFilter(QString name) : QCvMatFilter(name) {}

  protected:
    void execFilter(const cv::Mat& inMat, cv::Mat& outMat);
};

#endif // QCVHISEQFILTER_H
