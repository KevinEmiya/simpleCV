#ifndef QCVHISEQFILTER_H
#define QCVHISEQFILTER_H

#include "filter/QCvMatFilter.h"

class QCvHisEqFilter : public QCvMatFilter
{
  public:
    QCvHisEqFilter(QString name) : QCvMatFilter(name) {}

  protected:
    cv::Mat execFilter(const cv::Mat &mat);
};

#endif // QCVHISEQFILTER_H
