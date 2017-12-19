#include "QCvEdgeDetectFilter.h"
#include "opencv2/imgproc.hpp"

#include <QDebug>

void QCvEdgeDetectFilter::setThresholds(int lower, int higher)
{
    m_lowerThres = lower;
    m_higherThres = higher;
}

cv::Mat QCvEdgeDetectFilter::execFilter(cv::Mat& mat)
{
    switch (m_type)
    {
        case OPERATOR_TYPE::Canny:
        {
            cv::Canny(mat.clone(), mat, m_lowerThres, m_higherThres);
            return mat;
        }
        default:
        {
            qWarning() << "Operator not supported! ";
            return mat;
        }
    }
}
