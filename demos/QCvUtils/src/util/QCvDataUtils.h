#ifndef QCVDATAUTILS_H
#define QCVDATAUTILS_H

class QImage;

#include <opencv2/core.hpp>
#include <QLabel>

class QCvDataUtils
{
public:
    static QImage cvMatToQImage(const cv::Mat& mat);
    static void showCvMat2D(const cv::Mat_<float>& mat);
};

#endif // QCVDATAUTILS_H
