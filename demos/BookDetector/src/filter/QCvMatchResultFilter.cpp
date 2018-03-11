#include "QCvMatchResultFilter.h"

Mat QCvMatchResultFilter::execFilter(const Mat &mat)
{
    if(mat.empty() || m_detector == NULL)
    {
        return mat;
    }
    else
    {
        if(m_detector->findPatternFromScene(mat))
        {
            return mat;
        }
        else
        {
            return mat;
        }
    }
}
