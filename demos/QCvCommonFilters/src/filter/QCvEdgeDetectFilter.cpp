#include "QCvEdgeDetectFilter.h"
#include "opencv2/imgproc.hpp"

#include <QDebug>

void QCvEdgeDetectFilter::setThresholds(int lower, int higher)
{
    m_lowerThres = lower;
    m_higherThres = higher;
}

void QCvEdgeDetectFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    // Canny Detector
    cv::Canny(inMat, outMat, m_lowerThres, m_higherThres);
}
