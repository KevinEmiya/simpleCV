#include "QCvDataUtils.h"

#include <QImage>

QImage QCvDataUtils::cvMatToQImage(const cv::Mat& mat)
{
    return QImage((const unsigned char*) (mat.data),
                           mat.cols, mat.rows,
                           QImage::Format_RGB888)
                        .rgbSwapped();
}
