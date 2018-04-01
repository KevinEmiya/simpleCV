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
            const QCvCamera* camera = QCvMatFilter::camera();
            if (camera != NULL && camera->isIntrinsicValid() &&
                m_detector->computePose(camera->intrinsic()))
            {
                //draw 3d
                m_detector->tracker()->draw3DCube(outMat, camera->intrinsic());
            }
            else
            {
                //draw 2d
                m_detector->tracker()->draw2DContour(outMat);
            }
        }
    }
}
