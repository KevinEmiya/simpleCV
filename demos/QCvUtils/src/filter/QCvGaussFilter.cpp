#include "QCvGaussFilter.h"

#include "opencv2/imgproc.hpp"

void QCvGaussFilter::setKernelSize(unsigned int size)
{
    m_kernelSize = cv::Size(size, size);
    m_sigma = size;
}

cv::Mat QCvGaussFilter::execFilter(const cv::Mat &mat)
{
    cv::Mat ret;
    cv::GaussianBlur(mat, ret, m_kernelSize, m_sigma);
    return ret;
}
