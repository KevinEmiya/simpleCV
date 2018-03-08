#include "CamCalibrator.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/core/persistence.hpp>

#include <QDebug>
#include <QDateTime>

#define MAX_DATA_CNT 1000

CamCalibrator::CamCalibrator(QObject* parent) : QObject(parent)
{
    m_squareHeight = 0.0;
    m_squareWidth = 0.0;

    m_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    m_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
}

void CamCalibrator::onImgPts(const std::vector<cv::Point2f>& imgPts,
                             const int& colCnt, const int& rowCnt,
                             const cv::Size& imgSize)
{
    if (m_squareHeight >= 0 && m_squareWidth >= 0 && imgPts.size() > 0)
    {
        checkAndPushData(m_imgPts, imgPts);
        //calculate object points;
        std::vector<cv::Point3f> corners;
        for (int i = 0; i < rowCnt; i++)
        {
            for (int j = 0; j < colCnt; j++)
            {
                corners.push_back(cv::Point3f(float(j * m_squareHeight),
                                              float(i * m_squareWidth), 0));
            }
        }
        checkAndPushData(m_objPts, corners);
        runCalibration(imgSize);
        showParams();
    }
}

void CamCalibrator::reset()
{
    m_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    m_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
    m_objPts.clear();
    m_imgPts.clear();
}

void CamCalibrator::saveResults(QString fileName)
{
    cv::FileStorage fs(fileName.toStdString().c_str(), cv::FileStorage::WRITE);
    fs << "calibration_time" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString().c_str();
    fs << "nframes" << (int) m_objPts.size();
    fs << "rms" << m_rms;
    fs << "camera_matrix" << m_cameraMatrix;
    fs << "distortion_coefficients" << m_distCoeffs;
}

void CamCalibrator::runCalibration(const cv::Size& imgSize)
{
    std::vector<cv::Mat> rvecs, tvecs;
    m_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    m_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
    m_rms = cv::calibrateCamera(m_objPts, m_imgPts,
                                imgSize, m_cameraMatrix, m_distCoeffs, rvecs, tvecs);
}

template <typename T>
void CamCalibrator::checkAndPushData(std::vector<T>& vector, const T& data)
{
    if (vector.size() < MAX_DATA_CNT)
    {
        vector.push_back(data);
    }
    else
    {
        qDebug() << "Data count exeeds limit, removing oldest data...";
        vector.erase(vector.begin());
        vector.push_back(data);
    }
}

void CamCalibrator::showParams()
{
    QString outStr = "Camera Matrix: \n";
    for (int i = 0; i < m_cameraMatrix.rows; i++)
    {
        for (int j = 0; j < m_cameraMatrix.cols; j++)
        {
            outStr.append(QString("%1 ").arg(m_cameraMatrix.at<double>(i, j)));
        }
        outStr.append("\n");
    }
    outStr.append("distortion coefficients: ");
    for (int i = 0; i < m_distCoeffs.rows; i++)
    {
        outStr.append(QString("%1 ").arg(m_distCoeffs.at<double>(i, 0)));
    }
    outStr.append("\n");
    outStr.append(QString("rms = %1\n").arg(m_rms));
    emit calibrated(outStr);
}
