#include "QCvHisEqFilter.h"

#include "opencv2/imgproc.hpp"

void QCvHisEqFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    if (inMat.type() != CV_8UC3)
    {
        outMat = inMat.clone();
        return;
    }

    // Transform into HSV space
    cv::Mat hsvMat;
    cv::cvtColor(inMat, hsvMat, cv::COLOR_BGR2HSV);
    // Equalize Histogram of H-Channel
    cv::Mat hsvMats[3];
    cv::split(hsvMat, hsvMats);
    cv::equalizeHist(hsvMats[2].clone(), hsvMats[2]);
    cv::merge(hsvMats, 3, hsvMat);
    // Transform back into BGR space
    cv::cvtColor(hsvMat, outMat, cv::COLOR_HSV2BGR);
}
