#include "QCvFaceDetectFilter.h"

#include <opencv2/imgproc.hpp>

bool QCvFaceDetectFilter::load(QString fileName)
{
    return m_classifier.load(fileName.toStdString().c_str());
}

bool QCvFaceDetectFilter::isClassifierValid()
{
    return !m_classifier.empty();
}

void QCvFaceDetectFilter::execFilter(const cv::Mat& inMat, cv::Mat &outMat)
{
    outMat = inMat.clone();
    if (m_classifier.empty())
    {
        return;
    }

    std::vector<cv::Rect> faces;
    cv::Mat frameGray;
    cv::cvtColor(outMat, frameGray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frameGray, frameGray);
    //-- Detect faces
    m_classifier.detectMultiScale(frameGray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE,
                                  cv::Size(30, 30));
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        cv::ellipse(outMat, center,
                    cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360,
                    cv::Scalar(255, 0, 255), 4, 8, 0);
    }
}
