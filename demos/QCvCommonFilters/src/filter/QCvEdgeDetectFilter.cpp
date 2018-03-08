#include "QCvEdgeDetectFilter.h"
#include "opencv2/imgproc.hpp"

#include <QDebug>

void QCvEdgeDetectFilter::setThresholds(int lower, int higher)
{
    m_lowerThres = lower;
    m_higherThres = higher;
}

cv::Mat QCvEdgeDetectFilter::execFilter(const cv::Mat& mat)
{
    cv::Mat ret;
    // Canny Detector
    cv::Canny(mat, ret, m_lowerThres, m_higherThres);
    return ret;
}
