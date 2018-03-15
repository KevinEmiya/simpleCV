#include "QCvCamera.h"

#include <QFileDialog>
#include <QDebug>

QCvCamera::QCvCamera(QObject* parent) : QObject(parent)
{
}

bool QCvCamera::isIntrinsicValid() const
{
    bool ret = !m_intrinsic.cameraMat.empty();
    ret &= !m_intrinsic.distortCoeff.empty();
    return ret;
}

bool QCvCamera::loadCalibrationData(const QString& fileName)
{
    if (!fileName.isEmpty())
    {
        cv::FileStorage fs(fileName.toStdString().c_str(), cv::FileStorage::READ);
        if (fs.isOpened())
        {
            cv::Mat camMat = cv::Mat_<double>::zeros(3, 3);
            cv::Mat distortMat = cv::Mat_<double>::zeros(5, 1);
            cv::FileNode camMatNode = fs["camera_matrix"];
            if (!camMatNode.empty() && camMatNode.isMap())
            {
                camMatNode >> camMat;
            }
            cv::FileNode distortNode = fs["distortion_coefficients"];
            if (!distortNode.empty() && distortNode.isMap())
            {
                distortNode >> distortMat;
            }
            if (!camMat.empty() && !distortMat.empty())
            {
                m_intrinsic.cameraMat = camMat;
                m_intrinsic.distortCoeff = distortMat;
                return true;
            }
            else
            {
                qWarning() << "Intrinsic config data not valid!";
                return false;
            }
        }
        else
        {
            qWarning() << "Could not open intrinsic config file!";
            return false;
        }
    }
    else
    {
        qWarning() << "Invalid file name!";
        return false;
    }
}
