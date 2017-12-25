#include "QCvHisEqFilter.h"

#include "opencv2/imgproc.hpp"

cv::Mat QCvHisEqFilter::execFilter(const cv::Mat& mat)
{
    if (mat.type() != CV_8UC3)
    {
        return mat;
    }

    // Transform into HSV space
    cv::Mat hsvMat;
    cv::cvtColor(mat, hsvMat, cv::COLOR_BGR2HSV);
    // Equalize Histogram of H-Channel
    cv::Mat hsvMats[3];
    cv::split(hsvMat, hsvMats);
    cv::equalizeHist(hsvMats[2].clone(), hsvMats[2]);
    cv::merge(hsvMats, 3, hsvMat);
    // Transform back into BGR space
    cv::Mat retMat;
    cv::cvtColor(hsvMat, retMat, cv::COLOR_HSV2BGR);
    return retMat;
}
