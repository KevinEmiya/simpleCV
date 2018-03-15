#ifndef QCVEDGEPTFILTER_H
#define QCVEDGEPTFILTER_H

#include "filter/QCvMatFilter.h"

#include <QObject>
#include <opencv2/imgproc.hpp>
#include <vector>

class QCvEdgePtFilter : public QCvMatFilter
{
    Q_OBJECT
  public:
    explicit QCvEdgePtFilter(QString name, QObject* parent = nullptr) : QCvMatFilter(name, parent) {}
    virtual ~QCvEdgePtFilter() { m_pointbuf.clear(); }

  public:
    void setColCnt(const int& colCnt) { m_colCnt = colCnt; }
    void setRowCnt(const int& rowCnt) { m_rowCnt = rowCnt; }

  signals:
    void imgPtsCaptured(const std::vector<cv::Point2f>& imgPts,
                        const int& colCnt, const int& rowCnt,
                        const cv::Size& imgSize);

  public slots:
    void onCapture();

  protected:
    virtual void execFilter(const cv::Mat& inMat, cv::Mat& outMat);

  private:
    int m_colCnt;
    int m_rowCnt;
    std::vector<cv::Point2f> m_pointbuf;
    cv::Size m_imgSize;
};

#endif // QCVEDGEPTFILTER_H
