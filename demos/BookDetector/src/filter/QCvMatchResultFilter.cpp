#include "QCvMatchResultFilter.h"

void QCvMatchResultFilter::execFilter(const Mat& inMat, Mat& outMat)
{
    outMat = inMat.clone();
    if (inMat.empty() || m_detector == NULL)
    {
        return;
    }
    else
    {
        Pose pose;
        if (m_detector->findPatternFromScene(inMat))
        {
            if (m_camera != NULL && m_camera->isIntrinsicValid() &&
                m_detector->computePose(m_camera->intrinsic()))
            {
                //draw 3d
                m_detector->tracker()->draw3DCube(outMat, m_camera->intrinsic());
            }
            else
            {
                //draw 2d
                m_detector->tracker()->draw2DContour(outMat);
            }
        }
    }
}
