#include "QCvDataUtils.h"

#include <QDebug>
#include <QImage>

QImage QCvDataUtils::cvMatToQImage(const cv::Mat& mat)
{
    const unsigned char* data = mat.data;
    int width = mat.cols;
    int height = mat.rows;
    int bytesPerLine = static_cast<int>(mat.step);
    switch (mat.type())
    {
        // 8bit, ARGB
        case CV_8UC4:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_ARGB32);
            return image;
        }
        // 8bit, BGR
        case CV_8UC3:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_RGB888);
            //swap blue and red channel
            return image.rgbSwapped();
        }
        // 8bit, Grayshale
        case CV_8UC1:
        {
            QImage image(data, width, height, bytesPerLine,
                         QImage::Format_Grayscale8);
            return image;
        }
        default:
        {
            // Unsupported format
            qWarning() << "Unsupported cv::Mat tpye: " << mat.type()
                       << ", Empty QImage will be returned!";
            return QImage();
        }
    }
}
