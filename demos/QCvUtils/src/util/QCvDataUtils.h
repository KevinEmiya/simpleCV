#ifndef QCVDATAUTILS_H
#define QCVDATAUTILS_H

class QImage;

#include <opencv2/core.hpp>
#include <QLabel>

class QCvDataUtils
{
public:
    static QImage cvMatToQImage(const cv::Mat& mat);
};

#endif // QCVDATAUTILS_H
