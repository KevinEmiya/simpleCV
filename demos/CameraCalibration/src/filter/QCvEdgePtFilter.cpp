#include "QCvEdgePtFilter.h"

#include <opencv2/calib3d.hpp>

#include <QDebug>

cv::Mat QCvEdgePtFilter::execFilter(const cv::Mat& mat)
{
    if (m_colCnt <= 1 || m_rowCnt <= 1)
    {
        qDebug() << "BOARD SIZE EMPTY!";
        return mat;
    }

    cv::Mat retMat = mat.clone();
    cv::Mat frameGray;
    cv::cvtColor(retMat, frameGray, cv::COLOR_BGR2GRAY);
    //-- Detect corners
    m_pointbuf.clear();
    bool found = cv::findChessboardCorners(frameGray, cv::Size(m_colCnt, m_rowCnt),
                                           m_pointbuf, cv::CALIB_CB_ADAPTIVE_THRESH |
                                                           cv::CALIB_CB_FAST_CHECK);
    if (found)
    {
        cv::cornerSubPix(frameGray, m_pointbuf, cv::Size(11, 11),
                         cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
                                          30, 0.1));
        m_imgSize = retMat.size();
        for (size_t i = 0; i < m_pointbuf.size(); i++)
        {
            cv::ellipse(retMat, m_pointbuf[i],
                        cv::Size(5, 5), 0, 0, 360,
                        cv::Scalar(255, 0, 255), 4, 8, 0);
        }
    }
    return retMat;
}

void QCvEdgePtFilter::onCapture()
{
    if(!m_pointbuf.empty())
    {
        emit imgPtsCaptured(m_pointbuf, m_colCnt, m_rowCnt, m_imgSize);
    }
}
