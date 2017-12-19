#ifndef QCVDATAUTILS_H
#define QCVDATAUTILS_H

class QImage;

#include <opencv2/opencv.hpp>

class QCvDataUtils
{
public:
    static QImage cvMatToQImage(const cv::Mat& mat);
};

#endif // QCVDATAUTILS_H
