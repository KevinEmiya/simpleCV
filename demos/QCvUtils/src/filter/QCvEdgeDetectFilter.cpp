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
    switch (m_type)
    {
        case OPERATOR_TYPE::Canny:
        {
            cv::Mat ret;
            cv::Canny(mat, ret, m_lowerThres, m_higherThres);
            return ret;
        }
        default:
        {
            qWarning() << "Operator not supported! ";
            return mat;
        }
    }
}
