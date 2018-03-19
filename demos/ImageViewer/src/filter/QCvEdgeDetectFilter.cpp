#include "QCvEdgeDetectFilter.h"
#include "opencv2/imgproc.hpp"

#include <QDebug>

QCvEdgeDetectFilter::QCvEdgeDetectFilter(QString name, QObject* parent) : QCvMatFilter(name, parent)
{
    m_thres = 140;
}

void QCvEdgeDetectFilter::setThresholds(int thres)
{
    m_thres = thres;
}

void QCvEdgeDetectFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    // Canny Detector
    cv::Canny(inMat, outMat, m_thres, m_thres * 3);
}
