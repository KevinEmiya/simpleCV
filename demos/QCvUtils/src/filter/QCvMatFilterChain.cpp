#include "QCvMatFilterChain.h"

QCvMatFilterChain::QCvMatFilterChain(QObject* parent) : QObject(parent)
{
}

QCvMatFilterChain::~QCvMatFilterChain()
{
    qDeleteAll(m_filters);
}

void QCvMatFilterChain::append(QCvMatFilter* filter)
{
    m_filters.append(filter);
}

void QCvMatFilterChain::setEnabled(QString filterName, bool enabled)
{
    foreach (QCvMatFilter* filter, m_filters)
    {
        if(filter->name() == filterName)
        {
            filter->setEnabled(enabled);
        }
    }
}


cv::Mat QCvMatFilterChain::execFilter(const cv::Mat& mat)
{
    cv::Mat retMat = mat;
    foreach (QCvMatFilter* filter, m_filters)
    {
        retMat = filter->filter(mat);
    }
    return retMat;
}
