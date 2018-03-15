#include "QCvGaussFilter.h"

#include "opencv2/imgproc.hpp"

void QCvGaussFilter::setKernelSize(unsigned int size)
{
    m_kernelSize = cv::Size(size, size);
    m_sigma = size;
}

void QCvGaussFilter::execFilter(const cv::Mat& inMat, cv::Mat& outMat)
{
    cv::GaussianBlur(inMat, outMat, m_kernelSize, m_sigma);
}
