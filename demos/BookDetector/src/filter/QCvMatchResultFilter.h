#ifndef QCVMATCHRESULTFILTER_H
#define QCVMATCHRESULTFILTER_H

#include "component/PatternDetector.h"
#include "filter/QCvMatFilter.h"

class QCvMatchResultFilter : public QCvMatFilter
{
  public:
    QCvMatchResultFilter(PatternDetector* detector, QObject* parent = nullptr, QString name = "matchRet")
        : QCvMatFilter(name, parent) { m_detector = detector; }

  protected:
    virtual Mat execFilter(const Mat& mat);

  private:
    PatternDetector* m_detector;
};

#endif // QCVMATCHRESULTFILTER_H
